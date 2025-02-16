/****************************************************************
 ****
 **** This program file is part of the book 
 **** `Parallel programming for Science and Engineering'
 **** by Victor Eijkhout, eijkhout@tacc.utexas.edu
 ****
 **** copyright Victor Eijkhout 2012-2021
 ****
 **** MPI Exercise for Isend/Irecv, sending an array
 ****
 ****************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "mpi.h"

#include "tools.h"

#define LATENCY_HIDE

int main(int argc,char **argv) {

  MPI_Init(&argc,&argv);
  MPI_Comm comm = MPI_COMM_WORLD;

  int nprocs, procno;
  MPI_Comm_size(comm,&nprocs);
  MPI_Comm_rank(comm,&procno);

#define N 100
  double indata[N],outdata[N];
  for (int i=0; i<N; i++)
    indata[i] = 1.;

  double leftdata=0.,rightdata=0.;
  int sendto,recvfrom;
  MPI_Request requests[4];

  // Exercise:
  // -- set `sendto' and `recvfrom' twice
  //    once to get data from the left, once from the right
  // -- for first/last process use MPI_PROC_NULL
  //
  if (procno==0) {
    sendto=MPI_PROC_NULL;
    recvfrom=MPI_PROC_NULL;
}
 else {
     sendto=procno-1; 
     recvfrom=procno-1;
  }
  // first specify left neighbor data
  MPI_Isend(&indata,1,MPI_DOUBLE, sendto,0, comm,&requests[0]);
  MPI_Irecv(&leftdata,1,MPI_DOUBLE, recvfrom,0, comm,&requests[1]);

  if (procno==nprocs-1) {
    sendto=MPI_PROC_NULL;
    recvfrom=MPI_PROC_NULL;
}
 else {
     sendto=procno+1; 
     recvfrom=procno+1;
  }
  MPI_Isend(&indata, 1,MPI_DOUBLE, sendto,0, comm,&requests[2]);
  MPI_Irecv(&rightdata,1,MPI_DOUBLE, recvfrom,0, comm,&requests[3]);

#ifdef LATENCY_HIDE

  // LATENCY HIDING -- COMPUTE INTERNAL POINTS
  for (int i=1; i < N-1; i++) {
	outdata[i] = indata[i-1] + indata[i] + indata[i+1];
  }
  // make sure all irecv/isend operations are concluded
  MPI_Waitall(4, requests, MPI_STATUSES_IGNORE);

  // compute boundary values
  outdata[0] = leftdata + indata[0] + indata[1];
  outdata[N-1] = indata[N-2] + indata[N-1] + rightdata;

#else

  // make sure all irecv/isend operations are concluded
  MPI_Waitall(4, requests, MPI_STATUSES_IGNORE);
  /*
   * Do the averaging operation
   * Note that leftdata==rightdata==0 if not explicitly received.
   */
  for (int i=0; i<N; i++)
    if (i==0)
      outdata[i] = leftdata + indata[i] + indata[i+1];
    else if (i==N-1)
      outdata[i] = indata[i-1] + indata[i] + rightdata;
    else
      outdata[i] = indata[i-1] + indata[i] + indata[i+1];
#endif
  
  /*
   * Check correctness of the result:
   * value should be 2 at the end points, 3 everywhere else
   */
  double answer[N];
  for (int i=0; i<N; i++) {
    if ( (procno==0 && i==0) || (procno==nprocs-1 && i==N-1) ) {
      answer[i] = 2.;
    } else {
      answer[i] = 3.;
    }
  }
  int error_test = array_error(answer,outdata,N);
  print_final_result(error_test,comm);

  MPI_Finalize();
  return 0;
}
