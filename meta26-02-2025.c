#include <stdio.h>
#include <stdlib.h>

void sumMatriz(int *A, int*B, int*C, int n, int i){
    if(i == n*n) return;
    C[i] = A[i] + B[i];
    sumMatriz(A, B, C, n, i+1);
}

void subtractionMatriz(int *A, int *B, int*C, int n, int i){
    if(i == n*n) return;
    C[i] = A[i] - B[i];
    subtractionMatriz(A, B, C, n, i+1);
}

void multiplicationMatriz(int *A, int *B, int *C, int n, int i, int j, int k, int soma){
    //i = Indice da linha atual da matriz C
    //j = Indice da coluna atual da matriz C
    //k = Indice usado para percorrer a linha da matriz A e a coluna da matriz B
    //soma = vaviável acumuladora que guarda a soma dos produtos de A[i][k] * B[k][j]

    if(i == n*n) return;

    if(j==n) {
        multiplicationMatriz(A, B, C, n, i+1, 0, 0, 0);
        return;
    }

    if(k==n){
        C[i * n + j] = soma;
        multiplicationMatriz(A, B, C, n, i, j + 1, 0, 0);
        return;
    }

    multiplicationMatriz(A, B, C, n, i, j, k + 1, soma + A[i * n + k] * B[k * n + j]);

}

void readMatriz(int *M, int n, int i){
    if(i == n*n) return;
    scanf("%d", &M[i]);
    readMatriz(M, n, i+1);
}

void printMatriz(int *M, int n, int i){
    if(i == n*n) return;
    printf("%d ", M[i]);
    if((i+1)%n == 0){
        printf("\n");
    }

    printMatriz(M, n, i+1);

}

int main(){
    int n;
    printf("Digite o tamanho da Matriz NxN: ");
    scanf("%d", &n);

    int *A = (int *)malloc(n * n * sizeof(int));
    int *B = (int *)malloc(n * n * sizeof(int));
    int *C = (int *)malloc(n * n * sizeof(int));

    printf("Digite os elementos da matriz A:\n");
    readMatriz(A, n, 0);

    printf("Digite os elementos da matriz B:\n");
    readMatriz(B, n, 0);

    sumMatriz(A, B, C, n, 0);
    printf("Soma:\n");
    printMatriz(C, n, 0);

    subtractionMatriz(A, B, C, n, 0);
    printf("Subtracao:\n");
    printMatriz(C, n, 0);

    multiplicationMatriz(A, B, C, n, 0, 0, 0, 0);
    printf("Multiplicacao:\n");
    printMatriz(C, n, 0);

    free(A);
    free(B);
    free(C);

    return 0;
}

/*
AUTOR: FELIPE AMORIM DO CARMO SILVA
DATA: 01/03/2025
SESSÃO PBL: 26/02/2025
*/
