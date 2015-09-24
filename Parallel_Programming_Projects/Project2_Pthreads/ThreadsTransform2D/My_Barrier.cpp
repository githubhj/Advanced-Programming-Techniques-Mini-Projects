//Harshit Jain
//Barrier Class
#include <iostream>
#include <pthread.h>
#include <string.h>
#include "My_Barrier.h"
using namespace std;
My_Barrier::My_Barrier(unsigned int nThreads_t):nThreads(nThreads_t),present_threads(nThreads_t){

	pthread_mutex_init(&barrier_mutex,0);
	threadswitch = new bool[nThreads];
	//memset(threadswitch, true, (sizeof(threadswitch)/sizeof(bool)));
	for (int i = 0; i < nThreads; ++i) threadswitch[i] = true;
	mainswitch = true;
}

void My_Barrier::Start(unsigned int ThreadID){
	threadswitch[ThreadID] = !threadswitch[ThreadID];
	if(reduce_thread_count() == 1){
		cout << "Last thread: " << ThreadID<< endl<<endl << endl;
		present_threads = nThreads;
		mainswitch = threadswitch[ThreadID];
	}

	else{
		cout << "Running threadID" << ThreadID << endl;
		while(mainswitch != threadswitch[ThreadID]);
	}
}

unsigned int My_Barrier::reduce_thread_count(){
	pthread_mutex_lock(&barrier_mutex);
	unsigned int left_threads = present_threads;
	present_threads--;
	pthread_mutex_unlock(&barrier_mutex);
	return left_threads;
}

My_Barrier::~My_Barrier(){
	pthread_mutex_destroy(&barrier_mutex);
	delete threadswitch;
}
