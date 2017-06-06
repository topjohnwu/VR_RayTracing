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

// PolygonClip.cpp
//  Author: Sam Buss, October 2004.

#include "PolygonClip.h"
#include "LinearR3.h"

// Routine for clipping a convex polygon against a plane.
// The edge and verts are clipped so as to lie in or below the plane.
// Returns true if clipping has occured and updates the numVerts value
// Always returns the number of new verts  as numNewVerts (0, 1, or 2)
//     except if entire polygon clipped in which case numNewVerts = -1.
// numVerts gives the number of vertices in the vertex array vertArray
// It is *always* assume that vertArray[] has enough room to
//	add a new vertex if necessary.  Space must have been preallocated before
//	the routine is called.
// The planarity assumption is not used.  The important thing is that the
//	line loop of edge vectors can cross (or touch) the plane at most once.
bool ClipConvexPolygonAgainstPlane( int *numVerts, VectorR3 vertArray[],
								    const VectorR3& PlaneNormal,
								    double PlaneConstant,
								    int *numNewVerts)
{
	// Check for verts that are in or out to decide which verts
	//	to clip away.
	VectorR3 entryPoint;		// The point where the polygon reenters the plane
	VectorR3 exitPoint;		// The point where the polygon leaves the plane
	int enterIndex = -1;	// The index of the first vert inside the plane
	int exitIndex = -1;		// The index of the first vert outside the plane
	int enterExact;			// Equals zero if vertArray[enterVert] lies exactly on the plane
	int exitExact;			// Equals zero if vertArray[exitVert-1] lies exactly on the plane

	int N = *numVerts;
	VectorR3* prevVertPtr = &(vertArray[N-1]);
	double lastAboveAmt = ((*prevVertPtr)^PlaneNormal)-PlaneConstant;
	bool lastOutFlag = (lastAboveAmt>0.0);
	double prevAboveAmt = lastAboveAmt;
	bool prevOutFlag = lastOutFlag;
	VectorR3* thisVertPtr = vertArray;			// pointer to a vert
	int i;
	// Loop over all edges
	for ( i=0; ; i++ ) {
		double thisAboveAmt;
		bool thisOutFlag;
		if ( i<N-1 ) {
			// Get position of next vert relative to plane
			thisAboveAmt = ((*thisVertPtr)^PlaneNormal)-PlaneConstant;
			thisOutFlag = (thisAboveAmt>0.0);
		}
		else if ( i==N-1 ) {
			// Have reached last edge (and last vert)
			thisAboveAmt = lastAboveAmt;
			thisOutFlag = lastOutFlag;
		}
		else {
			break;		// Break from for loop.  Have processed all edges
		}
		if ( prevOutFlag!=thisOutFlag ) {
			// Verts both on opposite sides of plane
			if ( prevOutFlag ) {
				// Previous vert above plane, current vert on or below plane
				enterIndex = i;
				if ( thisAboveAmt==0.0 ) {
					// This vertex is exactly on the plane
					enterExact = 0;			// Zero new vertices to add because of the entry
				}
				else {
					// This vertex is properly below the plane
					enterExact = 1;			// One new vert to add
					LerpDelta( *thisVertPtr, *prevVertPtr, thisAboveAmt/(thisAboveAmt-prevAboveAmt), entryPoint );
				}
			}
			else {
				// This vert is above plane, previous was on or below the plane
				exitIndex = i;
				if ( prevAboveAmt==0.0 ) {
					// Previous vertex was exactly on the plane
					exitExact = 0;		// Zero new vertices to add as exit.
				}
				else {
					// Previous vertex was below the plane.
					exitExact = 1;			// One new vert to add as exits.
					LerpDelta( *prevVertPtr, *thisVertPtr, prevAboveAmt/(prevAboveAmt-thisAboveAmt), exitPoint );
				}
			}
		}
		prevAboveAmt = thisAboveAmt;
		prevOutFlag = thisOutFlag;
		prevVertPtr = thisVertPtr;
		thisVertPtr++;
	}
	
	// Handle the cases with of either no clipping or total clipping
	assert ( (enterIndex==-1 && exitIndex==-1) || (enterIndex>=0 && exitIndex>=0) ); // programming bug
	if ( enterIndex==-1 ) {
		// Did not cross the plane!
		if ( lastOutFlag ) {
			// Polygon is entirely outside the plane
			*numVerts = 0;			// No verts left at all!
			*numNewVerts = -1;
			return true;
		}
		else {
			// Polygon is entirely below the plane
			*numNewVerts = 0;		// No change to the polygon
			return false;
		}
	}

	RemoveIntervalFromListCircular( vertArray, *numVerts,
									exitIndex, enterIndex, 
									exitExact, exitPoint, enterExact, entryPoint );
	int numNew = enterExact+exitExact;		// Number of new verts added
	int numLost = enterIndex-exitIndex;
	if ( numLost<0 ) {
		numLost += *numVerts;
	}
	*numNewVerts = numNew;
	*numVerts += numNew - numLost;
	return true;
}

//*******************************************************************
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
// Special handling: If the slab intersection with the polygon has area zero,
//	   then the entire polygon is clipped away.  In particular, for zero thickness
//		slabs, the polygon is clipped away unless the polygon lies inside the slab plane.
//	   And polygons that are touch only the surface of the slab are entirely
//		clipped away.
//****************************************************************
bool ClipConvexPolygonAgainstSlab( int *numVerts, VectorR3 vertArray[],
								   const VectorR3& PlaneNormal,
								   double planeConstantMin, double planeConstantMax )
{

	assert ( planeConstantMin <= planeConstantMax );
	bool isZeroThickness = ( planeConstantMin==planeConstantMax );

	// Check for verts that are in or out to decide which verts
	//	to clip away.
	VectorR3 crossingPt[4];	// Points where the polygon leaves/enters the slab
	int crossingIndex[4];	// Indices where the polygon leaves/enters the slab
	int crossingExact[4];	// ==0 if crossing involved point exactly on the slab surface
							// ==1 if the crossing created a new vertex
	bool crossingType[4];	// true if the polygon *entered* the slab at this point.
	int crossingCount = 0;

	int N = *numVerts;
	VectorR3* prevVertPtr = &(vertArray[N-1]);
	double lastDot = ((*prevVertPtr)^PlaneNormal);
	int lastOutCode = (lastDot>planeConstantMax) ? 4 :
						((lastDot==planeConstantMax) ? 3 :
						((lastDot>planeConstantMin) ? 2 :
						((lastDot==planeConstantMin) ? 1 : 0 )));
	bool foundVertOutside = false;
	bool foundNonRightside = false;
	bool foundNonLeftside = false;

	double prevDot = lastDot;
	int prevOutCode = lastOutCode;
	VectorR3* thisVertPtr = vertArray;			// pointer to a vert
	int i;
	// Loop over all edges
	for ( i=0; ; i++ ) {
		double thisDot;
		int thisOutCode;
		if ( i<N-1 ) {
			// Get position of next vert relative to slab
			thisDot = ((*thisVertPtr)^PlaneNormal);
			thisOutCode = (thisDot>planeConstantMax) ? 4 :
							((thisDot==planeConstantMax) ? 3 :
							((thisDot>planeConstantMin) ? 2 :
							((thisDot==planeConstantMin) ? 1 : 0 )));
		}
		else if ( i==N-1 ) {
			// Have reached last edge (and last vert)
			thisDot = lastDot;
			thisOutCode = lastOutCode;
		}
		else {
			break;		// Break from for loop.  Have processed all edges
		}
		if ( thisOutCode==4 || thisOutCode==0 ) {
			foundVertOutside = true;
		}
		if ( thisOutCode<=2 ) {
			foundNonRightside = true;
		}
		if ( thisOutCode>=2 ) {
			foundNonLeftside = true;
		}
		if ( prevOutCode!=thisOutCode ) {
			int prevOC, thisOC;
			double inDot, outDot;
			if ( prevOutCode<thisOutCode ) {		// Transitioning from less to greater dot value
				prevOC = prevOutCode;
				thisOC = thisOutCode;
				inDot = planeConstantMin;
				outDot = planeConstantMax;
			}
			else {
				prevOC = 4-prevOutCode;
				thisOC = 4-thisOutCode;
				inDot = planeConstantMax;
				outDot = planeConstantMin;
			}
			switch ( prevOC ) {
				case 0:
					assert ( crossingCount < 4 );
					crossingIndex[crossingCount] = i;
					crossingType[crossingCount] = true;
					crossingExact[crossingCount] = (thisOC==1) ? 0 : 1;
					if ( crossingExact[crossingCount]==1 ) {
						LerpDelta( *thisVertPtr, *prevVertPtr, (thisDot-inDot)/(thisDot-prevDot), crossingPt[crossingCount] );
					}
					crossingCount++;
				case 1:
				case 2:
					if ( thisOC<4 ) {
						break;
					}
				case 3:
					assert ( crossingCount < 4 );
					crossingIndex[crossingCount] = i;
					crossingType[crossingCount] = false;
					crossingExact[crossingCount] = (prevOC==3) ? 0 : 1;
					if ( crossingExact[crossingCount]==1 ) {
						LerpDelta( *thisVertPtr, *prevVertPtr, (thisDot-outDot)/(thisDot-prevDot), crossingPt[crossingCount] );
					}
					crossingCount++;
			}
		}
		prevDot = thisDot;
		prevOutCode = thisOutCode;
		prevVertPtr = thisVertPtr;
		thisVertPtr++;
	}

	assert( crossingCount==0 || crossingCount==2 || crossingCount==4 );
	if ( crossingCount==0 ) {
		// No crossings at all.  Either keep all or discard all.
		if ( foundVertOutside ) {
			*numVerts = 0;			// No verts left at all!
			return true;
		}
		else {
			// Polygon is entirely inside the plane - No change
			return false;
		}
	}
	if ( (isZeroThickness && foundVertOutside) || (!foundNonLeftside) || (!foundNonRightside) ) {
		*numVerts = 0;
		return true;								// Return nothing at all for zero thickness.
	}

	if ( crossingType[1]==true ) {			// If last crossing is entering
		if ( crossingCount==4 ) {
			RemoveIntervalFromListCircular( vertArray, *numVerts,
											crossingIndex[2], crossingIndex[3], 
											crossingExact[2], crossingPt[2], 
											crossingExact[3], crossingPt[3] );
			int numNew = crossingExact[2]+crossingExact[3];		// Number of new verts added
			int numLost = crossingIndex[3]-crossingIndex[2];
			(*numVerts) -= (numLost-numNew);
		}
		RemoveIntervalFromListCircular( vertArray, *numVerts,
										crossingIndex[0], crossingIndex[1], 
										crossingExact[0], crossingPt[0], 
										crossingExact[1], crossingPt[1] );
		int numNew = crossingExact[0]+crossingExact[1];		// Number of new verts added
		int numLost = crossingIndex[1]-crossingIndex[0];
		(*numVerts) -= (numLost-numNew);
	}
	else {					// Else, if last crossing is exiting
		int exitI = 1;
		if ( crossingCount==4 ) {
			RemoveIntervalFromListCircular( vertArray, *numVerts,
											crossingIndex[1], crossingIndex[2], 
											crossingExact[1], crossingPt[1], 
											crossingExact[2], crossingPt[2] );
			int numNew = crossingExact[1]+crossingExact[2];		// Number of new verts added
			int numLost = crossingIndex[2]-crossingIndex[1];
			(*numVerts) -= (numLost-numNew);
			crossingIndex[3] -= (numLost-numNew);				// Adjust for removals and insertions
			exitI = 3;
		}
		RemoveIntervalFromListCircular( vertArray, *numVerts,
										crossingIndex[exitI], crossingIndex[0], 
										crossingExact[exitI], crossingPt[exitI], 
										crossingExact[0], crossingPt[0] );
		int numNew = crossingExact[exitI]+crossingExact[0];		// Number of new verts added
		int numLost = (*numVerts)+crossingIndex[0]-crossingIndex[exitI];
		(*numVerts) -= (numLost-numNew);
	}

	return true;
}



// ************************************************************
// Clip the convex polygon against a bounding box.
// Returns the new number of vertices (possibly zero)
// Returns non-zero only if the intersection of the polygon with the box has
//		non-zero area.  Flat boxes are permitted.
// The version that takes planeNormal uses this as the normal to
//		the plane containing the vertices --- gives more stable
//		results in near-degenerate cases.
// ************************************************************

int ClipConvexPolygonAgainstBoundingBox( int numVerts, VectorR3 vertArray[], const VectorR3& planeNormal,
									 const VectorR3& boxMin, const VectorR3& boxMax )
{
	if ( numVerts<=2 ) {
		return 0;
	}

	// Find index of greatest component of planeNormal
	double nXabs = fabs(planeNormal.x);
	double nYabs = fabs(planeNormal.y);
	double nZabs = fabs(planeNormal.z);
	int greatestN = nXabs>nYabs ? (nXabs>nZabs ? 0 : 2) : (nYabs>nZabs ? 1 : 2);

	// Do the axis corresponding to greatestN last.
	VectorR3 dirVec;
	for ( int j=2; j>=0; j-- ) {
		switch ( (greatestN+j)%3 )
		{
		case 0:
			dirVec.SetUnitX();
			ClipConvexPolygonAgainstSlab( &numVerts, vertArray, dirVec, boxMin.x, boxMax.x );
			break;
		case 1:
			dirVec.SetUnitY();
			ClipConvexPolygonAgainstSlab( &numVerts, vertArray, dirVec, boxMin.y, boxMax.y );
			break;
		case 2:
			dirVec.SetUnitZ();
			ClipConvexPolygonAgainstSlab( &numVerts, vertArray, dirVec, boxMin.z, boxMax.z );
			break;
		}
		if ( numVerts<=2 ) {
			return 0;
		}
	}

	return numVerts;

}

int ClipConvexPolygonAgainstBoundingBox( int numVerts, VectorR3 vertArray[],
									 const VectorR3& boxMin, const VectorR3& boxMax )
{
	// x extents
	if ( numVerts<=2 ) {
		return 0;
	}
	VectorR3 dirVec(1.0, 0.0, 0.0);
	ClipConvexPolygonAgainstSlab( &numVerts, vertArray, dirVec, boxMin.x, boxMax.x );

	// y extents
	if ( numVerts<=2 ) {
		return 0;
	}
	dirVec.SetUnitY();
	ClipConvexPolygonAgainstSlab( &numVerts, vertArray, dirVec, boxMin.y, boxMax.y );

	// z extents
	if ( numVerts<=2 ) {
		return 0;
	}
	dirVec.SetUnitZ();
	ClipConvexPolygonAgainstSlab( &numVerts, vertArray, dirVec, boxMin.z, boxMax.z );

	return numVerts;
}



