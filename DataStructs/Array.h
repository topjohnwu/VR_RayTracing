/*
 *
 * RayTrace Software Package, release 3.3.  August 10, 2010.
 *
 * Data Structures Subpackage (DataStructs)
 *
 * Author: Samuel R. Buss
 *
 * Software accompanying the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.  Please acknowledge
 *   all use of the software in any publications or products based on it.
 *
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 *
 */

// Array.h
//
//   A general purpose dynamically resizable array.
//   Implemented with templates.
//	Items are stored contiguously, for quick indexing.
//		However, allocation may require the array to be copied into
//		new memory locations.
//	Because of the dynamic resizing, you should be careful to understand
//		how the array code works before using it.  Care should be taken 
//		if the array might be resized during an expression evaluation.
//
// Author: Sam Buss.
// Contact: sbuss@math.ucsd.edu
// All rights reserved.  May be used for any purpose as long
//	as use is acknowledged.


#ifndef ARRAY_H
#define ARRAY_H

#include <assert.h>
#include <stdlib.h>

#include "../VrMath/MathMisc.h"

template <class T> class Array {

public:
	Array();						// Constructor
	Array(long initialSize);		// Constructor
	Array( const Array<T>& );		// Constructor (presently disabled)
	~Array();						// Destructor

	bool InitialAllocationSucceeded() const { return (TheEntries!=0); }

	void Reset() { ReducedSizeUsedNoCheck(0); }

	// Next two routines: Main purpose to control allocation.
	//   Note: Allocation can occur without warning if you do not take care.
	//	 When allocation occurs, pointer and references become bad.
	// Use this judiciously to control when the array may be resized
	bool PreallocateMore( long numAdditional ) { return Resize( SizeUsed()+numAdditional ); }
	bool Resize( long newMaxSize );	// Increases allocated size (will not decrease allocated size)

	// ExplicitAllocate allows for taking control of the array allocation
	void ExplicitAllocate( T* newArrayLocation, long newSize );
	void ExplicitAllocateNoCopy( T* newArrayLocation, long newSize );
	void ExplicitAllocateDelete( T* newArrayLocation, long newSize );
	void ExplicitReduceSize( long reducedAllocatedSize );

	// Next routines are used to update the "SizeUsed" of the stack.
	void Touch( long i );			// Makes entry i active.  Increases SizeUsed (MaxEntryPlus)
	void ReducedSizeUsed( long i ); // "i" is the number of used entries now.
	void ReducedSizeUsedNoCheck( long i ); // "i" is the number of used entries now.
	void MakeEmpty() {MaxEntryPlus=0;};	// Makes the length "SizeUsed" equal zero.
	// Next four functions give the functionality of a stack.
	T& Top() { assert(MaxEntryPlus>0); return *(TheEntries+(MaxEntryPlus-1)); }
	const T& Top() const { assert(MaxEntryPlus>0); return *(TheEntries+(MaxEntryPlus-1)); }
	T& Pop();
	T* Push();					// Push with no argument returns pointer to new top element
	T* Push( const T& newElt );
	bool IsEmpty() const { return (MaxEntryPlus==0); }

	void DisallowDynamicResizing() { DynamicResizingOK = false; }
	void AllowDynamicResizing() { DynamicResizingOK = true; }

	// Access function - if i is out of range, it resizes
	T& operator[]( long i );
	const T& operator[]( long i ) const;

	// Access functions - Does not check whether i is out of range!!
	//		No resizing will occur!  Use only to get existing entries.
	T& GetEntry( long i );
	const T& GetEntry( long i ) const;
	T& GetFirstEntry();
	const T& GetFirstEntry() const;
	T& GetLastEntry();
	const T& GetLastEntry() const;
	T* GetFirstEntryPtr();
	const T* GetFirstEntryPtr() const;
	T* GetEntryPtr( long i );
	const T* GetEntryPtr( long i ) const;

	long SizeUsed() const;			// Number of elements used (= 1+(max index));

	long AllocSize() const;			// Size allocated for the array
	bool IsFull() const { return MaxEntryPlus==Allocated; }
	// long SizeAvailable() const { return Allocated-MaxEntryPlus; }

	Array<T>& operator=( const Array<T>& other );

	// Higher-level functions
	bool IsMember( const T& queryElt ) const;	// Check if present in array

private:

	long MaxEntryPlus;				// Maximum entry used, plus one (Usually same as size)
	long Allocated;					// Number of entries allocated
	T* TheEntries;					// Pointer to the array of entries

	bool DynamicResizingOK;			// If "true", array can be dynamically resized.
	bool TheEntriesExternallyHandled;	// True if "Explicit" allocation methods used.

};

template<class T> inline Array<T>::Array()
{ 
	MaxEntryPlus = 0;
	TheEntries = 0;
	Allocated = 0;
	DynamicResizingOK = true;
	TheEntriesExternallyHandled = false;
	Resize( 10 );
}

template<class T> inline Array<T>::Array(long initialSize)
{
	MaxEntryPlus = 0;
	TheEntries = 0;
	Allocated = 0;
	DynamicResizingOK = true;
	TheEntriesExternallyHandled = false;
	Resize( initialSize );
	assert( initialSize==0 || TheEntries!=0 );
}


template<class T> inline Array<T>::~Array()
{
	if ( !TheEntriesExternallyHandled ) {
		delete[] TheEntries;
	}
}

template<class T> inline bool Array<T>::Resize( long newMaxSize )
{
	assert( newMaxSize>=0 );
	if ( newMaxSize <= Allocated ) {
		return false;				// No actual reallocation is needed.
	}
	if ( !DynamicResizingOK ) {
		assert(false);
		exit(0);
	}
	Allocated = Max(2*Allocated+1,newMaxSize);
	T* newArray = new T[Allocated];
	if ( newArray==0 ) {
		return false;			// Return false to indicate that the reallocation failed.
	}
	T* toPtr = newArray;
	T* fromPtr = TheEntries;
	for ( long i=0; i<MaxEntryPlus; i++ ) {
		*(toPtr++) = *(fromPtr++);
	}
	assert( !TheEntriesExternallyHandled );
	delete[] TheEntries;
	TheEntries = newArray;	
	return true;				// Return true to indicate reallocation occurred.
}

// ExplicitAllocate allows for taking control of the array allocation
template<class T> inline void Array<T>::ExplicitAllocate( T* newArrayLocation, long newSize )
{
	assert( newSize>=MaxEntryPlus );
	T* toPtr = newArrayLocation;
	T* fromPtr = TheEntries;
	for ( long i=MaxEntryPlus; i>0; i-- ) {
		*(toPtr++) = *(fromPtr++);
	}
	Allocated = newSize;
	TheEntries = newArrayLocation;
	DynamicResizingOK = false;
	TheEntriesExternallyHandled = true;
}

template<class T> inline void Array<T>::ExplicitAllocateNoCopy( T* newArrayLocation, long newSize )
{
	assert( newSize>=MaxEntryPlus );
	Allocated = newSize;
	TheEntries = newArrayLocation;
	DynamicResizingOK = false;
	TheEntriesExternallyHandled = true;
}

template<class T> inline void Array<T>::ExplicitAllocateDelete( T* newArrayLocation, long newSize ) {
	T* oldEntries = TheEntries;
	ExplicitAllocate( newArrayLocation, newSize );
	delete oldEntries;
	TheEntriesExternallyHandled = true;
}

template<class T> inline void Array<T>::ExplicitReduceSize( long reducedAllocatedSize ) {
	assert ( reducedAllocatedSize>=MaxEntryPlus );
	Allocated = reducedAllocatedSize;
	assert( TheEntriesExternallyHandled == true );
}

template<class T> inline void Array<T>::Touch( long i )
{
	assert( i >= 0 );
	if ( i>=Allocated ) {
		Resize( i+1 );
	}
	if ( i >= MaxEntryPlus ) {
		MaxEntryPlus = i+1;
	}
}

template<class T> inline void Array<T>::ReducedSizeUsed( long i )
{
	// "i" is the number of used entries now.
	assert( i >= 0 );
	if ( i<MaxEntryPlus ) {
		MaxEntryPlus = i;
	}
}

template<class T> inline void Array<T>::ReducedSizeUsedNoCheck( long i )
{
	// "i" is the number of used entries now.
	assert( i >= 0 && i <= MaxEntryPlus );
	MaxEntryPlus = i;
}


template<class T> inline T& Array<T>::Pop()
{
	assert( MaxEntryPlus>0 );
	MaxEntryPlus-- ;
	return *(TheEntries+MaxEntryPlus);
}

template<class T> inline T* Array<T>::Push()
{
	if ( MaxEntryPlus>=Allocated ) {
		Resize( MaxEntryPlus+1 );
	}
	T* ret = TheEntries+MaxEntryPlus;
	MaxEntryPlus++;
	return ret;
}

template<class T> inline T* Array<T>::Push( const T& newElt )
{
	T* top = Push();
	*top = newElt;
	return top;
}

template<class T> inline T& Array<T>::operator[]( long i )
{
	assert( i >= 0 );
	if ( i>=Allocated ) {
		Resize( i+1 );
	}
	if ( i >= MaxEntryPlus ) {
		MaxEntryPlus = i+1;
	}
	return TheEntries[i];
}

template<class T> inline const T& Array<T>::operator[]( long i ) const
{
	assert( i >= 0 );
	if ( i>=Allocated ) {
		const_cast<Array<T>*>(this)->Resize( i+1 );
	}
	if ( i >= MaxEntryPlus ) {
		const_cast<Array<T>*>(this)->MaxEntryPlus = i+1;
	}
	return TheEntries[i];
}

template<class T> inline T& Array<T>::GetEntry( long i )
{
	assert( i >= 0 );
	assert ( i < MaxEntryPlus );
	return TheEntries[i];
}

template<class T> inline const T& Array<T>::GetEntry( long i ) const
{
	assert( i >= 0 );
	assert ( i < MaxEntryPlus );
	return TheEntries[i];
}

template<class T> inline T& Array<T>::GetFirstEntry()
{
	assert(Allocated>0);
	return *TheEntries;
}

template<class T> inline const T& Array<T>::GetFirstEntry() const
{
	assert(Allocated>0);
	return *TheEntries;
}

template<class T> inline T* Array<T>::GetFirstEntryPtr()
{
	assert(Allocated>0);
	return TheEntries;
}

template<class T> inline const T* Array<T>::GetFirstEntryPtr() const
{
	assert(Allocated>0);
	return TheEntries;
}

template<class T> inline T* Array<T>::GetEntryPtr( long i )
{
	assert( i >= 0 );
	assert( i < MaxEntryPlus );
	return TheEntries+i;
}

template<class T> inline const T* Array<T>::GetEntryPtr( long i ) const
{
	assert( i >= 0 );
	assert( i < MaxEntryPlus );
	return TheEntries+i;
}

template<class T> inline T& Array<T>::GetLastEntry()
{
	assert(MaxEntryPlus>0);
	return *(TheEntries+(MaxEntryPlus-1));
}

template<class T> inline const T& Array<T>::GetLastEntry() const
{ 
	assert(MaxEntryPlus>0);
	return *(TheEntries+(MaxEntryPlus-1));
}

template<class T> inline long Array<T>::SizeUsed () const
{
	return MaxEntryPlus;
}

template<class T> inline long Array<T>::AllocSize () const
{
	return Allocated;
}

// Check if queryElt present in array
template<class T> inline bool Array<T>::IsMember( const T& queryElt ) const
{
	long i;
	T* tPtr = TheEntries;
	for ( i=MaxEntryPlus; i>0; i--, tPtr++) {
		if ( (*tPtr) == queryElt ) {
			return true;
		}
	}
	return false;
}

// My thanks to Hans Dietrich for reporting a bug in an earlier untested version,
//    and assisting with the fix.
template<class T> inline Array<T>& Array<T>::operator=( const Array<T>& other )
{
	if ( TheEntries != other.TheEntries ) {
		Resize( other.MaxEntryPlus );
		MaxEntryPlus = other.MaxEntryPlus;
		long i;
		T* toPtr = TheEntries;
		const T* fromPtr = other.TheEntries;
		for ( i=MaxEntryPlus; i>0; i--, toPtr++, fromPtr++ ) {
			*toPtr = *fromPtr;
		}
	}
	return *this;
}

// copy ctor - Supplied by Hans Dietrich (Thanks!)
//  However in the present version below, I added an "assert(false)"
//  because it is easy to inadvertantly use a copy constructor 
//  and this is often a mistake for performance reasons.
// If you wish to use this, comment out the assert.
template<class T> inline Array<T>::Array(const Array<T>& other)
{
	assert ( false );
    TheEntries = 0;
    Allocated = 0;
    DynamicResizingOK = other.DynamicResizingOK;
    Resize(other.MaxEntryPlus);
    MaxEntryPlus = other.MaxEntryPlus;

    T* toPtr = TheEntries;
    const T* fromPtr = other.TheEntries;

    for (int i = MaxEntryPlus; i > 0; i--, toPtr++, fromPtr++) 
    {
        *toPtr = *fromPtr;
    }
}



#endif // ARRAY_H