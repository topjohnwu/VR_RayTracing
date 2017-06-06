/*
 * Extents.h
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

#ifndef EXTENTS_H
#define EXTENTS_H

class VectorR3;
class Parallelepiped;

class ViewableParallelogram;
class ViewableParallelepiped;
class ViewableSphere;
class ViewableTriangle;

#include "../VrMath/LinearR3.h"

// This is a file for collecting routines that find bounding box extents
//  of ViewableBase objects intersected with bounding boxes.  
// So far, implemented for only:
//		ViewableParallelogram,
//		ViewableParallelopiped,
//		ViewableSphere,
//		ViewableTriangle.
// If and when the routines are implemented for enough kinds of ViewableBase
//	objects, its functionality might be moved into the ViewableBase objects'
//	classes.

// **********************************************************************
// CalcExtentsInBox. Consider the intersection of the ViewableBase
//		with the bounding box defined by boundBoxMax/Min.
//		This intersection is inside the bounding box given by
//		the values extentsMin/Max.
//	The purpose of this routine is to compute extentsMin and extentsMax.
//  Returns true if the extents are nonempty.
//  If returns false, the extentsMin/Max values are not set.
// **********************************************************************
bool CalcExtentsInBox( const ViewableParallelogram& parallelogram,
						   const VectorR3& boxBoundMin, const VectorR3& boxBoundMax,
						   VectorR3* extentsMin, VectorR3* extentsMax );

bool CalcExtentsInBox( const ViewableParallelepiped& ppiped,
						   const VectorR3& boxBoundMin, const VectorR3& boxBoundMax,
						   VectorR3* extentsMin, VectorR3* extentsMax );

bool CalcExtentsInBox( const ViewableSphere& sphere,
						   const VectorR3& boxBoundMin, const VectorR3& boxBoundMax,
						   VectorR3* extentsMin, VectorR3* extentsMax );

bool CalcExtentsInBox( const ViewableTriangle& tri,
						   const VectorR3& boxBoundMin, const VectorR3& boxBoundMax,
						   VectorR3* extentsMin, VectorR3* extentsMax );

// **********************************************************************
// CalcSolidExtentsInBox. Consider the intersection of a solid geometric 
//		object with the bounding box defined by boundBoxMax/Min.
//		This intersection is inside the bounding box given by
//		the values extentsMin/Max.
//	The purpose of this routine is to compute extentsMin and extentsMax.
//  Returns true if the extents are nonempty.
//  If returns false, the extentsMin/Max values are not set.
// **********************************************************************

bool CalcExtentsInBox( const Parallelepiped& ppiped,
						   const VectorR3& boxBoundMin, const VectorR3& boxBoundMax,
						   VectorR3* extentsMin, VectorR3* extentsMax );


// CalcBoundingBox intended for internal use. 
//	Finds the bounding box of a set of points.
bool CalcBoundingBox( int numPoints, const VectorR3* vertArray,
						   VectorR3* extentsMin, VectorR3* extentsMax );

// Functions below are helper functions, intended for internal use.

// CalcMinMaxSquares
//  Input values: valMin and valMax where valMin <= valMax
//  Returns the values  Min{ x*x :  x \in [valMin, valMax] }
//                 and  Max{ x*x :  x \in [valMin, valMax] }
void CalcMinMaxSquares( double valMin, double valMax, double* valSqMin, double* valSqMax );
bool CalcExtentsHelpForSphere( double boxMin, double boxMax, 
							   double radiusSq, double otherSqMin, double otherSqMax,
							   double* inExtent, double* maxExtent );

inline void CEIB_AddOrSubtract( VectorR3* u, VectorR3* v,
						 const VectorR3& addTerm, bool addOrSubtract )
{
	if ( addOrSubtract ) {
		*v += addTerm;
	}
	else {
		*u -= addTerm;
	}
}

inline bool InAABB( const VectorR3& pt, const VectorR3& boxBoundMin, const VectorR3& boxBoundMax )
{
	return (   boxBoundMin.x <= pt.x && pt.x <= boxBoundMax.x 
			&& boxBoundMin.y <= pt.y && pt.y <= boxBoundMax.y
			&& boxBoundMin.z <= pt.z && pt.z <= boxBoundMax.z );
}

#endif // EXTENTS_H
