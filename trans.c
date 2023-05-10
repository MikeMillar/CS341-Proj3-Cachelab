/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include <math.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void transpose_block32(int M, int N, int A[N][M], int B[M][N]);
void transpose_block(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (M == 32 && N == 32) {
        transpose_block32(M, N, A, B);
    } else if (M == 64 && N == 64) {
        transpose_block(M, N, A, B);
    } else {
        transpose_block(M, N, A, B);
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

char transpose_block32_desc[] = "A blocking transpose for 32x32 Matrix";
void transpose_block32(int M, int N, int A[N][M], int B[M][N])
{
    double blockSize = 8.0;
    int rowBlockCount = (int) ceil(M / blockSize);
    int colBlockCount = (int) ceil(N / blockSize);
    int diagValue, diagIndex;
    for (int rowBlock = 0; rowBlock < rowBlockCount; rowBlock++) {
        for (int colBlock = 0; colBlock < colBlockCount; colBlock++) {
            for (int row = 0; row < blockSize; row++) {
                int rowIndex = rowBlock*blockSize+row;
                if (rowIndex >= M) {
                    break;
                }
                for (int col = 0; col < blockSize; col++) {
                    int colIndex = colBlock*blockSize+col;
                    if (colIndex >= N) {
                        break;
                    }
                    if (rowIndex == colIndex) {
                        diagIndex = rowIndex;
                        diagValue = A[diagIndex][diagIndex];
                    } else {
                        B[colIndex][rowIndex] = A[rowIndex][colIndex];
                    }
                }
                if (rowBlock == colBlock) {
                    B[diagIndex][diagIndex] = diagValue;
                }
            }
        }   
    }
}

char transpose_block_desc[] = "A test blocking transpose";
void transpose_block(int M, int N, int A[N][M], int B[M][N])
{
    double blockSize = 4.0;
    // int rowBlockCount = (int) ceil(M / blockSize);
    // int colBlockCount = (int) ceil(N / blockSize);
    for (int rowBlock = 0; rowBlock < M; rowBlock+=blockSize) {
        for (int colBlock = 0; colBlock < N; colBlock+=blockSize) {
            int v1,v2,v3,v4,v5,v6;
            
            // // Get all elements below block diag
            // v1=A[rowBlock+1][colBlock];
            // v2=A[rowBlock+2][colBlock];
            // v3=A[rowBlock+2][colBlock+1];
            // v4=A[rowBlock+3][colBlock];
            // v5=A[rowBlock+3][colBlock+1];
            // v6=A[rowBlock+3][colBlock+2];
            // // Assign items in B[c][]
            // B[colBlock][rowBlock]=A[rowBlock][colBlock];
            // B[colBlock][rowBlock+1]=v1;
            // B[colBlock][rowBlock+2]=v2;
            // B[colBlock][rowBlock+3]=v4;
            // // Assign items in B[c+1][]
            // B[colBlock+1][rowBlock]=A[rowBlock][colBlock+1];
            // B[colBlock+1][rowBlock+1]=A[rowBlock+1][colBlock+1];
            // B[colBlock+1][rowBlock+2]=v3;
            // B[colBlock+1][rowBlock+3]=v5;
            // // Assign items in B[c+2][]
            // B[colBlock+2][rowBlock]=A[rowBlock][colBlock+2];
            // B[colBlock+2][rowBlock+1]=A[rowBlock+1][colBlock+2];
            // B[colBlock+2][rowBlock+2]=A[rowBlock+2][colBlock+2];
            // B[colBlock+2][rowBlock+3]=v6;
            // // Assign items in B[c+3][]
            // B[colBlock+3][rowBlock]=A[rowBlock][colBlock+3];
            // B[colBlock+3][rowBlock+1]=A[rowBlock+1][colBlock+3];
            // B[colBlock+3][rowBlock+2]=A[rowBlock+2][colBlock+3];
            // B[colBlock+3][rowBlock+3]=A[rowBlock+3][colBlock+3];

            // Get L of block
            v1=A[rowBlock][colBlock];
            v2=A[rowBlock+1][colBlock];
            v3=A[rowBlock+2][colBlock];
            v4=A[rowBlock+3][colBlock];
            v5=A[rowBlock+3][colBlock+1];
            v6=A[rowBlock+3][colBlock+2];
            // Assign B[c+3][]
            B[colBlock+3][rowBlock]=A[rowBlock][colBlock+3];
            B[colBlock+3][rowBlock+1]=A[rowBlock+1][colBlock+3];
            B[colBlock+3][rowBlock+2]=A[rowBlock+2][colBlock+3];
            B[colBlock+3][rowBlock+3]=A[rowBlock+3][colBlock+3];
            // Assign B[c+2][]
            B[colBlock+2][rowBlock]=A[rowBlock][colBlock+2];
            B[colBlock+2][rowBlock+1]=A[rowBlock+1][colBlock+2];
            B[colBlock+2][rowBlock+2]=A[rowBlock+2][colBlock+2];
            B[colBlock+2][rowBlock+3]=v6;
            // Assign B[c+1][]
            B[colBlock+1][rowBlock]=A[rowBlock][colBlock+1];
            B[colBlock+1][rowBlock+1]=A[rowBlock+1][colBlock+1];
            B[colBlock+1][rowBlock+2]=A[rowBlock+2][colBlock+1];
            B[colBlock+1][rowBlock+3]=v5;
            // Assign B[c][]
            B[colBlock][rowBlock]=v1;
            B[colBlock][rowBlock+1]=v2;
            B[colBlock][rowBlock+2]=v3;
            B[colBlock][rowBlock+3]=v4;
        }   
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    // registerTransFunction(trans, trans_desc); 
    // registerTransFunction(transpose_block, transpose_block_desc);

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

