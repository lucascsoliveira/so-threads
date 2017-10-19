/*
  Author: Lucas Coutinho de Souza Oliveira
  Date: 27/02/2013 23:30
  Description: Introdução à Programação Multithread com PThreads
  Version: 1.0.2
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

/* Constantes */
#define SEED    10		//Seed para geração dos número aleatórios
#define RANGE_NUM 30000 //Range dos números aleatórios (0 a (RANGE_NUM-1) )

#define THREADS 2	//Quantidade de threads que rodarão no programa

#define LIN	10000		//Numero de linhas da matriz
#define COL	10000		//NUmero de colunas da matriz

#define MB_LIN 10000		//Numero de linhas  do macrobloco.
#define MB_COL 5000		//Numero de colunas do macrobloco.

#define FALSE 0	// "Criação" do valor booleano FALSO.
#define TRUE  1	// "Criação" do valor booleano VERDADEIRO.

/* Protótipos */
int ePrimo(int num);
void preencheMatriz(int linhas, int colunas);
void *verificar_primos(void *parametro);

/* Variáveis globais */
int contador=0;	//Contador de primos.
int mb_prox_col=0;//Índice de coluna do próximo macrobloco.
int mb_prox_lin=0;//Índice de linha  do próximo macrobloco.
int matriz[LIN][COL];
pthread_mutex_t mutex_cont;	//Mutex da variável contador.
pthread_mutex_t mutex_mb;	//Mutex das variáveis mb_prox_col e mb_prox_lin.

/* Função Principal */
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

/* Funções Auxiliares */
int ePrimo(int num){
    int i;

    if(num<2) return FALSE; //Por definição o menor número primo é 2
    if(num==2) return TRUE; //Se o número for 2, é um número primo!
    if(num%2 == 0) return FALSE; //Se o número for par, então não é primo!

	//Testa se existem divisores não-pares de 3 a raiz quadrada do numero testado.
    for( i=3; i<= (sqrt((double)num)); i+=2 ){ 
        if(num%i == 0)
            return FALSE; // Caso exista, o número não é primo.
    }

    return TRUE; //Caso não exista, o número é primo.
}

void preencheMatriz(int linhas, int colunas){
    int i, j;

    srand(SEED); //Define o Seed para a função rand();
    for(i=0; i<linhas; i++){
        for(j=0; j<colunas; j++){
                matriz[i][j]= rand()%RANGE_NUM; //Preenche cada posição da matriz
        }
    }
}

void *verificar_primos(void *parametro){
	while(1){
		int i=0, j=0, k=0;
		int cont_thread = 0;
		int cmax, lmax;

		pthread_mutex_lock( &mutex_mb ); //Entra na SEÇÃO CRÍTICA - INDICES DE MACROBLOCO
		
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
				pthread_mutex_unlock( &mutex_mb );//Sai da SEÇÃO CRÍTICA - INDICES DE MACROBLOCO
				return NULL;	//Encerra a thread
			}
		}

		pthread_mutex_unlock( &mutex_mb ); //Sai da SEÇÃO CRÍTICA - INDICES DE MACROBLOCO

		cmax = j + MB_COL;
		lmax = i + MB_LIN;
		for(; i < lmax; i++){
			for(j=k; j < cmax; j++){
				if( ePrimo(matriz[i][j]) ){
					cont_thread++;
				}
			}
		}

		pthread_mutex_lock( &mutex_cont ); //Entra na SEÇÃO CRITICA - CONTADOR DE PRIMOS
		contador += cont_thread;
		pthread_mutex_unlock( &mutex_cont );//Sai da SEÇÃO CRITICA - CONTADOR DE PRIMOS
	}
}
