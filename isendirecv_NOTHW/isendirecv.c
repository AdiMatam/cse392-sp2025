/****************************************************************
 ****
 **** This program file is part of the book 
 **** `Parallel programming for Science and Engineering'
 **** by Victor Eijkhout, eijkhout@tacc.utexas.edu
 ****
 **** copyright Victor Eijkhout 2012-2021
 ****
 **** MPI Exercise for Isend/Irecv
 ****
 ****************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "mpi.h"

#include "tools.h"

int main(int argc,char **argv) {

  MPI_Init(&argc,&argv);
  MPI_Comm comm = MPI_COMM_WORLD;

  int nprocs, procno;
    MPI_Comm_size(comm,&nprocs);
  MPI_Comm_rank(comm,&procno);

  double mydata=procno;
  double leftdata=0;
  double rightdata=0.;
  int sendto,recvfrom;
  MPI_Request requests[4];

/*NOTE: NON-BLOCKING PARADIGM. IRECV, ISEND TRIGGERS A 'FUTURE' WHICH IS RESOLVED BY REQUEST CAPTURING */

  // Exercise:
  // -- set `sendto' and `recvfrom' twice
  //    once to get data from the left, once from the right
  // -- for first/last process use MPI_PROC_NULL

  if (procno==0) {
	recvfrom = MPI_PROC_NULL;
	sendto = MPI_PROC_NULL;
  }
  else {
	recvfrom = procno-1;
	sendto = procno-1;
  }

 // get data from the left: who are you communicating with?
  MPI_Isend(&mydata,1,MPI_DOUBLE, sendto,0, comm, &requests[0]);
  MPI_Irecv(&leftdata,1,MPI_DOUBLE, recvfrom,0, comm, &requests[1]);

  if (procno==nprocs-1) {
	recvfrom = MPI_PROC_NULL;
	sendto = MPI_PROC_NULL;
  }
  else {
	recvfrom = procno+1;
	sendto = procno+1;
  }

  // get data from the right: who are you communicating with?
  MPI_Isend(&mydata,1,MPI_DOUBLE, sendto,0, comm, &requests[2]);
  MPI_Irecv(&rightdata,1,MPI_DOUBLE, recvfrom,0, comm, &requests[3]);

  //
  // Now make sure all Isend/Irecv operations are completed
  //

  MPI_Waitall(4, requests, MPI_STATUSES_IGNORE);// TODO: wait for async 'futures' to resolve

  /*
   * Correctness check:
   * `error' will be:
   * - the lowest process number where an error occured, or
   * - `nprocs' if no error.
   */
  // check correctness
  mydata = mydata+leftdata+rightdata;

  double res;
  if (procno==0) {
    res = 2*procno+1;
  } else if (procno==nprocs-1) {
    res = 2*procno-1;
  } else {
    res = 3*procno;
  }

  int error_test = !ISAPPROX(mydata,res);
  if (error_test)
    printf("Data on proc %d should be %e, found %e\n",procno,res,mydata);
  print_final_result(error_test,comm);

  MPI_Finalize();
  return 0;
}
