
// Stack.h.   release 3.2.  May 3, 2007.
//
//   A general purpose dynamically resizable stack.
//   Implemented with templates.
//	Items are stored contiguously, for quick accessing.
//		However, allocation may require the stack to be copied into
//		new memory locations.
//
// Author: Sam Buss.
// Contact: sbuss@math.ucsd.edu
// All rights reserved.  May be used for any purpose as long
//	as use is acknowledged.


#ifndef STACK_H
#define STACK_H

#include <assert.h>

#include "../VrMath/MathMisc.h"

template <class T> class Stack {

public:
	Stack();						// Constructor
	Stack(long initialSize);		// Constructor
	~Stack();					// Destructor

	void Reset();

	void Resize( long newMaxSize );	// Increases allocated size (will not decrease size)

	T& Top() const { return *TopElement; };
	T& Pop();

	T* Push();					// New top element is arbitrary
	T* Push( const T& newElt );			// Push newElt onto stack.

	bool IsEmpty() const { return (SizeUsed==0); }

	long Size() const { return SizeUsed; }
	long SizeAllocated() const { return Allocated; }

private:

	long SizeUsed;				// Number of elements in the stack
	T* TopElement;				// Pointer to the top element of the stack
	long Allocated;				// Number of entries allocated
	T* TheStack;				// Pointer to the array of entries
};

template<class T> inline Stack<T>::Stack()
{ 
	SizeUsed = 0;
	TheStack = 0;
	Allocated = 0;
	Resize( 10 );
}

template<class T> inline Stack<T>::Stack(long initialSize)
{
	SizeUsed = 0;
	TheStack = 0;
	Allocated = 0;
	Resize( initialSize );
}


template<class T> inline Stack<T>::~Stack()
{
	delete[] TheStack;
}

template<class T> inline void Stack<T>::Reset()
{
	SizeUsed = 0;
	TopElement = TheStack-1;
}

template<class T> inline void Stack<T>::Resize( long newMaxSize )
{
	if ( newMaxSize <= Allocated ) {
		return;
	}
	long newSize = Max(2*Allocated+1,newMaxSize);
	T* newArray = new T[newSize];
	T* toPtr = newArray;
	T* fromPtr = TheStack;
	long i;
	for ( i=0; i<SizeUsed; i++ ) {
		*(toPtr++) = *(fromPtr++);
	}
	delete[] TheStack;
	TheStack = newArray;
	Allocated = newSize;
	TopElement = TheStack+(SizeUsed-1);
}

template<class T> inline T& Stack<T>::Pop()
{
	T* ret = TopElement;
	assert( SizeUsed>0 );		// Should be non-empty
	SizeUsed--;
	TopElement--;
	return *ret;
}

// Enlarge the stack but do not update the top element.
//    Returns a pointer to the top element (which is unchanged/uninitialized)
template<class T> inline T* Stack<T>::Push( )
{
	if ( SizeUsed >= Allocated ) {
		Resize(SizeUsed+1);
	}
	SizeUsed++;
	TopElement++;
	return TopElement;
}

template<class T> inline T* Stack<T>::Push( const T& newElt )
{
	Push();
	*TopElement = newElt;
	return TopElement;
}


#endif	// STACK_H