#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

int test(int *a, int n);
int parsetable(int *a,int n);

int main(int argc, char *argv[]) {


        int my_rank , num_procs;
	/* Start up MPI */
	MPI_Init(&argc, &argv);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	if (argc != 2){
    			printf("Non correct number of arguments\n");
   			return 1;
			}

	int elements_table = strtol(argv[1], NULL, 10);
	int i;
	int max = 0;
	int* a;
	double start , end;

	   a = malloc(elements_table * sizeof(int));
       //exclusive master code epeidh 8eloume na kanei mono o master 1 fora ton a pou 8a 
       // doulepsoume panw
       if(my_rank == 0){
		   for(i = 0;i < elements_table;i++)
			{
			  int x = rand() % elements_table;
			  if(x == 0)
				x = x + (rand() % elements_table) + 1;
			  a[i] = x ;
			  if(a[i]>max)
				max=a[i];
		   }
		}
		MPI_Bcast(&max,1,MPI_INT,0,MPI_COMM_WORLD);
	    int sizeofcount = max + 1;
	    int piece = ceil(sizeofcount / num_procs);
		int extra = sizeofcount % num_procs;
		int sum = 0;
		int sendcounts[num_procs];
		int displs[num_procs];
		int *temp = malloc(piece * sizeof(int));
		int *count = malloc(sizeofcount * sizeof(int));

		for(int i = 0;i<num_procs;i++){
			if(i == (num_procs - 1)) sendcounts[i] = piece + extra;
			else sendcounts[i] = piece;
			displs[i] = sum;
			sum += sendcounts[i];
		}
		int *localcount = malloc(sizeofcount * sizeof(int));
		for(i=0;i<sizeofcount;i++){
			localcount[i]=0;
			temp[i]=0;
         }
		 if(my_rank == (num_procs - 1))
				piece = piece + extra;
			
		int *locala = malloc(piece * sizeof(int));
		MPI_Scatterv(a, sendcounts,displs, MPI_LONG,&locala, piece, MPI_LONG,0, MPI_COMM_WORLD);	
		//o master stelnei ton a xwrismeno gia na epeksergastei kai to sizeofcount gia na dhmiourgh8ei ena local count pou 8a ginei
		//reduce sto count tou master (stelnei kai ston master opote kai o master paizei ton rolo slave)

		//SLAVE k master
		//dhmiourgoume ena local count me to locala pou exei xwrismenh thn douleia gia na to kanoume
		//reduce sto count
			for (int i = 0; i < piece; i++) {
				localcount[locala[i]]++;
			}
   MPI_Reduce(&localcount, &count, sizeofcount, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

	//to delta einai log me bash to 2 tou n
	// gia ta log kai pow prepei otan kanoume gcc na valoume k -lm
	// gia thn math.h sto telos
   double delta = (log(elements_table)/log(2)) ;
   double cdelta = ceil(delta);
   //ksanakanoume spread ton phnaka count
     for(int j=0;j<cdelta;j++){
     	int value = 2<<j;
        //parallhlo kommati		   
			MPI_Scatterv(count, sendcounts,displs, MPI_LONG, &localcount, piece, MPI_LONG, 0, MPI_COMM_WORLD);
			//stelnoume xwrista enan count ston localcount pou exei mesa mono piece stoixeia kai olos o allos kenos gia na ton kanoume gather meta

			//SLAVE code
			//etsi to ka8e task 8a ektelesei ston localcount kai meta 8a ginei gather ston count me mege8os sizeofcount
			for (int i = 0; i < piece; i++) {
				if (i % 2 == 0) {
					if (i < value) {
						temp[i] = localcount[i];
					}
					else {
						temp[i] = localcount[i] + localcount[i - value];
					}
				}
				else {
					if (i < value) {
						localcount[i] = temp[i];
					}
					else {
						localcount[i] = temp[i] + temp[i - value];
					}
				}
				if(i % 2 == 0)
					MPI_Gather(&temp,1,MPI_LONG,count,1, MPI_LONG,0 , MPI_COMM_WORLD);
				else
					MPI_Gather(&localcount, 1, MPI_LONG, count, 1, MPI_LONG, 0, MPI_COMM_WORLD);
			}
           }
     //telos perioxhs kai tou slave kammatiou
     if(my_rank == 0){
		int *output = malloc(elements_table * sizeof(int)); 
		for (i = 0; i<elements_table; i++){ 
			output[count[a[i]]-1] = a[i]; 
			count[a[i]]--; 
     }

     for (i = 0; i<elements_table; i++) 
        a[i] = output[i]; 
    }
    MPI_Finalize();
    test(a,elements_table);
    printf("time in parallel area:%f",(end-start));
    return 0;    
}

int test(int *a, int n) {
  // if 0 == false
  int pass = 1; 
  for (int i = 1; i<n; i++)
  {        
      if (a[i] < a[i-1])
      	pass = 0;    
  }
  if(pass)
       	   printf("The list is sorted\n");
     else
           printf("The list isn't sorted\n");  
  return 0; 
}

int parsetable(int *a,int n){

    printf("\n");
    for (int i = 0; i<n; i++) 
        printf("-%d-",a[i]); 
   printf("\n");
 return 0;
}
