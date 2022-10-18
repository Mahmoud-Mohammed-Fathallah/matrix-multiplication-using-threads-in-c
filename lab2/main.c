#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
int r1=0,c1=0,r2=0,c2=0;
long int mat1[20][20],mat2[20][20],res1[20][20],res2[20][20],res3[20][20];
/*function for reading the matrices from file*/
void read_data_mat(char* filename,int num,long int mat[20][20])
{
    int r,c;
    FILE *f;
    f = fopen(filename,"r");
    if(num==1){
        fscanf(f,"row=%d col=%d",&r1,&c1);
        r=r1;c=c1;
    }
    else{
        fscanf(f,"row=%d col=%d",&r2,&c2);
        r=r2;c=c2;
    }
    for(int i=0;i<r;i++){
        for(int j=0;j<c;j++){
            fscanf(f,"%d",&mat[i][j]);
        }
    }
    fclose(f);
}
/*function for writing resulting matrices to files*/
void write_data_res(char* filename,int num,long int res[20][20])
{
    int i,j;
    FILE *f;
    f = fopen(filename,"a");
    if(num==1){
        fprintf(f, "method: one thread per matrix.\n");
    }else if(num==2){
        fprintf(f, "method: one thread per row.\n");
    }else{
        fprintf(f, "method: one thread per element.\n");
    }
    fprintf(f, "row=%d col=%d\n",r1,c2);
    for(i=0;i<r1;i++){
        for(j=0;j<c2;j++){
            fprintf(f, "%d  ",res[i][j]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}
/*function to multiply two matrices*/
void mult_matrix_onethread()
{
    int i,j,k;
    for(i=0;i<r1;i++){
        for(j=0;j<c2;j++){
            res1[i][j]=0;
            for(k=0;k<c1;k++){
                res1[i][j]+=mat1[i][k]*mat2[k][j];
            }
        }
    }
}
/*function to multiply two rows of two matrices*/
void* mult_matrix_row(void* row)
{
    int j,k;
    for(j=0;j<c2;j++){
        res2[(int)row][j]=0;
        for(k=0;k<c1;k++){
            res2[(int)row][j]+=mat1[(int)row][k]*mat2[k][j];
        }
    }
}
/*function to multiply two elements of two matrices*/
void* mult_matrix_element(void* rc)
{
    int k;
    int* temp = (int*)rc;
    res3[temp[0]][temp[1]]=0;
    for(k=0;k<c1;k++){
        res3[temp[0]][temp[1]]+=mat1[temp[0]][k]*mat2[k][temp[1]];
    }
}

int main(int argc,char* argv[])
{
    struct timeval stop1,stop2,stop3,start1,start2,start3;
    char in1[100],in2[100],out1[100],out2[100],out3[100];
    //taking input arguments
    if(argc>1){
        strcpy(in1,argv[1]);
        strcpy(in2,argv[2]);
        strcpy(out1,argv[3]);
        strcpy(out2,argv[3]);
        strcpy(out3,argv[3]);
    }else{
        //default values
        strcpy(in1,"a");
        strcpy(in2,"b");
        strcpy(out1,"c");
        strcpy(out2,"c");
        strcpy(out3,"c");
    }
    int i,j;
    read_data_mat(strcat(in1,".txt"),1,mat1);
    read_data_mat(strcat(in2,".txt"),2,mat2);
    if(c1!=r2){printf("Error: Invalid sizes of arrays!!\n");return 0;}
    pthread_t threadid[r1];
    int threads = r1*c2;
    pthread_t threadid2[threads];
    int roc[threads][2];
    /*first multiplying the matrix using one thread*/
    gettimeofday(&start1, NULL);
    mult_matrix_onethread();
    gettimeofday(&stop1, NULL);
    /*second method multiplying the matrices using a thread per row*/
    gettimeofday(&start2, NULL);
    for(i=0;i<r1;i++){
        pthread_create(&threadid[i],NULL,mult_matrix_row,(void*)i);
    }
              //joining the threads
    for(i=0;i<r1;i++){
        pthread_join(threadid[i],NULL);
    }
    gettimeofday(&stop2, NULL);
    /*third method multiplying the matrices using a thread per element*/
    gettimeofday(&start3, NULL);
    int k=0;
    for(i=0;i<r1;i++){
        for(j=0;j<c2;j++){
            roc[k][0]=i;roc[k][1]=j;
            pthread_create(&threadid2[k],NULL,mult_matrix_element,(void*)roc[k]);
            k++;
        }
    }
            //joining the threads
    for(i=0;i<threads;i++){
        pthread_join(threadid2[i],NULL);
    }
    gettimeofday(&stop3, NULL);
	//////////////////////////////////////////////////////
    //writing output to files and printing execution time
    write_data_res(strcat(out1,"_per_matrix.txt"),1,res1);
    write_data_res(strcat(out2,"_per_row.txt"),2,res2);
    write_data_res(strcat(out3,"_per_element.txt"),3,res3);
    printf("Execution time of one thread per matrix is: %d microseconds\n",stop1.tv_usec-start1.tv_usec);
    printf("Execution time of one thread per row is: %d microseconds\n",stop2.tv_usec-start2.tv_usec);
    printf("Execution time of one thread per element is: %d microseconds\n",stop3.tv_usec-start3.tv_usec);
    return 0;
}
