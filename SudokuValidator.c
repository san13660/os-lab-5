//Christopher Sandoval 13660

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <pthread.h>
#include<omp.h>

int a[9][9];
bool error = false;

void *checkCol(){
	omp_set_nested(true);
	omp_set_num_threads(9);
	int i;
	int j;
	
	#pragma omp parallel for shared(a) private(i, j)
	for(j=0;j<9;j++)
	{
		printf("El thread en el que se ejecuta la revision de columnas es: %d\n", syscall(SYS_gettid));
		bool rowCheck[9] = {false,false,false,false,false,false,false,false,false};
		for(i=0;i<9;i++){
			if(!rowCheck[a[i][j]-1]){
				rowCheck[a[i][j]-1] = true;
			}else{
				printf("ERROR CHECK COL\n");
				printf("FILA:%d \n", i);
				printf("COLUMNA:%d \n", j);
				error = true;
			}
		}
	}
}

void *checkRow(){
	omp_set_nested(true);
	omp_set_num_threads(9);

	int i;
	int j;

	#pragma omp parallel for shared(a) private(i, j) schedule(dynamic)
	for(i=0;i<9;i++)
	{
		bool rowCheck[9] = {false,false,false,false,false,false,false,false,false};
		for(j=0;j<9;j++){
			if(!rowCheck[a[i][j]-1]){
				rowCheck[a[i][j]-1] = true;
			}else{
				printf("ERROR CHECK ROW\n");
				printf("FILA:%d \n", i);
				printf("COLUMNA:%d \n", j);
				error = true;
			}
		}
	}
}

void *checkSquare(){
	int i;
	int j;
	int x;
	int y;
	for(x=0;x<3;x++){
		for(y=0;y<3;y++){
			bool rowCheck[9] = {false,false,false,false,false,false,false,false,false};
			for(i=x*3;i<((x*3)+3);i++){
				for(j=y*3;j<((y*3))+3;j++){
					if(!rowCheck[a[i][j]-1]){
						rowCheck[a[i][j]-1] = true;
					}else{
						printf("ERROR CHECK SQUARE\n");
						printf("FILA:%d \n", i);
						printf("COLUMNA:%d \n", j);
						error = true;
					}
				}
			}
		}
	}
}


int main(){
	//1

    FILE *filePointer;
    char ch;
	char solucion[81];
	int contador = 0;

    filePointer = fopen("sudoku", "r");

    if (filePointer == NULL)
    {
        printf("File is not available \n");
    }
    else
    {
        while ((ch = fgetc(filePointer)) != EOF || contador < 82)
        {
		solucion[contador] = ch;
		contador++;
        }
    }
    fclose(filePointer);

	int i;
	int j;

	for(i=0;i<9;i++)
	{
	    for(j=0;j<9;j++)
	    {
	    	a[i][j]=(int)solucion[(i*9)+j]-'0';
	    }
	}


	
	int pidPadre = getpid();
	char buf[8];
	snprintf(buf, 8, "%d", pidPadre);

	int var1 = fork();
	if ( var1 == 0 ) {
		execlp("ps", "ps", "-p", buf, "-lLf", (char *) NULL);
	}else{
		pthread_t thread;
		pthread_create(&thread, NULL, checkCol, NULL);
		pthread_join(thread, NULL);
		printf("El thread en el que se ejecuta el main es: %d\n", syscall(SYS_gettid));
		wait(NULL);
		checkRow();
		if(error){
			printf("\nSUDOKU INCORRECTO\n");
		}else{
			printf("\nSUDOKU CORRECTO\n");
		}
		int var2 = fork();
		if ( var2 == 0 ) {
			execlp("ps", "ps", "-p", buf, "-lLf", (char *) NULL);
		}else{
			wait(NULL);
		}
	}

	return 0;
}


