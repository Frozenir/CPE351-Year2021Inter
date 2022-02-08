//
//  read_matrix.c
//
//
//  Created by Forina_Fore on 1/2/2565 BE.
//
//#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int checkAllocate(double **array)
{
    if (array == NULL) {
        printf("Out of memory\n");
        exit(1);
    }

    return 0;
}

int freeAllocate(double **array, int m, int n)
{
    for (int i = 0; i < m; i++)
            free(array[i]);

    free(array);
    array = NULL;

    return 0;
}

int main(int argc, char** argv)
{
    FILE* firstInput;
    FILE* secondInput;
    FILE* output;
    double **matrix_first = NULL;
    double **matrix_second = NULL;
    double **matrix_sum = NULL;
    int m,n,o,p,i,j =0;
    double num = 0;
    
    firstInput = fopen("matAsmall.txt", "r");
    secondInput = fopen("matBsmall.txt", "r");
     
    if ((firstInput == NULL)||(secondInput == NULL)) {
        printf("file can't be opened\n");
    }
    else {
        fscanf(firstInput, "%d %d", &m, &n);
        printf("Reading File 1\n");
        printf("Matrix 1: column = %d, row = %d\n",m,n);
        
        matrix_first = (double**)malloc(m * sizeof(double*));
        printf("pass allocate1\n");
        for (i = 0; i < n; i++){
            //printf("pass %d\n", i);
            matrix_first[i] = (double*)malloc(n * sizeof(double));
            if(matrix_first[i] == NULL){
                printf("out of memory\n");
                exit(1);
            }
        }
        checkAllocate(matrix_first);
        printf("pass allocate2\n");
        
        for (i=0 ; i<n; i++){
            for (j=0; j<m; j++){
                fscanf(firstInput, "%lf", &num);
                //printf("pass1\n");
                matrix_first[i][j] = num;
                //printf("pass2\n");
                //printf("pass assign column %d\n", j);
            }
            //printf("pass assign row %d\n", i);
        }
        
        printf("#####################################################################################\n");
        printf("Reading File 2\n");
        
        fscanf(secondInput, "%d %d", &m, &n);
        printf("Reading File 1\n");
        printf("Matrix 2: column = %d, row = %d\n",m,n);
        
        matrix_second = (double**)malloc(m * sizeof(double*));
        printf("pass allocate1\n");
        for (i = 0; i < n; i++){
            //printf("pass %d\n", i);
            matrix_second[i] = (double*)malloc(n * sizeof(double));
            if(matrix_second[i] == NULL){
                printf("out of memory\n");
                exit(1);
            }
        }
        checkAllocate(matrix_second);
        printf("pass allocate2\n");
        
        for (i=0 ; i<n; i++){
            for (j=0; j<m; j++){
                fscanf(secondInput, "%lf", &num);
                //printf("pass1\n");
                matrix_second[i][j] = num;
                //printf("pass2\n");
                //printf("pass assign column %d\n", j);
            }
            //printf("pass assign row %d\n", i);
        }
        
        printf("#####################################################################################\n");
        printf("\n");
        
        output = fopen("matlarge_sum.txt", "w");
        
        for (i=0 ; i<m; i++){
            for (j=0; j<n; j++){
                printf("%f ",matrix_second[i][j]);
            }
            printf("\n");
        }
        
        //add matrix
        matrix_sum = (double**)malloc(m * sizeof(double*));
        for (i = 0; i < n; i++)
            matrix_sum[i] = (double*)malloc(n * sizeof(double));
        checkAllocate(matrix_sum);

        for (i = 0; i < m; i++) {
           for (j = 0 ; j < n; j++) {
              matrix_sum[i][j] = matrix_first[i][j] + matrix_second[i][j];
              printf("%lf ", matrix_sum[i][j]);
           }
           printf("\n");
        }
        
    }
    
    freeAllocate(matrix_first, m, n);
    freeAllocate(matrix_second, m, n);
    freeAllocate(matrix_sum, m, n);
    
    fclose(firstInput);
    fclose(secondInput);
    fclose(output);
}
