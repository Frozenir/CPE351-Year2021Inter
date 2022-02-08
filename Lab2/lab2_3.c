//
//  lab2_3.c
//
//
//  Created by Forina_Fore on 1/2/2565 BE.
//
#include <mpi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int freeAllocate(double **array, int row)
{
    for (int i = 0; i < row; i++){
            free(array[i]);
    }

    free(array);
    array = NULL;

    return 0;
}

double **readMatrix(FILE* filename, int row, int col)
{
    int i,j =0;
    double num = 0;
    double **matrix = NULL;
    
    //allocateMatrix
    matrix = (double**)malloc(row * sizeof(double*));
    for (i = 0; i < row; i++){
        matrix[i] = (double*)malloc(col * sizeof(double));
        if(matrix[i] == NULL){
            printf("out of memory\n");
            exit(1);
        }
    }
    
    for (i=0 ; i<row; i++){
        for (j=0; j<col; j++){
            fscanf(filename, "%lf", &num);
            matrix[i][j] = num;
        }
    }
    
    return matrix;
}

int main(int argc, char** argv)
{
    int myrank, nprocs;
    MPI_Request request[5];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    
    //process at rank 0
    if (myrank == 0){
        FILE* firstInput = NULL;
        FILE* secondInput = NULL;
        FILE* output = NULL;
        double **matrix_first = NULL;
        double **matrix_second = NULL;
        double **matrix_sum = NULL;
        double startTime, endTime = 0;
        int row, col, i, j, countrank = 0;
        int divide_col, remain_col = 0;
        
        startTime = MPI_Wtime();

        firstInput = fopen("matAlarge.txt", "r");
        secondInput = fopen("matBlarge.txt", "r");
        if ((!firstInput)||(!secondInput)) {
            printf("file can't be opened\n");
        }
        else {
            printf("#### Reading Matrix 1 #############################################\n");
            fscanf(firstInput, "%d %d", &row, &col);
            printf("Matrix 1: row = %d, column = %d\n",row,col);
            matrix_first = readMatrix(firstInput, row, col);
            fclose(firstInput);
        
            printf("#### Reading Matrix 2 #############################################\n");
            fscanf(secondInput, "%d %d", &row, &col);
            printf("Matrix 2: row = %d, column = %d\n",row,col);
            matrix_second = readMatrix(secondInput, row, col);
            fclose(secondInput);
            
            printf("#### Adding two matrix ############################################\n");
            //allocate sum matrix
            matrix_sum = (double**)malloc(row * sizeof(double*));
            for (i = 0; i < row; i++)
                matrix_sum[i] = (double*)malloc(col * sizeof(double));
            
            //initiallize sum matrix to 0
            for(i = 0 ; i < row ; i++)
                for(j = 0; j < col; j++)
                    matrix_sum[i][j]=0;
            
            //seperate column for each processor
            divide_col = (int)col/nprocs;
            remain_col = col - (divide_col*nprocs);
            printf("Sum Matrix: row = %d, column = %d, divide part = %d, remaining = %d\n",row,col, divide_col, remain_col);
            
            output = fopen("matrix_sum.txt","w");
        
            if (output == NULL){
                printf("file can't be opened\n");
            }
            else{
                //addition in rank 0
                fprintf(output,"%d %d\n", row, col);
                for (i = 0; i < row; i++) {
                   for (j = 0 ; j < divide_col; j++) {
                       matrix_sum[i][j] = matrix_first[i][j] + matrix_second[i][j];
                   }
                }
            }
            
            if (nprocs != 1)
            {
                //rank 0 send detail to other ranks
                for (countrank = 1; countrank < nprocs; countrank++)
                {
                    MPI_Send(&row,1,MPI_INT,countrank,1,MPI_COMM_WORLD);
                    MPI_Send(&divide_col,1,MPI_INT,countrank,2,MPI_COMM_WORLD);
                }
        
                //rank 0 end value in each part of matrix to destinate rank
                for (countrank = 1; countrank < nprocs; countrank++){
                    for(i = 0 ; i < row ; i++){
                        for(j = divide_col*countrank; j < divide_col*(countrank+1); j++){
                            MPI_Send(&matrix_first[i][j],1,MPI_DOUBLE,countrank,3,MPI_COMM_WORLD);
                            MPI_Send(&matrix_second[i][j],1,MPI_DOUBLE,countrank,4,MPI_COMM_WORLD);
                        }
                    }
                }
                printf(">> Rank 0 send matrix completed\n");
                
                //addition of remaining column in rank 0 - put before recieve not to waste time
                if (remain_col !=0){
                    for (i = 0; i < row; i++) {
                       for (j = divide_col*nprocs; j < (divide_col*nprocs)+remain_col; j++) {
                           matrix_sum[i][j] = matrix_first[i][j] + matrix_second[i][j];
                       }
                    }
                }
                printf(">> Rank 0 calculate remaining completed\n");
                
                
                //rank 0 receive sum of matrix from other rank calcutatation
                for (countrank = 1; countrank < nprocs; countrank++){
                    for(i = 0 ; i < row ; i++){
                        for(j = divide_col*countrank; j < divide_col*(countrank+1); j++){
                            MPI_Irecv(&matrix_sum[i][j],1,MPI_DOUBLE,countrank,5,MPI_COMM_WORLD,&request[4]);
                            MPI_Wait(&request[4], MPI_STATUS_IGNORE)
                        }
                    }
                }
                printf(">> Rank 0 recieve matrix completed\n");
                
            }
        
            freeAllocate(matrix_first, row);
            freeAllocate(matrix_second, row);
            
            //write sum matrix to file
            for (i = 0; i < row; i++) {
               for (j = 0 ; j < col; j++) {
                   fprintf(output,"%lf ", matrix_sum[i][j]);
               }
               fprintf(output,"\n");
            }
            
            freeAllocate(matrix_sum, row);
            fclose(output);
            endTime = MPI_Wtime();
            
            printf("FINISHED WRITING SUM TO FILE!! in %lf seconds\n", endTime - startTime);
    }
    //process in other rank
    else {
        double **matrix_first = NULL;
        double **matrix_second = NULL;
        double **matrix_sum = NULL;
        int i, j = 0;
        int divide_col, row = 0;
        
        MPI_Irecv(&row,1,MPI_INT,0,1,MPI_COMM_WORLD, &request[0]);
        MPI_Irecv(&divide_col,1,MPI_INT,0,2,MPI_COMM_WORLD, &request[1]);
        MPI_Wait(&request[1], &status);
        MPI_Wait(&request[2], &status);
        printf(">> rank %d recieve divide part information\n",myrank);
        printf("   divide column = %d, row = %d\n",divide_col, row);
        
        //allocate matrix
        matrix_first = (double**)malloc(row * sizeof(double*));
        for (i = 0; i < row; i++){
            matrix_first[i] = (double*)malloc(divide_col * sizeof(double));
            if(matrix_first[i] == NULL){
                printf("out of memory at rank %d\n", myrank);
                exit(1);
            }
        }
        
        matrix_second = (double**)malloc(row * sizeof(double*));
        for (i = 0; i < row; i++){
            matrix_second[i] = (double*)malloc(divide_col * sizeof(double));
            if(matrix_second[i] == NULL){
                printf("out of memory at rank %d\n", myrank);
                exit(1);
            }
        }
        
        matrix_sum = (double**)malloc(row * sizeof(double*));
        for (i = 0; i < row; i++){
            matrix_sum[i] = (double*)malloc(divide_col * sizeof(double));
            if(matrix_sum[i] == NULL){
                printf("out of memory at rank %d\n", myrank);
                exit(1);
            }
        }
        
        //recieve value from the two input matrices to calculate
        for (i = 0 ; i < row; i++){
            for (j = 0; j < divide_col; j++){
                MPI_Recv(&matrix_first[i][j],1,MPI_DOUBLE,0,3,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                MPI_Recv(&matrix_second[i][j],1,MPI_DOUBLE,0,4,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                matrix_sum[i][j] = matrix_first[i][j] + matrix_second[i][j];
            }
        }
        printf(">> recieve matrix at rank %d completed\n",myrank);

        //send matrix Res back to rank 0
        for(i = 0 ; i < row; i++){
            for(j = 0; j < divide_col; j++){
                MPI_Send(&matrix_sum[i][j],1,MPI_DOUBLE,0,5,MPI_COMM_WORLD);
            }
        }
        printf(">> Rank %d send sum matrix back completed\n",myrank);
  
    }
    
    MPI_Finalize();
    return 0;
}

/* For run check matrix ------------------------------------------------------
    for (i=0 ; i<row; i++){
        for (j=0; j<col; j++){
            printf("%lf ",matrix_second[i][j]);
        }
        printf("\n");
    }
 -----------------------------------------------------------------------------*/
