/*
 * UNM CS341 Cachelab Project 3
 * Due May 8, 2023
 * Michael Millar
 * mmillar@unm.edu
 */

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
void transpose_block64(int M, int N, int A[N][M], int B[M][N]);
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
        transpose_block64(M, N, A, B);
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

char transpose_block_desc[] = "A blocking transpose for MxN Matrix";
void transpose_block(int M, int N, int A[N][M], int B[M][N]) 
{
    double blockSize = 16.0;
    int diagValue, diagIndex;
    for (int rowBlock = 0; rowBlock < M; rowBlock+=blockSize){
        for (int colBlock = 0; colBlock < N; colBlock+=blockSize) {
            for (int row = colBlock; (row < N) && (row < colBlock + blockSize); row++) {
                for (int col = rowBlock; (col < M) && (col < rowBlock + blockSize); col++) {
                    if (row != col) {
                        B[col][row] = A[row][col];
                    } else {
                        diagIndex = row;
                        diagValue = A[diagIndex][diagIndex];
                    }
                }   
                if (rowBlock == colBlock) {
                    B[diagIndex][diagIndex] = diagValue;
                }
            }
        }
    }
}

char transpose_block64_desc[] = "A blocking transpose for 64x64 Matrix";
void transpose_block64(int M, int N, int A[N][M], int B[M][N])
{
    for (int rowBlock = 0; rowBlock < M; rowBlock+=4) {
        for (int colBlock = 0; colBlock < N; colBlock+=4) {
            int v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12;

            // Get first 3 rows of block
            v1=A[rowBlock][colBlock];
            v2=A[rowBlock][colBlock+1];
            v3=A[rowBlock][colBlock+2];
            v4=A[rowBlock][colBlock+3];

            v5=A[rowBlock+1][colBlock];
            v6=A[rowBlock+1][colBlock+1];
            v7=A[rowBlock+1][colBlock+2];
            v8=A[rowBlock+1][colBlock+3];

            v9=A[rowBlock+2][colBlock];
            v10=A[rowBlock+2][colBlock+1];
            v11=A[rowBlock+2][colBlock+2];
            v12=A[rowBlock+2][colBlock+3];
            // Assign B[][r+3]
            B[colBlock][rowBlock+3]=A[rowBlock+3][colBlock];
            B[colBlock+1][rowBlock+3]=A[rowBlock+3][colBlock+1];
            B[colBlock+2][rowBlock+3]=A[rowBlock+3][colBlock+2];
            B[colBlock+3][rowBlock+3]=A[rowBlock+3][colBlock+3];
            // Assign B[][r+2]
            B[colBlock][rowBlock+2]=v9;
            B[colBlock+1][rowBlock+2]=v10;
            B[colBlock+2][rowBlock+2]=v11;
            B[colBlock+3][rowBlock+2]=v12;
            // Assign B[][r+1]
            B[colBlock][rowBlock+1]=v5;
            B[colBlock+1][rowBlock+1]=v6;
            B[colBlock+2][rowBlock+1]=v7;
            B[colBlock+3][rowBlock+1]=v8;
            // Assign B[][r]
            B[colBlock][rowBlock]=v1;
            B[colBlock+1][rowBlock]=v2;
            B[colBlock+2][rowBlock]=v3;
            B[colBlock+3][rowBlock]=v4;
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

