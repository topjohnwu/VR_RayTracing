/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006.
 *
 * Mathematics Subpackage (VrMath)
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

#ifndef VR_DATA_H
#define VR_DATA_H

#include <math.h>

// Implements a doubly-linked list with a sentinal (called the root).

template< class T> class CLinkedListElt;

template< class T > class CLinkedList {

public:
	CLinkedList<T>();
	CLinkedList<T>( CLinkedList<T>& );
	~CLinkedList<T>();
	CLinkedList<T> operator=(const CLinkedList<T>&);

	bool IsEmpty() const { return RootElt->GetNext()->IsRoot(); }
	CLinkedListElt<T>* GetRoot() const { return RootElt; }
	CLinkedListElt<T>* GetFirst() const { return RootElt->GetNext(); }
	CLinkedListElt<T>* GetLast() const { return RootElt->GetPrev(); }
	CLinkedListElt<T>* AddElt( T* );
	CLinkedListElt<T>* AddHead( T* );
	static CLinkedListElt<T>* AddAfter( T*, CLinkedListElt<T>* );
	static CLinkedListElt<T>* Remove( CLinkedListElt<T> * );
	static CLinkedListElt<T>* DeleteRemove( CLinkedListElt<T> * );

	int GetNumElts() const;		// Number of elements in linked list (inefficient)

private:
	CLinkedListElt<T> *RootElt;
	int *RefCounter;				// Pointer to a reference counter
};

template< class T > class CLinkedListElt {
	friend class CLinkedList<T>;
	
private:
	T* Tptr;
	CLinkedListElt<T> *NextElt;
	CLinkedListElt<T> *PrevElt;

public:
	CLinkedListElt();
	~CLinkedListElt();
	void Remove();				// Removes the current element from the list
	void DeleteRemove();		// Deletes and removes the current element from the list
	void AddAfter( T* );		// Inserts a new element
	void Change ( T* );			// Changes the current element
	T* GetEntry( ) const;		// Return NULL if at end (=at root)
	CLinkedListElt<T>* GetNext() const;
	CLinkedListElt<T>* GetPrev() const;
	bool IsRoot() const;			// Returns TRUE it is root
	bool IsFirst() const;			// Returns TRUE if first in list
	bool IsLast() const;			// Returns TRUE if last in list.
};

// Adds a new element to the end of the list.
//  Returns a pointer to the new linked list element.
template<class T> inline CLinkedListElt<T>* CLinkedList<T>::AddElt( T* newT)
{
	return( AddAfter( newT, RootElt->GetPrev() ));
}

// Adds a new element to the head of the list.
//  Returns a pointer to the new linked list element.
template<class T> inline CLinkedListElt<T>* CLinkedList<T>::AddHead( T* )
{
	return( AddAfter( newT, RootElt) );
}

// Constructor for a linked list element.  Does not put it in a linked list
template<class T> inline CLinkedListElt<T>::CLinkedListElt() {
	Tptr = NULL;
	NextElt = this;
	PrevElt = this;
}

// Destructor for a linked list element.  Updates pointer in the linked
//	list so as to remove it from the list.
template<class T> inline CLinkedListElt<T>::~CLinkedListElt() {
	NextElt->PrevElt = PrevElt;
	PrevElt->NextElt = NextElt;
}

// Returns (a pointer to) the object in the linked list element.
template<class T> inline T* CLinkedListElt<T>::GetEntry() const {
	return ( Tptr );
}

// Returns the next element in the linked list. 
//    Wraps around from the last element to the root.
//	  Steps forward from the root to the head.
template<class T> inline CLinkedListElt<T>* CLinkedListElt<T>::GetNext() const {
	return ( NextElt );
}

// Returns the previous element in the linked list.
//	  Wraps around from the root to the last element.
//	  If at the head, steps back to the root.
template<class T> inline CLinkedListElt<T>* CLinkedListElt<T>::GetPrev() const {
	return ( PrevElt );
}

// Changes the object stored in the linked list element.
template<class T> inline void CLinkedListElt<T>::Change( T* newT ) {
	Tptr = newT;
}

// Checks if this is the root of the linked list.
template<class T> inline bool CLinkedListElt<T>::IsRoot() const
{
	return ( Tptr==NULL );
}

// Returns true if this element is the head (not the root) of the linked list
template<class T> inline bool CLinkedListElt<T>::IsFirst() const
{
	return ( PrevElt->IsRoot() && !IsRoot() );
}


// Returns true if this element is the last element in the linked list.
template<class T> inline bool CLinkedListElt<T>::IsLast() const
{
	return ( NextElt->IsRoot() && !IsRoot() );
}

// Constructor. Creates a new linked list with no elements.
template<class T> inline CLinkedList<T>::CLinkedList() 
{
	RootElt = new CLinkedListElt<T>();
	RefCounter = new int;
	*RefCounter = 1;
}

// Copy constructor. Clones a CLinkedList<T>.
template<class T> inline CLinkedList<T>::CLinkedList(CLinkedList<T>& l) 
{
	(*(l.RefCounter))++;
	RefCounter = l.RefCounter;
	RootElt = l.RootElt;
}

// Destructor. Frees the linked list elements, but not the objects 
//		pointed to by the linked list elements.
template<class T> inline CLinkedList<T>::~CLinkedList() 
{
	(*RefCounter)--;
	if ( (*RefCounter) == 0 ) {
		CLinkedListElt<T>* theElt;
		theElt = RootElt->GetNext();
		while ( theElt != RootElt ) {
			CLinkedListElt<T>* tt=theElt;
			theElt = theElt->GetNext();
			delete tt;
		} 
		delete RootElt;
		delete RefCounter;
	}
}

template<class T> inline CLinkedList<T> CLinkedList<T>::operator=(const CLinkedList<T>& l )
{
	(*(l.RefCounter))++;
	RefCounter = l.RefCounter;
	RootElt = l.RootElt;
	return *this;
}

// Adds a new element to the linked list by putting it immediately after
//    the element "afterElt"
template<class T> inline CLinkedListElt<T>* 
			CLinkedList<T>::AddAfter( T* newT, CLinkedListElt<T>* afterElt)
{
	CLinkedListElt<T> *newElt = new CLinkedListElt<T>;
	newElt->Tptr = newT;
	newElt->PrevElt = afterElt;
	newElt->NextElt = afterElt->NextElt;
	afterElt->NextElt->PrevElt = newElt;
	afterElt->NextElt = newElt;
	return newElt;
}

// Adds a new element to the linked list by putting it immediately after
//    the current element.
template<class T> inline void
			CLinkedListElt<T>::AddAfter( T* newT ) 
{
	CLinkedList<T>::AddAfter( newT, this );
}


// Remove a linked list element from the linked list.
//	Frees (delete's) the linked list element, but does not delete the object
//	pointed to by the linked list element.
template<class T> inline void
			CLinkedListElt<T>::Remove( ) 
{
	CLinkedList<T>::Remove ( this );
}

// Remove a linked list element from the linked list.
//	Frees (delete's) the linked list element, but does not delete the object
//	pointed to by the linked list element.
template<class T> inline CLinkedListElt<T>* 
				CLinkedList<T>::Remove( CLinkedListElt<T>* CLLelt ) 
{
	CLinkedListElt<T>* ret;
	if ( CLLelt->NextElt == CLLelt ) {
		ret = NULL;
	}
	else {
		ret = CLLelt->NextElt;
	}
	delete CLLelt;
	return ret;
}

// Remove a linked list element from the linked list.
//	Frees (delete's) the linked list element, AND deletes the object
//	pointed to by the linked list element.
template<class T> inline void
			CLinkedListElt<T>::DeleteRemove( ) 
{
	CLinkedList<T>::DeleteRemove ( this );
}

// Remove a linked list element from the linked list.
//	Frees (delete's) the linked list element, AND deletes the object
//	pointed to by the linked list element.
template<class T> inline CLinkedListElt<T>* 
				CLinkedList<T>::DeleteRemove( CLinkedListElt<T>* CLLelt ) 
{
	CLinkedListElt<T>* ret;
	if ( CLLelt->NextElt == CLLelt ) {
		ret = NULL;
	}
	else {
		ret = CLLelt->NextElt;
	}
	delete CLLelt->Tptr;
	delete CLLelt;
	return ret;
}

template<class T> inline int 
	CLinkedList<T>::GetNumElts() const 
{
	int count=0;
	CLinkedListElt<T>* e;
	for ( e=GetFirst(); !e->IsRoot(); e=e->GetNext() ) {
		count++;
	}
	return count;
}
#endif // VR_DATA_H

