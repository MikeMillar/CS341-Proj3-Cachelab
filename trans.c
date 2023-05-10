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

char transpose_block_desc[] = "A test blocking transpose";
void transpose_block(int M, int N, int A[N][M], int B[M][N])
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
    registerTransFunction(trans, trans_desc); 
    registerTransFunction(transpose_block, transpose_block_desc);

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

