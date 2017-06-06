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

// Aabb.h
//
// Axis Aligned Bounding Box (AABB)
//
// Implements a fairly simple minded data structure for
//	holding information about an AABB.
//
// Author: Sam Buss.
// Contact: sbuss@math.ucsd.edu
// All rights reserved.  May be used for any purpose as long
//	as use is acknowledged.

#ifndef AABB_H
#define AABB_H

#include <assert.h>
#include "LinearR3.h"

class AABB {
public:

	AABB() {}
	AABB( const VectorR3& boxMin, const VectorR3& boxMax );
	AABB( const AABB& aabb );

	void Set( const VectorR3& boxMin, const VectorR3& boxMax );

	void SetNewAxisMin ( int axisNum, double newMin );	// Use 0, 1, 2, for x, y, z axes
	void SetNewAxisMax ( int axisNum, double newMax );

	const VectorR3& GetBoxMin() const { return BoxMin; }
	const VectorR3& GetBoxMax() const { return BoxMax; }

	VectorR3& GetBoxMin() { return BoxMin; }
	VectorR3& GetBoxMax() { return BoxMax; }
	
	double GetMinX() const { return BoxMin.x; }
	double GetMaxX() const { return BoxMax.x; }
	double GetMinY() const { return BoxMin.y; }
	double GetMaxY() const { return BoxMax.y; }
	double GetMinZ() const { return BoxMin.z; }
	double GetMaxZ() const { return BoxMax.z; }

	bool IsFlatX() const { return (BoxMin.x==BoxMax.x); }
	bool IsFlatY() const { return (BoxMin.y==BoxMax.y); }
	bool IsFlatZ() const { return (BoxMin.z==BoxMax.z); }

	bool WellFormed() const;	// If has non-negative volume
	bool IsEmpty() const;		// If has negative volume (definitely misformed in this case)

	// Update the Aabb to include the "newAabb"
	void EnlargeToEnclose ( const AABB& newAabb );

	// Miscellaneous functions
	double SurfaceArea() const;

	// Intersection functions
	// Form the intersection of two AABB's. 
	// "this" is updating by intersecting it with aabb1.
	// Use IsEmpty to check if result has is non-empty.
	void IntersectAgainst( const AABB& aabb1 );

	// Find intersection points with a ray.
	bool RayEntryExit( const VectorR3& startPos, const VectorR3& dir,
					   double *entryDist, int *entryFaceId,
					   double *exitDist, int *exitFaceId );
	bool RayEntryExit( const VectorR3& startPos, 
					   int signDirX, int signDirY, int signDirZ, const VectorR3& dirInv,
					   double *entryDist, int *entryFaceId,
					   double *exitDist, int *exitFaceId );

private:
	VectorR3 BoxMin;		// Lower corner (min value for all three coordinates)
	VectorR3 BoxMax;		// Upper corner (max value for all three coordinates)
};

inline AABB::AABB( const VectorR3& boxMin, const VectorR3& boxMax )
: BoxMin(boxMin), BoxMax(boxMax)
{
	assert( WellFormed() );
}

inline AABB::AABB( const AABB& aabb )
: BoxMin(aabb.BoxMin), BoxMax(aabb.BoxMax )
{
	assert( WellFormed() );
}

inline void AABB::Set( const VectorR3& boxMin, const VectorR3& boxMax )
{
	BoxMin = boxMin;
	BoxMax = boxMax;
	assert( WellFormed() );

}

// Use 0, 1, 2, for x, y, z axes
inline void AABB::SetNewAxisMin ( int axisNum, double newMin )
{
	switch (axisNum) {
	case 0:
		BoxMin.x = newMin;
		break;
	case 1:
		BoxMin.y = newMin;
		break;
	case 2:
		BoxMin.z = newMin;
		break;
	}
}

// Use 0, 1, 2, for x, y, z axes
inline void AABB::SetNewAxisMax ( int axisNum, double newMax )
{
	switch (axisNum) {
	case 0:
		BoxMax.x = newMax;
		break;
	case 1:
		BoxMax.y = newMax;
		break;
	case 2:
		BoxMax.z = newMax;
		break;
	}
}

// Form the intersection of two AABB's. Result is aabb2.
// Use IsEmpty to check if result has is non-empty.
inline void AABB::IntersectAgainst( const AABB& aabb1 )
{
	UpdateMax( aabb1.BoxMin.x, BoxMin.x );
	UpdateMax( aabb1.BoxMin.y, BoxMin.y );
	UpdateMax( aabb1.BoxMin.z, BoxMin.z );
	UpdateMin( aabb1.BoxMax.x, BoxMax.x );
	UpdateMin( aabb1.BoxMax.y, BoxMax.y );
	UpdateMin( aabb1.BoxMax.z, BoxMax.z );
}

inline bool AABB::WellFormed() const {
	return ( BoxMin.x<=BoxMax.x 
			 && BoxMin.y<=BoxMax.y 
			 && BoxMin.z<=BoxMax.z);
}

// Flat boxes do not count as "empty"
inline bool AABB::IsEmpty() const {
	return ( BoxMax.x<BoxMin.x 
				|| BoxMax.y<BoxMin.y 
				|| BoxMax.z<BoxMin.z );
}

#endif
