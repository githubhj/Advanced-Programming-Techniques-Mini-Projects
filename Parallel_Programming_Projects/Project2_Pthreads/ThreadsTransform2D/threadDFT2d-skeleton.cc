// Threaded two-dimensional Discrete FFT transform
// Harshit Jain
// ECE8893 Project 2


#include <iostream>
#include <string>
#include <math.h>
#include <pthread.h>
#include "Complex.h"
#include "InputImage.h"
#include <stdlib.h>
#include "My_Barrier.h"

#define NUM_THREADS 16
// You will likely need global variables indicating how
// many threads there are, and a Complex* that points to the
// 2d image being transformed.

using namespace std;
Complex* Image_Data_Ptr;
unsigned int Image_Height;
unsigned int Image_Width;
Complex* W;

pthread_mutex_t ThreadLeftMutex;
pthread_mutex_t DoTransposeMutex;
pthread_cond_t DoTransposeCond;
unsigned int ThreadLeftCount;

My_Barrier* Barrier;

// Function to reverse bits in an unsigned integer
// This assumes there is a global variable N that is the
// number of points in the 1D transform.
unsigned ReverseBits(unsigned v)
{ //  Provided to students
  unsigned n = Image_Width; // Size of array (which is even 2 power k value)
  unsigned r = 0; // Return value
   
  for (--n; n > 0; n >>= 1)
    {
      r <<= 1;        // Shift return value
      r |= (v & 0x1); // Merge in next bit
      v >>= 1;        // Shift reversal value
    }
  return r;
}

// GRAD Students implement the following 2 functions.
// Undergrads can use the built-in barriers in pthreads.

// Call MyBarrier_Init once in main
/*
void MyBarrier_Init()
{
	pthread_mutex_init(&ThreadLeftMutex,0);
	pthread_mutex_init(&DoTransposeMutex,0);
	pthread_cond_init(&DoTransposeCond,0);
	ThreadLeftCount = (unsigned int)NUM_THREADS;
}

// Each thread calls MyBarrier after completing the row-wise DFT
void MyBarrier(unsigned int my_id) // Again likely need parameters
{
	cout << "Thread "<<my_id<< " is done!"<<endl;
	pthread_mutex_lock(&ThreadLeftMutex);
	ThreadLeftCount--;
	if (ThreadLeftCount == 0)
	{ // Last to exit
		pthread_mutex_unlock(&ThreadLeftMutex);
		pthread_mutex_lock(&DoTransposeMutex);
		pthread_cond_signal(&DoTransposeCond);
		pthread_mutex_unlock(&DoTransposeMutex);
	}
	else
	{
		pthread_mutex_unlock(&ThreadLeftMutex);
	}
	while(ThreadLeftCount!=NUM_THREADS){}; //spin till main releases
	cout << "Thread: "<< my_id << " I am out"<< endl;
}
*/
void Compute_Weights(unsigned int N, bool inverse){

	W = new Complex[N/2];
	double angle = 2*M_PI/N;
	for(unsigned int i =0; i<N/2 ; i++){
		double theta = angle*i;
		W[i].real = (double)cos(theta);
		W[i].imag = -(double)sin(theta);
		if(inverse ==true){
			W[i].imag = -W[i].imag;
		}
	}
}
                    
void Transform1D(Complex* h, unsigned int N)
{
  // Implement the efficient Danielson-Lanczos DFT here.
  // "h" is an input/output parameter
  // "N" is the size of the array (assume even power of 2)

	Complex* rev_h = new Complex[N];
	for(unsigned int i=0 ; i < N; i++){
		rev_h[i] = h[i];
	}
	//Swap the data as per Cooley Turkey
	for(unsigned int i=0; i<N; i++){
		h[i] = rev_h[ReverseBits(i)];
	}
	Complex temp;
	delete rev_h;

	//Get Id transform as per Danielson–Lanczos

	for(unsigned int i = 2; i <= N ; i*=2){
		unsigned int l = i/2;
		for(unsigned int j = 0; j< N; j+=i){
			for(unsigned k=0; k< (i/2); k++){
				temp = h[k+j];
				h[j+k] = h[j+k] + W[k*N/i]*h[j+k+l];
				h[j+k+l] = temp - W[k*N/i]*h[j+k+l];
			}
		}
	}
}

void DividebyN(Complex* H, unsigned int N){
	for(unsigned int i = 0; i<N ;i++){
		H[i] = H[i]/double(N);
	}
}

void* Transform2DTHread(void* v)
{ // This is the thread starting point.  "v" is the thread number
	unsigned int my_id = (unsigned int)v;
	unsigned int my_row_count = Image_Width/NUM_THREADS;

	// Calculate 1d DFT for assigned rows
	cout << "My Id is: "<< my_id << endl;
	for(unsigned int row_count =0 ; row_count < my_row_count; row_count++){
		cout << "Thread : " << my_id << "is doing row: " << (row_count+(my_row_count*my_id)) << endl;
		Transform1D(&Image_Data_Ptr[(row_count+(my_row_count*my_id))*Image_Width], Image_Width);
	}

  // wait for all to complete
	Barrier->Start(my_id);

  //Wait for transpose
	Barrier->Start(my_id);

  // Calculate 1d DFT for assigned columns
	for(unsigned int row_count =0 ; row_count < my_row_count; row_count++){
			Transform1D(&Image_Data_Ptr[(row_count+(my_row_count*my_id))*Image_Width], Image_Width);
		}

	// 2D done go and hit the barrier
	Barrier->Start(my_id);

	// Now time for 1D Inverse
	//Wait till main saves 2d transform image and gets new weights
	Barrier->Start(my_id);

	// Calculate 1D Inverse DFT
	for(unsigned int row_count =0 ; row_count < my_row_count; row_count++){
				Transform1D(&Image_Data_Ptr[(row_count+(my_row_count*my_id))*Image_Width], Image_Width);
				DividebyN(&Image_Data_Ptr[(row_count+(my_row_count*my_id))*Image_Width], Image_Width);
			}

	//Wait for all to be done
	Barrier->Start(my_id);

	//Wait for saving and transpose
	Barrier->Start(my_id);

	// Calculate 2D Inverse DFT
		for(unsigned int row_count =0 ; row_count < my_row_count; row_count++){
					Transform1D(&Image_Data_Ptr[(row_count+(my_row_count*my_id))*Image_Width], Image_Width);
					DividebyN(&Image_Data_Ptr[(row_count+(my_row_count*my_id))*Image_Width], Image_Width);
				}

	Barrier->Start(my_id);

  return 0;
}

void TransposeMatrix(Complex* I, int w, int h)
{
   Complex* H= new Complex[w*h];

   for(int i = 0; i<w; i++){
	   for(int j=0; j<h; j++){
		   H[i*w+j].real=I[i*w+j].real;
		   H[i*w+j].imag=I[i*w+j].imag;
	   }
   }

   for(int i = 0; i<w; i++)
   {
	for(int j=0; j<h; j++)
        {
           I[i*w+j].real=H[j*w+i].real;
           I[i*w+j].imag=H[j*w+i].imag;
        }
   }

   delete H;
}

void Transform2D(const char* inputFN) 
{ // Do the 2D transform here.

  // Create the helper object for reading the image
  InputImage image(inputFN);

  // Create the global pointer to the image array data
  Image_Data_Ptr = image.GetImageData();
  Image_Height = image.GetHeight();
  Image_Width = image.GetWidth();

  //Compute Weights for Transform
  Compute_Weights(Image_Width,false);

  //Create My Barrier for simple case
  //MyBarrier_Init();
  Barrier = new My_Barrier(NUM_THREADS + 1); // +1 for my main


  // Create 16 threads/
  for(int i =0; i<NUM_THREADS; i++){
	  pthread_t p;
	  pthread_create(&p,0, Transform2DTHread, (void*)i);
  }

  //Wait for 16 to complete
  Barrier->Start(NUM_THREADS);

  //Threads done with 1D, do transpose
  image.SaveImageData("MyAfter1D.txt", Image_Data_Ptr, Image_Height, Image_Width);
  TransposeMatrix(Image_Data_Ptr, Image_Width, Image_Height);

  //You are done with transform sync up with threads
  Barrier->Start(NUM_THREADS);

  //Threads staring 2D transform wait till they finish
  Barrier->Start(NUM_THREADS);

  //Threads done with 2D, do transpose
   TransposeMatrix(Image_Data_Ptr, Image_Width, Image_Height);

  // Write the transformed data
  image.SaveImageData("MyAfter2D.txt", Image_Data_Ptr, Image_Height, Image_Width);

  //Compute new weights
 Compute_Weights(Image_Width, true);

  //you are done with transform and writing files go sync up
  Barrier->Start(NUM_THREADS);

  //Threads doing Inverse 1D transform Wait till they finish
   Barrier->Start(NUM_THREADS);

   //Threads done with 1D Inverse, do transpose
   image.SaveImageData("MyAfter1DInverse.txt", Image_Data_Ptr, Image_Height, Image_Width);
   TransposeMatrix(Image_Data_Ptr, Image_Width, Image_Height);

   //Synch up with all threads
   Barrier->Start(NUM_THREADS);

   //Wait till every guy finishes 2D inverse
   Barrier->Start(NUM_THREADS);

   //Transpose
   TransposeMatrix(Image_Data_Ptr, Image_Width, Image_Height);

   //Print image
   image.SaveImageData("MyAfter2DInverse.txt", Image_Data_Ptr, Image_Height, Image_Width);

/*
  //Wait till threads complete 1D
  pthread_cond_wait(&DoTransposeCond, &DoTransposeMutex);

  //Threads done with 1D, do transpose
  image.SaveImageData("MyAfter1DInverse.txt", Image_Data_Ptr, Image_Height, Image_Width);
  TransposeMatrix(Image_Data_Ptr, Image_Width, Image_Height);
  //Hold the mutex for conditional signal

  //Decrement thread count
  pthread_mutex_lock(&ThreadLeftMutex);

  //Initialize Transpose mutexes again
  pthread_mutex_init(&DoTransposeMutex,0);
  pthread_cond_init(&DoTransposeCond, 0);
  pthread_mutex_lock(&DoTransposeMutex);

  //Make thread count so as to begin again
  ThreadLeftCount = (unsigned int)NUM_THREADS;
  pthread_mutex_unlock(&ThreadLeftMutex);

   //Wait till threads complete 2D
   pthread_cond_wait(&DoTransposeCond, &DoTransposeMutex);

   //Threads done with 2D, do transpose
   TransposeMatrix(Image_Data_Ptr, Image_Width, Image_Height);

   // Write the transformed data
   image.SaveImageData("MyAfterInverse.txt", Image_Data_Ptr, Image_Height, Image_Width);

   //Compute Weights for inverse
   Compute_Weights(Image_Width, true);

   //Reset thread count
   pthread_mutex_lock(&ThreadLeftMutex);

   //Initialize Transpose mutexes again
   pthread_mutex_init(&DoTransposeMutex,0);
   pthread_cond_init(&DoTransposeCond, 0);
   pthread_mutex_lock(&DoTransposeMutex);

   //Make thread count so as to begin again
   ThreadLeftCount = (unsigned int)NUM_THREADS;
   pthread_mutex_unlock(&ThreadLeftMutex);

   //Wait till threads complete 1D Inverse
   pthread_cond_wait(&DoTransposeCond, &DoTransposeMutex);



   //Decrement thread count
   pthread_mutex_lock(&ThreadLeftMutex);
   ThreadLeftCount=NUM_THREADS;
   pthread_mutex_unlock(&ThreadLeftMutex);

   pthread_mutex_destroy(&ThreadLeftMutex);
   pthread_cond_destroy(&DoTransposeCond);
   pthread_mutex_destroy(&DoTransposeMutex);*/
}



int main(int argc, char** argv)
{
  string fn("Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  // MPI initialization here
  Transform2D(fn.c_str()); // Perform the transform.
}


void *PrintHello(void *threadid)
{
   long tid;
   tid = (long)threadid;
   printf("Hello World! It's me, thread #%ld!\n", tid);
   pthread_exit(NULL);
}


  
