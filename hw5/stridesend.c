/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   %%%%
   %%%% This program file is part of the book and course
   %%%%   "Parallel Computing for Science and Engineering"
   %%%% by Victor Eijkhout, copyright 2013-7
   %%%%
   %%%% MPI example for vector type
   %%%%
   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"

int main(int argc,char **argv) {

  MPI_Comm comm; 
  MPI_Init(&argc,&argv);
  comm = MPI_COMM_WORLD;

  int nprocs,procno;
  MPI_Comm_rank(comm,&procno);
  MPI_Comm_size(comm,&nprocs);

  if (nprocs<2) {
    printf("This program needs at least two processes\n");
    return -1;
  }
  int sender = 0; 
  int localsize = 10;

  if (procno==sender) { // proc0 is disseminating data to everyone else
    /*
     * Create big data array to be sent
     */
    int ndata = localsize*nprocs;
    int *data = (int*) malloc(ndata*sizeof(int));
    if (!data) {
      printf("Out of memory\n"); MPI_Abort(comm,0); }
    for (int i=0; i<ndata; i++)
      data[i] = i;
    /*
     * Exercise
     * - you need a datatype for sending data. Can you define it here....
     */
    MPI_Datatype scattertype;
    int stride=nprocs;  //good
    int blocklength=1; // good
    //int count=nprocs*blocklength; // ???
    int count=localsize; // ???

    /* 
     * loop over all processes you are sending to
     */

    MPI_Type_vector(count,blocklength,stride,MPI_INT,&scattertype);
    MPI_Type_commit(&scattertype);

    for (int sendto=0; sendto<nprocs; sendto++) {
      if (sendto==sender)
	continue;

      MPI_Send(&data[sendto],1,scattertype,sendto,0,comm);

      /*
       * - or do you define the datatype here?
       * - then do a send to the other processor.
       */
/**** your code here ****/
    }

    MPI_Type_free(&scattertype);

    /*
     * Make sure you free the datatype again
     * - do you do that here or inside the send loop?
     */

  } else {
    int *mydata = (int*) malloc(localsize*sizeof(int));
    MPI_Recv(mydata,localsize,MPI_INT,sender,0,comm,MPI_STATUS_IGNORE);
    for (int i=0; i<localsize; i++)
      if (mydata[i]%nprocs!=procno)
	printf("[%d] received element=%d, should be %d\n",procno,mydata[i],i*nprocs+procno);
  }

  if (procno==0)
    printf("Finished\n");

  MPI_Finalize();
  return 0;
}
