//
//  lab3.c
//  
//
//  Created by Forina_Fore on 8/2/2565 BE.
//

#include <mpi.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int myrank, nprocs;
    double startTime,endTime = 0;
    
    FILE *fileA;
    FILE *fileB;
    FILE *output;
    int colA,rowA,colB,rowB;
    int divide_row,remain_row;
    int *matrixbufA,*matrixbufC;
    int *matrixA,*matrixB,*matrix_out;
    int sum = 0;
    int i,j,k = 0;
    char *fileAname= "matAsmall.txt",*fileBname= "matBsmall.txt",*outputname= "SolSmall.txt";

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    if(myrank==0){
        startTime = MPI_Wtime();
        
        fileA = fopen("matAlarge.txt","r");
        fileB = fopen("matBlarge.txt","r");
        if(!fileA || !fileB){
            printf("File Not Found!\n");
            return 0;
        }
        else{
            fscanf(fileA, "%d %d",&rowA,&colA);
            printf("Matrix 1: row = %d, column = %d\n",rowA,colA);
            fscanf(fileB, "%d %d",&rowB,&colB);
            printf("Matrix 2: row = %d, column = %d\n",rowB,colB);

            if (colA!=rowB){
                printf("These two matrices can't be multiplied.\n");
                return 0;
            }
            else{
                divide_row = rowA/nprocs;
                remain_row = rowA%nprocs;
        
                //allocate matrix
                matrixA = (int*)malloc(rowA * colA * sizeof(int));
                matrixB = (int*)malloc(rowB * colB * sizeof(int));
                matrix_out = (int*)malloc(rowA * colB * sizeof(int));
                printf("allocate A B Result successful\n");

                printf("#### Reading Matrix 1 #############################################\n");
                for(i=0; i<rowA; i++){
                    for(j=0; j<colA; j++){
                        fscanf(fileA, "%d", &matrixA[(i*colA)+j]);
                    }
                }
                fclose(fileA);

                printf("#### Reading Matrix 2 #############################################\n");
                //read matrixB with transposing
                for(i=0; i<rowB; i++){
                    for(j=0; j<colB; j++){
                        fscanf(fileB, "%d", &matrixB[j*(rowB)+i]);
                    }
                }
                fclose(fileB);
            
                //swap row and column
                int tempswap = 0;
                
                tempswap = rowB;
                rowB = colB;
                colB = tempswap;

            }
        }
    }

    MPI_Bcast(&divide_row,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&rowA,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&colA,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&rowB,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&colB,1,MPI_INT,0,MPI_COMM_WORLD);
    printf("Rank %d >> Recieved column and row value\n",myrank);

    if (myrank!=0)
    {
        matrixB = (int*)malloc(rowB * colB * sizeof(int));
        matrixbufA = (int*)malloc(divide_row * colA * sizeof(int));
    }

    MPI_Bcast(&matrixB[0],rowB*colB,MPI_INT,0,MPI_COMM_WORLD);
    printf("Rank %d >> Got matrix B\n",myrank);

    matrixbufC = (int*)malloc(divide_row * rowB * sizeof(int));

    if(myrank==0){
        //send matrix A
        for (i=1; i<nprocs; i++) {
            MPI_Send(&matrixA[divide_row*colA*i],divide_row*colA, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        printf("Rank 0 >> Sent matrix A successful\n");
    }
    else{ //other rank

        MPI_Recv(&matrixbufA[0],divide_row*colA, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        printf("Rank %d >> Received matrix A \n",myrank);

        //multiplication in other rank
        printf("Rank %d >> Start multiplication \n",myrank);
        for(i=0;i<divide_row;i++){ //shift down row in A
            for(j=0; j < rowB; j++){ //shift down row in B
                for(k = 0 ; k < colA; k++){
                    sum+=matrixbufA[(i*colA)+k]*matrixB[(j*colA)+k];
                }
                matrixbufC[(i*rowB)+j]=sum;
                sum = 0;
            }
        }

        printf("Rank %d >> Finish multiplication \n",myrank);

        MPI_Send(&matrixbufC[0],divide_row*rowB, MPI_INT, 0, 123, MPI_COMM_WORLD);
        printf("Rank %d >> Send result matrix back to rank 0\n",myrank);
    }

    if(myrank==0)
    {
        //multiplication in rank 0
        printf("Start multiplication! rank 0\n");
        for(i=0;i<divide_row;i++){
            for(j=0; j < rowB; j++){
                for(k = 0 ; k < colA; k++){
                    sum+=matrixA[(i*colA)+k]*matrixB[(j*colA)+k];
                }
                matrix_out[(i*rowB)+j]=sum;
                sum = 0;
            }
        }
        printf("Rank %d >> Finish multiplication\n",myrank);

        if (remain_row!=0)
        {
            //calculate multiplication in rank 0
            sum = 0;
            for(i=divide_row*nprocs;i<(divide_row*nprocs)+remain_row;i++){
                for(j=0; j < rowB; j++){
                    for(k = 0 ; k < colA; k++){
                        sum+=matrixA[(i*colA)+k]*matrixB[(j*colA)+k];
                    }
                    matrix_out[(i*rowB)+j]=sum;
                    sum = 0;
                }
            }
            printf("Rank 0 >> Finish multiplication\n");
        }

        free(matrixA);
        free(matrixB);

        for (i=1; i<nprocs; i++) { // rank 0 get the parts from other ranks
            MPI_Recv(&matrix_out[divide_row*rowB*i],divide_row*rowB, MPI_INT, i, 123, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        }

        //write to file
        output = fopen(outputname,"w+");
        fprintf(output,"%d %d\n",rowA,rowB);
        for(i=0;i<rowA;i++){
            for(j=0;j<rowB;j++)
                fprintf(output, "%d ",matrix_out[(i*rowB)+j]);
            fprintf(output, "\n");
        }
        free(matrix_out);
        
        endTime = MPI_Wtime();

        fclose(output);
        printf("\nFINISHED WRITING SUM TO FILE!! in %lf seconds\n", endTime - startTime);
    }

    MPI_Finalize();
    return 0;
}
