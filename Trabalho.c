/*
  Author: Lucas Coutinho de Souza Oliveira
  Date: 27/02/2013 23:30
  Description: Introdu��o � Programa��o Multithread com PThreads
  Version: 1.0.2
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

/* Constantes */
#define SEED    10		//Seed para gera��o dos n�mero aleat�rios
#define RANGE_NUM 30000 //Range dos n�meros aleat�rios (0 a (RANGE_NUM-1) )

#define THREADS 2	//Quantidade de threads que rodar�o no programa

#define LIN	10000		//Numero de linhas da matriz
#define COL	10000		//NUmero de colunas da matriz

#define MB_LIN 10000		//Numero de linhas  do macrobloco.
#define MB_COL 5000		//Numero de colunas do macrobloco.

#define FALSE 0	// "Cria��o" do valor booleano FALSO.
#define TRUE  1	// "Cria��o" do valor booleano VERDADEIRO.

/* Prot�tipos */
int ePrimo(int num);
void preencheMatriz(int linhas, int colunas);
void *verificar_primos(void *parametro);

/* Vari�veis globais */
int contador=0;	//Contador de primos.
int mb_prox_col=0;//�ndice de coluna do pr�ximo macrobloco.
int mb_prox_lin=0;//�ndice de linha  do pr�ximo macrobloco.
int matriz[LIN][COL];
pthread_mutex_t mutex_cont;	//Mutex da vari�vel contador.
pthread_mutex_t mutex_mb;	//Mutex das vari�veis mb_prox_col e mb_prox_lin.

/* Fun��o Principal */
int main()
{
    int i;
	clock_t time_ini;
	pthread_t threads[THREADS];

	time_ini = clock();

	pthread_mutex_init(&mutex_cont, NULL);
	pthread_mutex_init(&mutex_mb, NULL);

    preencheMatriz(LIN, COL);

    for(i=0; i<THREADS; i++){
		pthread_create(&threads[i], NULL, verificar_primos, NULL);
	}

	for (i = 0; i < THREADS; i++){
		if (pthread_join(threads[i], NULL) != 0) {
			printf("Pthread Join Erro!\n");
			exit(1); 
		}
	}

    printf("Existem %d primos na matriz!\n", contador);
	printf("Tempo: %f segundos!\n", ((double) (clock() - time_ini)/(double)CLOCKS_PER_SEC ));
	system("pause");
    return 0;
}

/* Fun��es Auxiliares */
int ePrimo(int num){
    int i;

    if(num<2) return FALSE; //Por defini��o o menor n�mero primo � 2
    if(num==2) return TRUE; //Se o n�mero for 2, � um n�mero primo!
    if(num%2 == 0) return FALSE; //Se o n�mero for par, ent�o n�o � primo!

	//Testa se existem divisores n�o-pares de 3 a raiz quadrada do numero testado.
    for( i=3; i<= (sqrt((double)num)); i+=2 ){ 
        if(num%i == 0)
            return FALSE; // Caso exista, o n�mero n�o � primo.
    }

    return TRUE; //Caso n�o exista, o n�mero � primo.
}

void preencheMatriz(int linhas, int colunas){
    int i, j;

    srand(SEED); //Define o Seed para a fun��o rand();
    for(i=0; i<linhas; i++){
        for(j=0; j<colunas; j++){
                matriz[i][j]= rand()%RANGE_NUM; //Preenche cada posi��o da matriz
        }
    }
}

void *verificar_primos(void *parametro){
	while(1){
		int i=0, j=0, k=0;
		int cont_thread = 0;
		int cmax, lmax;

		pthread_mutex_lock( &mutex_mb ); //Entra na SE��O CR�TICA - INDICES DE MACROBLOCO
		
		if( mb_prox_col < COL ){
			i = mb_prox_lin;
			k = j = mb_prox_col;

			mb_prox_col += MB_COL;
			//pthread_mutex_unlock( &mutex_mb );
		}
		else{
			if( (mb_prox_lin + MB_LIN) < LIN ){
				k = j = 0;
				mb_prox_col = MB_COL;
			
				i = mb_prox_lin += MB_LIN;

				//pthread_mutex_unlock( &mutex_mb );
			}
			else{
				pthread_mutex_unlock( &mutex_mb );//Sai da SE��O CR�TICA - INDICES DE MACROBLOCO
				return NULL;	//Encerra a thread
			}
		}

		pthread_mutex_unlock( &mutex_mb ); //Sai da SE��O CR�TICA - INDICES DE MACROBLOCO

		cmax = j + MB_COL;
		lmax = i + MB_LIN;
		for(; i < lmax; i++){
			for(j=k; j < cmax; j++){
				if( ePrimo(matriz[i][j]) ){
					cont_thread++;
				}
			}
		}

		pthread_mutex_lock( &mutex_cont ); //Entra na SE��O CRITICA - CONTADOR DE PRIMOS
		contador += cont_thread;
		pthread_mutex_unlock( &mutex_cont );//Sai da SE��O CRITICA - CONTADOR DE PRIMOS
	}
}
