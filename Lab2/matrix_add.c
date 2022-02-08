//
//  matrix_add.c
//  
//
//  Created by Forina_Fore on 1/2/2565 BE.
//

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int checkAllocate(int **array)
{
    if (array == NULL) {
        printf("Out of memory\n");
        exit(1);
    }
    
    for (int i = 0; i < n; i++) {
        array[i] = malloc(n * sizeof(int));
        if(array[i] == NULL) {
            printf("Out of memory\n");
            exit(1);
        }
    }
    
    return 0;
}

void matrix_read(FILE* firstFile, FILE* secondFile)
{
    int m,n = 0;
    int **matrix_first;
    int **matrix_second;
    int checkValue;
    char buffer[255];
    
    //Enter the number of rows and columns of matrix
    fgets(firstFile,"%s", &buffer);
    fscanf(buffer,"%d %d", &m, &n);
    
    //Enter the elements of first matrix
    matrix_first = (int *)malloc(n * sizeof(int *));
    checkAllocate(matrix_first);
    
    
    
    
    -------------------------------------------------------------
  
    for (c = 0; c < m; c++)
       for (d = 0; d < n; d++)
          scanf("%d", &first[c][d]);
  
    printf("Enter the elements of second matrix\n");
  
    for (c = 0; c < m; c++)
       for (d = 0 ; d < n; d++)
          scanf("%d", &second[c][d]);
    
    printf("Sum of entered matrices:-\n");
     
    for (c = 0; c < m; c++) {
       for (d = 0 ; d < n; d++) {
          sum[c][d] = first[c][d] + second[c][d];
          printf("%d\t", sum[c][d]);
       }
       printf("\n");
    }
}

int main(int argc, char** argv);
{
    FILE* firstInput;
    FILE* secondInput;
    int m, n, c, d;
    int first[20][20], second[20][20], sum[20][20];
    
    firstInput = fopen("matAsmall.txt", "r");
    secondInput = fopen("matBsmall.txt", "r");
     
    if ((NULL == firstInput)&&(NULL == secondInput)) {
        printf("both file or one of the files can't be opened\n");
    }
    else {
       
    }
}
