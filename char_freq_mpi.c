#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>
#include "mpi.h"
#define N 128
#define base 0

int main (int argc, char *argv[]) {
	
	FILE *pFile;
	long file_size;
	char * buffer;
	char * filename;
	size_t result;
	int i, j, freq[N];
        int my_rank, num_procs;
        double start , end;
		
			/* Start up MPI */
	MPI_Init(&argc, &argv);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	//int piece = ((max + 1) / num_procs) + 1;
		

    if (argc != 2) {
		printf ("Usage : %s <file_name>\n", argv[0]);
		return 1;
        }
		
	if(my_rank == 0){
		filename = argv[1];
		pFile = fopen ( filename , "rb" );
		if (pFile==NULL) {printf ("File error\n"); return 2;}

	
		// obtain file size:
		fseek (pFile , 0 , SEEK_END);
		file_size = ftell (pFile);
		rewind (pFile);
		printf("file size is %ld\n", file_size);
		
		// allocate memory to contain the file:
		buffer = (char*) malloc (sizeof(char)*file_size);
		if (buffer == NULL) {printf ("Memory error\n"); return 3;}

		// copy the file into the buffer:
		result = fread (buffer,1,file_size,pFile);
		if (result != file_size) {printf ("Reading error\n"); return 4;} 
		//gemizei 128 8eseis me 0
		for (j=0; j<N; j++){
			freq[j]=0;
		}
	}
	MPI_Bcast(&file_size,1,MPI_LONG,0,MPI_COMM_WORLD);

        //auto p 8eloume parallhlo , gemizei to freq 
        //parallhlopoihshmh perioxh 
		//to kommati pou 8a kanei o ka8enas einai
        	int piece = ceil(file_size / num_procs);
		int extra = file_size % num_procs;
		int sum = 0;
		int sendcounts[num_procs];
		int displs[num_procs];
		
       for(int i = 0;i<num_procs;i++){
			if(i == (num_procs - 1)) sendcounts[i] = piece + extra;
			else sendcounts[i] = piece;
			displs[i] = sum;
			sum += sendcounts[i];
	   }
    		//stelnoume se kommatia to arxeio ston buffer sta slaves k ston eauto tou 
		if(my_rank == (num_procs - 1))
				piece = piece + extra;
		char *localbuffer = malloc(piece * sizeof(char));
            MPI_Scatterv(buffer, sendcounts,displs, MPI_CHAR,localbuffer, piece, MPI_CHAR,0, MPI_COMM_WORLD);	
		 
	    int localfreq[N];
		
			for (int i=0; i < piece; i++){
				localfreq[localbuffer[i] - base]++;								
			}
		
			
	MPI_Reduce(localfreq, freq, N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	
       //telos perioxhs
	   
	   if(my_rank == 0){
	for (j=31; j<N; j++){
		printf("%d = %d\n", j+base, freq[j]);
	}	

        //printf("time in parallel area:%f",(end-start));
	   }

	MPI_Finalize();
//	fclose (pFile);
	free (buffer);

	return 0;
}
