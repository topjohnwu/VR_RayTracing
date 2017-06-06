
// ShellSort.h
//
// A ShellSort, written as templated C++ code.
//	Uses the h = 3h+1 recursion to set increment size.
//	Uses pointers to improve runtime.
//
// Author: Sam Buss, sbuss@math.ucsd.edu.
//		Freely usable without restrictions, 
//		Distributed "as is", with no warranties. 
//		I would appreciate acknowledgements of its use, 
//		as well as being informed of bugs or other improvements.


// Do a shell sort.
//   Pass a pointer to an array, and the number of objects
//   The class "T" must have "operator< ()" defined.

#include <assert.h>

template <class T>
bool DebugTestSort( T *array, long num, long* i );

template <class T>
void ShellSort( T *array, long num )
{
	//  Use the 3h+1 formula for shell sort increment.
	//		(Not sure if this is really best.  Sedgewick suggests another sequence)
	long incr = 1;
	long nextIncr = 4;
	while ( nextIncr<num ) {
		incr = nextIncr;
		nextIncr = 3*nextIncr + 1;
	}

	T temp;
	while ( incr>0 ) {
		// Do many bubble sorts of step size incr
		T* startSort = array+incr;
		for ( long i = incr; i<num; i++, startSort++ ) {
			T* backPtr = startSort-incr;
			if ( (*startSort)<(*backPtr) ) {
				temp = *startSort;
				long j = i-incr;		// Index of *backPtr
				T* thisPtr = startSort;
				do {
					*thisPtr = *backPtr;
					thisPtr = backPtr;
					backPtr -= incr;
					j -= incr;
				} while (j>=0 && temp<(*backPtr) );
				*thisPtr = temp;
			}
		}
		incr /= 3;
	}

	// DEBUG ONLY
	assert ( DebugTestSort( array, num, &incr ) );

}

template <class T>
bool DebugTestSort( T *array, long num, long* iFail )
{
	bool retCode = true;
	long i;
	for ( i=0; i<num-1; i++ ) {
		if ( array[i+1]<array[i] ) {
			retCode = false;
			break;
		}
	}
	*iFail = i;
	return retCode;
}

