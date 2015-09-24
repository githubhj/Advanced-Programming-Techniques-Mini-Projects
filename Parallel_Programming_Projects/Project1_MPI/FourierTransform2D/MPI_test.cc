nclude <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

using namespace std;

#define MSG_SIZE 1000
char buf[MSG_SIZE];  // Message contents

int main(int argc,char**argv)
{
  int  numtasks, rank, rc; 
  
  rc = MPI_Init(&argc,&argv);
  if (rc != MPI_SUCCESS) {
    printf ("Error starting MPI program. Terminating.\n");
    MPI_Abort(MPI_COMM_WORLD, rc);
  }
  
  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  int row_per_process =  MSG_SIZE/num_tasks;
    for(int j=0; j<100; j++)
    {
      buf[rank*100+j] = rank;
    }
  MPI_Gather(buff[rank*100], 100, MPI_CHAR, buff, 100, MPI_CHAR, rank, MPI_COMM_WORLD)
  
}
