/****************************************************************
 ****
 **** This program file is part of the book 
 **** `Parallel programming for Science and Engineering'
 **** by Victor Eijkhout, eijkhout@tacc.utexas.edu
 ****
 **** copyright Victor Eijkhout 2012-9
 ****
 **** MPI Exercise for the use of Comm_rank/size
 ****
 ****************************************************************/

#include <iostream>
#include <sstream>
using namespace std;
#include <mpi.h>

int main() {
  MPI_Comm comm = MPI_COMM_WORLD;
  int nprocs, procno;
  
  MPI_Init(0,0);
  MPI_Comm_size(comm, &nprocs);
  MPI_Comm_rank(comm, &procno);

  if (procno == 0) {
	printf("I am processor %d. There are %d processes in total\n", procno, nprocs); 
  }

/*
  // Exercise part 1:
  // -- Use the routine
  //    MPI_Comm_size and MPI_Comm_rank
  // -- Let each processor print out a message like
  //    "Hello from processor 7 out of 12"
  //    reporting its number and the total number.

  printf("Hello from processor %d out of %d\n", procno, nprocs);

  // Exercise part 2:
  // -- let only processs zero print out
  //    "There are 16 processes"
  //    reporting only the total number
  printf("There are %d processes\n", nprocs);
cout << "There are " << nprocs << " processes\n";

*/
  
  MPI_Finalize();
  return 0;
}
