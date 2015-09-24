// Distributed two-dimensional Discrete FFT transform
// Harshit Jain
// ECE8893 Project 1


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <signal.h>
#include <math.h>
#include <mpi.h>
#include <cmath>
#include "Complex.h"
#include "InputImage.h"

using namespace std;

void Transform1D(Complex *, int, Complex *, bool);
void TransposeMatrix(Complex *, int, int, Complex*);

void Transform2D(const char* inputFN) 
{ // Do the 2D transform here.
  // 1) Use the InputImage object to read in the Tower.txt file and
  //    find the width/height of the input image.
  // 2) Use MPI to find how many CPUs in total, and which one
  //    this process is
  // 3) Allocate an array of Complex object of sufficient size to
  //    hold the 2d DFT results (size is width * height)
  // 4) Obtain a pointer to the Complex 1d array of input data
  // 5) Do the individual 1D transforms on the rows assigned to your CPU
  // 6) Send the resultant transformed values to the appropriate
  //    other processors for the next phase.
  // 6a) To send and receive columns, you might need a separate
  //     Complex array of the correct size.
  // 7) Receive messages from other processes to collect your columns
  // 8) When all columns received, do the 1D transforms on the columns
  // 9) Send final answers to CPU 0 (unless you are CPU 0)
  //   9a) If you are CPU 0, collect all values from other processors
  //       and print out with SaveImageData().

  	InputImage image(inputFN);  // Create the helper object for reading the image

  	// Step (1) in the comments is the line above.
  	
        // No. of Rows in the Image
	int nRows = image.GetHeight();
  	//No. of Columns in the Image
 	int nColumns = image.GetWidth();
  
	// Step (2)
  	//Local variables, no. of CPUs, myrank, and status check
	int nCPUs, rank, rc;
  
	// As always, we must call MPI_Funcions
	MPI_Comm_size(MPI_COMM_WORLD,&nCPUs);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  	
	//Rows is a multiple of CPU, given
	//Calculating Rows per CPU for tranform 
	int Row_per_cpu = (nRows/nCPUs);

  	// Step (3)
  	//Lets take the Input matrix
	Complex * Input = new Complex[nColumns*nRows];
  	Input = image.GetImageData();

	//Id Transform Matrix
  	Complex * Transform = new Complex[nColumns*nRows];

        //2D Transform Matrix
  	Complex * Transform_2D = new Complex[nColumns*nRows];
  	
	//Temp row array to send data
 	Complex * temp_send_array = new Complex[nColumns*Row_per_cpu];
  	
	//Temprory transpose array
 	Complex * Tpose = new Complex[nRows*nColumns];
  
  	// Step(4)
	//Running loop for every CPU, CPU will execute only their Rows of Matrix
  	for(int i =0; i < Row_per_cpu ; i++)
  	{
		Transform1D( (Input + (Row_per_cpu*rank*nColumns) + (i*nColumns)), nColumns, (Transform + (Row_per_cpu*rank*nColumns) + (i*nColumns)),0);
  	}

	//Temp Array pointing to a CPU's Computed transoform
	temp_send_array = (Transform + (Row_per_cpu*rank*nColumns));
  	
	//Lets Merge. Using gather to merge everything
	for(int j =0 ; j<nCPUs; j++)
  	{

   	 	MPI_Gather( (temp_send_array), (sizeof(Complex)*nColumns*Row_per_cpu) , MPI_CHAR, Transform, (sizeof(Complex)*nColumns*Row_per_cpu), MPI_CHAR, j, MPI_COMM_WORLD);
   
  	}

	//Putting barrier for just in case
   	MPI_Barrier(MPI_COMM_WORLD);
    
	//Let any rank get me 1D transform
    	if(rank==0)
   	{
   	 	image.SaveImageData("MyAfter1d.txt", Transform, nRows, nColumns);
   	}
   	
	//Traanspose Matrix to do 2D transform
  	TransposeMatrix(Transform, nColumns, nRows, Tpose);
	
	//Again just in case
   	MPI_Barrier(MPI_COMM_WORLD);

	//Using any rank to print Transpose
   	if(rank==0)
   	{
    		image.SaveImageData("MyAfter1d_tpose.txt", Tpose, nRows, nColumns);
   	}
	
	//Again just in case
   	MPI_Barrier(MPI_COMM_WORLD);
  
	//Lets get 2D now!
	for(int i =0; i < Row_per_cpu ; i++)
	{
		Transform1D( (Tpose + (Row_per_cpu*rank*nColumns) + (i*nColumns)), nColumns, (Transform_2D + (Row_per_cpu*rank*nColumns) + (i*nColumns)), 0);
    	}
  	
	temp_send_array = (Transform_2D + (Row_per_cpu*rank*nColumns));
  	
	for(int j =0 ; j<nCPUs; j++)
  	{
		MPI_Gather( (temp_send_array), (sizeof(Complex)*nColumns*Row_per_cpu) , MPI_CHAR, Transform_2D, (sizeof(Complex)*nColumns*Row_per_cpu), MPI_CHAR, j, MPI_COMM_WORLD);
	}
   	TransposeMatrix(Transform_2D, nColumns, nRows, Tpose); 
       	
	if(rank==0)
   	{
     		image.SaveImageData("MyAfter2d.txt", Tpose, nRows, nColumns);
   	}

        // Inverse Calculations
	//Id Inverse Transform Matrix
  	Complex * Transform_Inv = new Complex[nColumns*nRows];

        //2D Inverse Transform Matrix
  	Complex * Transform_Inv_2D = new Complex[nColumns*nRows];
	
	// Step(4)
	//Running loop for every CPU, CPU will execute only their Rows of Matrix
  	for(int i =0; i < Row_per_cpu ; i++)
  	{
		Transform1D( (Tpose + (Row_per_cpu*rank*nColumns) + (i*nColumns)), nColumns, (Transform_Inv + (Row_per_cpu*rank*nColumns) + (i*nColumns)),true);
  	}

	//Temp Array pointing to a CPU's Computed transoform
	temp_send_array = (Transform_Inv + (Row_per_cpu*rank*nColumns));
  	
	//Lets Merge. Using gather to merge everything
	for(int j =0 ; j<nCPUs; j++)
  	{

   	 	MPI_Gather( (temp_send_array), (sizeof(Complex)*nColumns*Row_per_cpu) , MPI_CHAR, Transform_Inv, (sizeof(Complex)*nColumns*Row_per_cpu), MPI_CHAR, j, MPI_COMM_WORLD);
   
  	}

	//Putting barrier for just in case
   	MPI_Barrier(MPI_COMM_WORLD);
    
	//Let any rank get me 1D transform
    	if(rank==0)
   	{
   	 	image.SaveImageData("MyAfter1d_Inv.txt", Transform, nRows, nColumns);
   	}
   	
	//Traanspose Matrix to do 2D transform
  	TransposeMatrix(Transform_Inv, nColumns, nRows, Tpose);
	
	//Again just in case
   	MPI_Barrier(MPI_COMM_WORLD);

	//Using any rank to print Transpose
   	if(rank==0)
   	{
    		image.SaveImageData("MyAfter1d_Inv_tpose.txt", Tpose, nRows, nColumns);
   	}
	
	//Again just in case
   	MPI_Barrier(MPI_COMM_WORLD);
  
	//Lets get 2D now!
	for(int i =0; i < Row_per_cpu ; i++)
	{
		Transform1D( (Tpose + (Row_per_cpu*rank*nColumns) + (i*nColumns)), nColumns, (Transform_Inv_2D + (Row_per_cpu*rank*nColumns) + (i*nColumns)), true);
    	}
  	
	temp_send_array = (Transform_Inv_2D + (Row_per_cpu*rank*nColumns));
  	
	for(int j =0 ; j<nCPUs; j++)
  	{
		MPI_Gather( (temp_send_array), (sizeof(Complex)*nColumns*Row_per_cpu) , MPI_CHAR, Transform_Inv_2D, (sizeof(Complex)*nColumns*Row_per_cpu), MPI_CHAR, j, MPI_COMM_WORLD);
	}
   	TransposeMatrix(Transform_Inv_2D, nColumns, nRows, Tpose); 

	if(rank==0)
   	{
    		image.SaveImageData("MyAfterInverse.txt", Tpose, nRows, nColumns);
   	}

  	MPI_Finalize();
}


void Transform1D(Complex* h, int w, Complex* H, bool Inverse)
{
  // Implement a simple 1-d DFT using the double summation equation
  // given in the assignment handout.  h is the time-domain input
  // data, w is the width (N), and H is the output array.
  	double angle;
  	Complex W, temp;
 		
  	for(int i =0; i< w; i++)
  	{
		for(int j=0; j< w; j++)
		{ 
	   		angle = (2*M_PI*j*i)/w;
         			
			if(!Inverse)
			{ 
				W.real = cos(angle);
        	   		W.imag = sin(angle);
           			temp = W.Conj()*h[j];
			}
			else
			{
				W.real = (double)(cos(angle)/w);
	           		W.imag = (double)(sin(angle)/w);
				temp = W*h[j];
			}
           		
			H[i].real = H[i].real +  temp.real;
          	 	H[i].imag = H[i].imag +  temp.imag; 
 	 	}
 	}
}

void TransposeMatrix(Complex* I, int w, int h, Complex* H)
{
   for(int i = 0; i<w; i++)
   {
	for(int j=0; j<h; j++)
        {
           H[i*w+j].real=I[j*w+i].real;
           H[i*w+j].imag=I[j*w+i].imag;
        }
   }
}

int main(int argc, char** argv)
{
  int rc;

  rc = MPI_Init(&argc,&argv);
  if (rc != MPI_SUCCESS) {
    printf ("Error starting MPI program. Terminating.\n");
    MPI_Abort(MPI_COMM_WORLD, rc);
  }

  string fn("Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  Transform2D(fn.c_str()); // Perform the transform.
}  
  

  
