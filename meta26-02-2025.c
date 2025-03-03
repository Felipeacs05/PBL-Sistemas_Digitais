#include <stdio.h>

#define MAX_N 5 // Definição do tamanho máximo permitido para a matriz

void sumMatriz(int A[MAX_N][MAX_N], int B[MAX_N][MAX_N], int C[MAX_N][MAX_N], int n, int i, int j){
    // i = Linha
    // j = Coluna
    if(i == n) return;// Se a linha 'i' ultrapassou 'n', a recursão termina
    if(j == n){// Se a coluna 'j' chegou ao limite, passa para a próxima linha
        sumMatriz(A, B, C, n, i + 1, 0);
        return;
    }
    C[i][j] = A[i][j] + B[i][j];
    sumMatriz(A, B, C, n, i, j + 1);// Chamada recursiva
}

void subtractionMatriz(int A[MAX_N][MAX_N], int B[MAX_N][MAX_N], int C[MAX_N][MAX_N], int n, int i, int j){
    // Mesma lógica da soma porem subtraindo
    if(i == n) return;
    if(j == n){
        subtractionMatriz(A, B, C, n, i + 1, 0);
        return;
    }
    C[i][j] = A[i][j] - B[i][j];
    subtractionMatriz(A, B, C, n, i, j+1);
}

void multiplicationMatriz(int A[MAX_N][MAX_N], int B[MAX_N][MAX_N], int C[MAX_N][MAX_N], int n, int i, int j, int k, int soma){
    // i = linha
    // j = coluna
    if(i == n) return;

    if(j == n){
        multiplicationMatriz(A, B, C, n, i + 1, 0, 0, 0);
        return;
    }

    if(k == n){ // Se 'k' chegou ao limite, armazena a soma e passa para a próxima coluna
        C[i][j] = soma;
        multiplicationMatriz(A, B, C, n, i, j+1, 0, 0);
        return;
    }

    // Soma os produtos A[i][k] * B[k][j] para calcular o elemento C[i][j]
    multiplicationMatriz(A, B, C, n, i, j, k + 1, soma + A[i][k] * B[k][j]);
}

void readMatriz(int M[MAX_N][MAX_N], int n, int i, int j){
    if(i==n)return; // Caso base: terminou todas as linhas

    if(j==n){ // Se a coluna 'j' chegou ao limite, avança para a próxima linha
        readMatriz(M, n, i + 1, 0);
        return;
    }

    scanf("%d", &M[i][j]);
    readMatriz(M, n, i, j+ 1);
}

void printMatriz(int M[MAX_N][MAX_N], int n, int i, int j){
    if(i == n) return;

    if (j == n){
        printf("\n");
        printMatriz(M, n, i + 1, 0);
        return;
    }

    printf("%d ", M[i][j]);
    printMatriz(M, n, i, j + 1);

}

int main(){
    int n;
    printf("Digite o tamanho da Mtriz NxN (2-5): ");
    scanf("%d", &n);

    if(n<2 || n>MAX_N){
        printf("Tamanho válido! Escolha entre 2 e 5.\n");
        return 1;
    }

    int A[MAX_N][MAX_N], B[MAX_N][MAX_N], C[MAX_N][MAX_N];

    printf("Digite os elementos da matriz A:\n");
    readMatriz(A, n, 0, 0);

    printf("Digite os elementos da matriz B:\n");
    readMatriz(B, n, 0, 0);

    printf("\nSoma:\n");
    sumMatriz(A, B, C, n, 0, 0);
    printMatriz(C, n, 0, 0);

    printf("\nSubtracao:\n");
    subtractionMatriz(A, B, C, n, 0, 0);
    printMatriz(C, n, 0, 0);
    
    printf("\nMultiplicacao:\n");
    multiplicationMatriz(A, B, C, n, 0, 0, 0, 0);
    printMatriz(C, n, 0, 0);
    return 0;
}

/*
AUTOR: FELIPE AMORIM DO CARMO SILVA
DATA: 01/03/2025
SESSÃO PBL: 26/02/2025
*/
