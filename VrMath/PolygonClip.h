/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006
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

// PolygonClip.h
//  Author: Sam Buss, October 2004.

#ifndef POLYGON_CLIP_H
#define POLYGON_CLIP_H

#include <assert.h>

class VectorR3;

// Routine for clipping a convex polygon against a plane
// Returns true if clipping has occured and updates the numVerts value
// numVerts gives the number of vertices in the vertex array vertArray
// It is *always* assume that vertArray[] has enough room to
//	add a new vertex if necessary.  Space must have been preallocated before
//	the routine is called.
// The planarity assumption is not used in any essential way.  
//    The important thing is that the line loop of edge vectors can 
//    cross (or touch) the plane at most once.
bool ClipConvexPolygonAgainstPlane( int *numVerts, VectorR3 vertArray[],
								   const VectorR3& PlaneNormal,
								   double PlaneConstant );

// This version is similar, but also returns the number of new verts (0, 1, or 2)
//      except if entire polygon clipped in which case numNewVerts = -1.
bool ClipConvexPolygonAgainstPlane( int *numVerts, VectorR3 vertArray[],
								   const VectorR3& PlaneNormal,
								   double PlaneConstant,
								   int *numNewVerts);

// Routine for clipping a convex polygon against a slab (an infinite
//		region bounded by two parallel planes.
// Returns true if clipping has occured and updates the numVerts value
// numVerts gives the number of vertices in the vertex array vertArray
// It is *always* assume that vertArray[] has enough room to
//	add a new vertex if necessary.  Space must have been preallocated before
//	the routine is called.
// The planarity assumption is not used in any essential way.  
//    The important thing is that the line loop of edge vectors can 
//    enter and exit the slab more than twice.
// Special handling: If the slab intersects with the polygon has area zero,
//	   then the entire polygon is clipped away.  In particular, for zero thickness
//		slab, the polygon is clipped away unless the polygon lies inside the slab plane.
//	   And polygons that are touch only the surface of the slab are entirely
//		clipped away.
bool ClipConvexPolygonAgainstSlab( int *numVerts, VectorR3 vertArray[],
								   const VectorR3& PlaneNormal,
								   double planeConstantMin, double planeConstantMax );


// Clip the convex polygon against a bounding box.
// Returns the new number of vertices (possibly zero)
// Only returns vertices if the polygon's intersection
//		with the box has non-zero area.
// The version that takes planeNormal uses this as the normal to
//		the plane containing the vertices --- gives more stable
//		results in near-degenerate cases.

int ClipConvexPolygonAgainstBoundingBox( int numVerts, VectorR3 vertArray[], const VectorR3& planeNormal,
									 const VectorR3& boxMin, const VectorR3& boxMax );

int ClipConvexPolygonAgainstBoundingBox( int numVerts, VectorR3 vertArray[],
									 const VectorR3& boxMin, const VectorR3& boxMax );

// **********************************************************
// Inlined functions
// **********************************************************

inline bool ClipConvexPolygonAgainstPlane(
					  int *numVerts, VectorR3 vertArray[],
					  const VectorR3& PlaneNormal, double PlaneConstant )
{
	int numNewVerts;
	return ClipConvexPolygonAgainstPlane( numVerts, vertArray, PlaneNormal, PlaneConstant,
										   &numNewVerts );
}

// RemoveIntervalFromListCircular:
//     Removes all entries from list[i] to list[j-1], where the
//		list is circular.  i = firstRemove,  j = firstKeep
//	   It is permitted for firstRemove to equal firstKeep: in this
//		case, no entries are discarded.
//     Optionally adds up to two new entries.  If entries are added,
//     it is assumed there is enough room for them in the list.
//	   Returns the index of the first added item if any, or, if no item added,
//		returns the index of the first kept item.
//     The "addAtFirst" item (if exists) is added at the point where
//		the removal of items starts.
//     The "addAtLast" item (if exists) is added at the point where
//		the removal of items ends.  That is "addAtFirst" is added before
//		"addAtLast".
template <class T>
long RemoveIntervalFromListCircular( T list[], long listLen, long firstRemove, long firstKeep,
									 int addAtFirstFlag, const T& addAtFirst,
									 int addAtLastFlag, const T& addAtLast );



// *************************************************************
// RemoveIntervalFromListCircular 
//     Removes all entries from list[i] to list[j-1], where the
//		list is circular.   i = firstRemove,  j = firstKeep
//	   It is permitted for firstRemove to equal firstKeep: in this
//		case, no entries are discarded.
//     Optionally adds up to two new entries.  If entries are added,
//     it is assumed there is enough room for them in the list.
//	   Returns the index of the first added item, or, if no item added,
//		returns the item after the last deleted item.
//     The "addAtFirst" item (if exists) is added at the point where
//		the removal of items starts.
//     The "addAtLast" item (if exists) is added at the point where
//		the removal of items ends.  That is "addAtFirst" is added before
//		"addAtLast".
// Has to be inlined since uses templates
// *************************************************************
template <class T>
long RemoveIntervalFromListCircular( T list[], long listLen, long firstRemove, long firstKeep,
									 int addAtFirstFlag, const T& addAtFirst,
									 int addAtLastFlag, const T& addAtLast )
{	
	assert (addAtFirstFlag==0 || addAtFirstFlag==1);
	assert (addAtLastFlag==0 || addAtLastFlag==1);

	int numNew = addAtFirstFlag+addAtLastFlag;		// Number of new verts to add
	int numLost = firstKeep-firstRemove;

	if ( numLost<0 ) {			// If the last entry in the list is among the removed
		listLen = firstRemove;	// Automatically discard ends
		firstRemove = 0;		
		numLost = firstKeep;
	}
    		
	long newItemIndex;	// Index to put the "new" items.
	T* newItemPtr;		// Position to put the "new" items.

	if ( firstKeep==0 ) {
		newItemIndex = listLen;
		newItemPtr = list+newItemIndex;		// Just put them at the end
	}
	else {
		newItemIndex = firstRemove;
		newItemPtr = list+newItemIndex;
		int slideAmt = numNew-(firstKeep-firstRemove);
		if ( slideAmt>0 ) {
			// Need to slide up
			T* fromPtr = list + (listLen-1);
			T* toPtr = fromPtr + slideAmt;
			for ( long i=listLen-firstKeep; i>0; i-- ) {
				*(toPtr--) = *(fromPtr--);
			}
		}
		else if ( slideAmt<0 ) {
			// Need to slide down
			T* toPtr = newItemPtr + numNew;
			T* fromPtr = list + firstKeep;
			for ( long i=listLen-firstKeep; i>0; i-- ) {
				*(toPtr++) = *(fromPtr++);
			}
		}
	}

	// Add 0, 1, or 2 new verts for exiting/entering
	if ( addAtFirstFlag!=0 ) {
		*(newItemPtr++) = addAtFirst;
	}
	if ( addAtLastFlag!=0 ) {
		*(newItemPtr++) = addAtLast;
	}

	return newItemIndex%(listLen+addAtLastFlag+addAtFirstFlag);
}




#endif		// POLYGON_CLIP_H