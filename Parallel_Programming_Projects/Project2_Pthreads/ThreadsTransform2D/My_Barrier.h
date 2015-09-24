/*
 * My_Barrier.h
 *
 *  Created on: Sep 29, 2014
 *      Author: harshit
 */

#ifndef MY_BARRIER_H_
#define MY_BARRIER_H_


class My_Barrier{
	public:
		//constructor
		My_Barrier(unsigned int);
		My_Barrier(unsigned int, unsigned int);
		~My_Barrier();
		void Start(unsigned int);
		//void* specialfunc();

	private:

		unsigned int nThreads;
		unsigned int present_threads;
		unsigned int reduce_thread_count();
		pthread_mutex_t barrier_mutex;
		bool* threadswitch;
		bool mainswitch;

};


#endif /* MY_BARRIER_H_ */
