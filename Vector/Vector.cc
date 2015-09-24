// Implementation of the templated Vector class
// ECE4893/8893 lab 3
// YOUR NAME HERE

#include <iostream> // debugging
#include <stdlib.h>
#include "Vector.h"

// Your implementation here
// Fill in all the necessary functions below
using namespace std;

// Default constructor
template <typename T>
Vector<T>::Vector():count(0),reserved(0),elements(NULL)
{
}

// Copy constructor
template <typename T>
Vector<T>::Vector(const Vector& rhs):count(rhs.count),reserved(rhs.reserved)
{
	//If elements is not poiting to NULL
	if(rhs.elements){

		//alocate memory for copy
		elements = (T*)malloc(sizeof(T)*reserved);

		//Copy elements
		for(unsigned int i=0; i<count; i++){
			//Call in-place new on elements
			new(elements+i)T(rhs.elements[i]);
		}
	}

	//else make it NULL
	else{
		elements = NULL;
	}
}

// Assignment operator
template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& rhs)
{
	//If same then return
	if(this==&rhs){
		return *this;
	}

	//Call destructors on the present sets
	for(int i=0; i<count; i++){
		elements[i].~T();
	}

	//Deallocate memory
	free(elements);

	//Make count and reserved equal
	count = rhs.count;
	reserved = rhs.reserved;

	//If elements is not poiting to NULL
	if(rhs.elements){

		//alocate memory for copy
		elements = (T*)malloc(sizeof(T)*reserved);

		//Copy elements
		for(int i=0; i<count; i++){
			//Call in-place new on elements
			new(elements+i)T();
			elements[i] = rhs.elements[i];
		}
	}
	//else make it NULL
	else{
		elements = NULL;
	}
}

#ifdef GRAD_STUDENT
// Other constructors
template <typename T>
Vector<T>::Vector(size_t nReserved)
{ // Initialize with reserved memory
	//If elements is not NULL
	if(elements){
		for(int i=0; i<count; i++){
			elements[i].~T();
		}
		free(elements);
	}

	elements = (T*)malloc(sizeof(T)*nReserved);
	reserved = nReserved;
	count =0;
}

template <typename T>
Vector<T>::Vector(size_t n, const T& t)
{ // Initialize with "n" copies of "t"
	/*
	if(elements){
		std::cout << "Check elemets" << endl;
		for(unsigned int i=0; i<count; i++){
			std::cout << "Destructing element " << i << endl;
			elements[i].~T();
		}
		free(elements);
	}*/

	reserved = n;
	count = n;
	elements = (T*)malloc(sizeof(T)*n);
	for(unsigned int i=0;i<n;i++){
		//Call in-place new on elements
	//	std::cout << "Printing element:" << i << endl;
		new(elements+i)T(t);
	}
}
#endif

// Destructor
template <typename T>
Vector<T>::~Vector()
{
	if(elements){
		for(unsigned int i=0; i<count; i++){
			elements[i].~T();
		}
		free(elements);
	}
}

// Add and access front and back
template <typename T>
void Vector<T>::Push_Back(const T& rhs)
{
	if(count == reserved){
		//copy the present vector into a temprory vector
		T* temp_elements;
		temp_elements= (T*)malloc(sizeof(T)*(reserved+1));

		//Increment reserved and count
		reserved+=1;
		count+=1;

		//reallocate memory
		for(unsigned int i =0; i<(count-1);i++){
			//Call in-place new on elements
			new(temp_elements+i)T(elements[i]);
			elements[i].~T();
		}

		//copy the push back element
		//Call in-place new on elements
		new(temp_elements + (count-1))T(rhs);

		//free elements
		free(elements);

		//Point elements to new elements
		elements = temp_elements;
	}

	else{
		count +=1;
		new(elements+count-1)T(rhs);
	}
}

template <typename T>
void Vector<T>::Push_Front(const T& rhs)
{
	if(count==reserved){

			T* temp_elements= (T*)malloc(sizeof(T)*(reserved+1));

			count +=1;
			reserved +=1;

			//Call in-place new on elements
			new(temp_elements)T(rhs);

			for(unsigned int i =1; i<(count);i++){
				//Call in-place new on elements
				new(temp_elements+i)T(elements[i-1]);
				//Destruct the element
				elements[i-1].~T();
			}

			free(elements);
			elements = temp_elements;
		}
	else{
		for(unsigned int i =count; i>0; i--){

			//Call in-place new on elements
			new(elements+i)T(elements[i-1]);
			elements[i-1].~T();
		}
		new(elements)T(rhs);
		count +=1;
	}
}

template <typename T>
void Vector<T>::Pop_Back()
{ // Remove last element
	elements[count-1].~T();
	count--;
}

template <typename T>
void Vector<T>::Pop_Front()
{ // Remove first element
	T* temp_elements = (T*)malloc(sizeof(T)*(reserved));
	for(unsigned int i =0; i <count-1 ; i++){
		new(temp_elements+i)T(elements[i+1]);
		elements[i+1].~T();
	}
	elements[0].~T();
	free(elements);
	elements = temp_elements;
	count--;
}

// Element Access
template <typename T>
T& Vector<T>::Front() const
{
	return(elements[0]);
}

// Element Access
template <typename T>
T& Vector<T>::Back() const
{
	return(elements[count-1]);
}

template <typename T>
T& Vector<T>::operator[](size_t i) const
{
	return(elements[i]);
}

template <typename T>
size_t Vector<T>::Size() const
{
	return(count);
}

template <typename T>
bool Vector<T>::Empty() const
{
	return(((count==0)?true:false));
}

// Implement clear
template <typename T>
void Vector<T>::Clear()
{
	for(unsigned int i=0; i<count; i++){
		elements[i].~T();
	}
	count=0;
}

//Reserve Method
template <typename T>
void Vector<T>::Reserve(size_t n){
	this->reserved =n;

	elements = (T*)malloc(sizeof(T)*n);
}
// Iterator access functions
template <typename T>
VectorIterator<T> Vector<T>::Begin() const
{
  return VectorIterator<T>(elements);
}

template <typename T>
VectorIterator<T> Vector<T>::End() const
{
	return VectorIterator<T>(elements+count);
}

#ifdef GRAD_STUDENT
// Erase and insert
template <typename T>
void Vector<T>::Erase(const VectorIterator<T>& it)
{
	unsigned int temp_pos;
	bool element_found = false;
	for(unsigned int i=0; i<count; i++){
		if((elements+i)==it.current){
			temp_pos = i;
			element_found = true;
			break;
		}
	}

	if(element_found == true){
		for(unsigned int i= temp_pos; i< count-1; i++){
			elements[i].~T();
			new(elements+i)T(elements[i+1]);
		}
		this->Pop_Back();
	}

}

template <typename T>
void Vector<T>::Insert(const T& rhs, const VectorIterator<T>& it)
{
	//Temprorary variables
	//Temprorary position variable
	unsigned int temp_pos = 0;
	//Did we find the element?
	bool elements_found = false;

	//Try finding the element
	for(unsigned int i=0; i<count;i++){
		if((elements+i)==it.current){
			//Register element
			temp_pos = i;
			elements_found = true;
			//Break away
			break;
		}
	}

	//If count and reserved are equal
	if(count == reserved){
		//If element was found
		if(elements_found == true){
			//Allocate memory to temproray variables
			T* temp_elements =(T*)malloc(sizeof(T)*(reserved+1));
			//Increment reserved
			reserved++;
			//Copy elements from till temp pos
			for(unsigned int i=0;i<temp_pos;i++){
				new(temp_elements+i)T(elements[i]);
				elements[i].~T();
			}

			//New element in temp positions position
			new(temp_elements+temp_pos)T(rhs);

			//Copy stuff after temp position
			for(unsigned int i= count ; i > temp_pos; i--){
				new(temp_elements+i)T(elements[i-1]);
				elements[i-1].~T();
			}

			//free elements
			free(elements);
			//Assign ne elements
			elements = temp_elements;
			//Increment count
			count++;
		}
	}

	else{
		for(unsigned int i=count; i>temp_pos; i--){
			new(elements+i)T(elements[i-1]);
			elements[i-1].~T();
		}
		elements[temp_pos].~T();
		new(elements+temp_pos)T(rhs);
		count++;
	}
}
#endif

// Implement the iterators

// Constructors
template <typename T>
VectorIterator<T>::VectorIterator():current(NULL)
{
}

template <typename T>
VectorIterator<T>::VectorIterator(T* c):current(c)
{
}

// Copy constructor
template <typename T>
VectorIterator<T>::VectorIterator(const VectorIterator<T>& rhs)
{
	current = rhs.current;
}

// Iterator defeferencing operator
template <typename T>
T& VectorIterator<T>::operator*() const
{
	return(*(this->current));
}

// Prefix increment
template <typename T>
VectorIterator<T>  VectorIterator<T>::operator++()
{
	this->current+=1;
	VectorIterator<T> temp = *this;
	return(temp);
}

template <typename T>
VectorIterator<T>  VectorIterator<T>::operator--()
{
	this->current--;
	return(*this);
}
// Postfix increment
template <typename T>
VectorIterator<T> VectorIterator<T>::operator++(int)
{
	VectorIterator<T> temp;
	temp = *this;
	this->current++;
	return(temp);
}

// Comparison operators
template <typename T>
bool VectorIterator<T>::operator !=(const VectorIterator<T>& rhs) const
{
	if(this->current!=rhs.current){
		return(true);
	}
	else{
		return(false);
	}
}

template <typename T>
bool VectorIterator<T>::operator ==(const VectorIterator<T>& rhs) const
{
	return((this->current==rhs.current)?true:false);
}




