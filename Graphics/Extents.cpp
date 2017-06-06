/*
 * Extents.cpp
 *
 * RayTrace Software Package, release 3.2.  May 3, 2007
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

#include "Extents.h"
#include "ViewableParallelogram.h"
#include "ViewableParallelepiped.h"
#include "ViewableSphere.h"
#include "ViewableTriangle.h"
#include "../VrMath/Parallelepiped.h"
#include "../VrMath/PolygonClip.h"
#include "../VrMath/MathMisc.h"

// This is a file for collecting routines that find bounding box extents
//  of ViewableBase objects intersected with bounding boxes.  
// So far, implemented for only:
//		ViewableParallelogram,
//		ViewableParallelepiped
//		ViewableSphere
//		ViewableTriangle.
// If and when the routines are implemented for enough kinds of ViewableBase
//	objects, its functionality will be moved into the ViewableBase objects'
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

static VectorR3 VertArray[60];


bool CalcExtentsInBox( const ViewableParallelogram& parallelogram,
						   const VectorR3& boxBoundMin, const VectorR3& boxBoundMax,
						   VectorR3* extentsMin, VectorR3* extentsMax )
{
	VertArray[0] = parallelogram.GetVertexA();
	VertArray[1] = parallelogram.GetVertexB();
	VertArray[2] = parallelogram.GetVertexC();
	VertArray[3] = parallelogram.GetVertexD();

	int numClippedVerts = ClipConvexPolygonAgainstBoundingBox( 
														4, VertArray, parallelogram.GetNormal(),
														boxBoundMin, boxBoundMax );
	if ( numClippedVerts == 0 ) {
		return false;
	}
	CalcBoundingBox( numClippedVerts, VertArray, extentsMin, extentsMax );

	// Next six lines to avoid roundoff errors putting extents outside the bounding box
	ClampRange( &extentsMin->x, boxBoundMin.x, boxBoundMax.x );
	ClampRange( &extentsMin->y, boxBoundMin.y, boxBoundMax.y );
	ClampRange( &extentsMin->z, boxBoundMin.z, boxBoundMax.z );
	ClampRange( &extentsMax->x, boxBoundMin.x, boxBoundMax.x );
	ClampRange( &extentsMax->y, boxBoundMin.y, boxBoundMax.y );
	ClampRange( &extentsMax->z, boxBoundMin.z, boxBoundMax.z );
	return true;
}

bool CalcExtentsInBox( const ViewableParallelepiped& ppiped,
						   const VectorR3& boxBoundMin, const VectorR3& boxBoundMax,
						   VectorR3* extentsMin, VectorR3* extentsMax )
{
	VectorR3 deltaAB = ppiped.GetVertexB();
	VectorR3 deltaAC = ppiped.GetVertexC();
	VectorR3 deltaAD = ppiped.GetVertexD();
	const VectorR3& vertexA = ppiped.GetVertexA();
	deltaAB -= vertexA;
	deltaAC -= vertexA;
	deltaAD -= vertexA;

	int baseCount = 0;

	// Front face
	VertArray[baseCount+0] = ppiped.GetVertexA();
	VertArray[baseCount+1] = ppiped.GetVertexB();
	VertArray[baseCount+2] = ppiped.GetVertexC() + deltaAB;
	VertArray[baseCount+3] = ppiped.GetVertexC();
	baseCount += ClipConvexPolygonAgainstBoundingBox( 4, &(VertArray[baseCount]), ppiped.GetNormalABC(),
														boxBoundMin, boxBoundMax );
	// Back face
	VertArray[baseCount+0] = ppiped.GetVertexA() + deltaAD;
	VertArray[baseCount+1] = ppiped.GetVertexB() + deltaAD;
	VertArray[baseCount+2] = ppiped.GetVertexC() + deltaAB + deltaAD;
	VertArray[baseCount+3] = ppiped.GetVertexC() + deltaAD;
	baseCount += ClipConvexPolygonAgainstBoundingBox( 4, &(VertArray[baseCount]), ppiped.GetNormalABC(),
														boxBoundMin, boxBoundMax );
	// Left face
	VertArray[baseCount+0] = ppiped.GetVertexA();
	VertArray[baseCount+1] = ppiped.GetVertexC();
	VertArray[baseCount+2] = ppiped.GetVertexD() + deltaAC;
	VertArray[baseCount+3] = ppiped.GetVertexD();
	baseCount += ClipConvexPolygonAgainstBoundingBox( 4, &(VertArray[baseCount]), ppiped.GetNormalACD(),
														boxBoundMin, boxBoundMax );
	// Right face
	VertArray[baseCount+0] = ppiped.GetVertexA() + deltaAB;
	VertArray[baseCount+1] = ppiped.GetVertexC() + deltaAB;
	VertArray[baseCount+2] = ppiped.GetVertexD() + deltaAC + deltaAB;
	VertArray[baseCount+3] = ppiped.GetVertexD() + deltaAB;
	baseCount += ClipConvexPolygonAgainstBoundingBox( 4, &(VertArray[baseCount]), ppiped.GetNormalACD(),
														boxBoundMin, boxBoundMax );
	// Bottom face
	VertArray[baseCount+0] = ppiped.GetVertexA();
	VertArray[baseCount+1] = ppiped.GetVertexB();
	VertArray[baseCount+2] = ppiped.GetVertexD() + deltaAB;
	VertArray[baseCount+3] = ppiped.GetVertexD();
	baseCount += ClipConvexPolygonAgainstBoundingBox( 4, &(VertArray[baseCount]), ppiped.GetNormalABD(),
														boxBoundMin, boxBoundMax );
	// Top face
	VertArray[baseCount+0] = ppiped.GetVertexA() + deltaAC;
	VertArray[baseCount+1] = ppiped.GetVertexB() + deltaAC;
	VertArray[baseCount+2] = ppiped.GetVertexD() + deltaAB + deltaAC;
	VertArray[baseCount+3] = ppiped.GetVertexD() + deltaAC;
	baseCount += ClipConvexPolygonAgainstBoundingBox( 4, &(VertArray[baseCount]), ppiped.GetNormalABD(),
														boxBoundMin, boxBoundMax );

	int numClippedVerts = baseCount;
	if ( numClippedVerts == 0 ) {
		return false;
	}
	CalcBoundingBox( numClippedVerts, VertArray, extentsMin, extentsMax );

	// Next six lines to avoid roundoff errors putting extents outside the bounding box
	ClampRange( &extentsMin->x, boxBoundMin.x, boxBoundMax.x );
	ClampRange( &extentsMin->y, boxBoundMin.y, boxBoundMax.y );
	ClampRange( &extentsMin->z, boxBoundMin.z, boxBoundMax.z );
	ClampRange( &extentsMax->x, boxBoundMin.x, boxBoundMax.x );
	ClampRange( &extentsMax->y, boxBoundMin.y, boxBoundMax.y );
	ClampRange( &extentsMax->z, boxBoundMin.z, boxBoundMax.z );
	return true;
}

bool CalcExtentsInBox( const Parallelepiped& ppiped,
						   const VectorR3& boxBoundMin, const VectorR3& boxBoundMax,
						   VectorR3* extentsMin, VectorR3* extentsMax )
{
	// Step 1
	// Intersect the six faces of the parallelepiped against the bounding box.
	int faceIsectCount = 0;			// Number of faces that intersect the bounding box
	int baseCount = 0;				// Number of verts found in the count
	int vertsLeft;

	VectorR3 faceNormal;

	faceNormal = ppiped.GetNormalFront();
	ppiped.GetFrontFace( &VertArray[baseCount] );
	vertsLeft = ClipConvexPolygonAgainstBoundingBox( 4, &(VertArray[baseCount]), faceNormal,
														boxBoundMin, boxBoundMax );
	if ( vertsLeft>0 ) {
		baseCount += vertsLeft;
		faceIsectCount++;
	}
	ppiped.GetBackFace( &VertArray[baseCount] );
	vertsLeft = ClipConvexPolygonAgainstBoundingBox( 4, &(VertArray[baseCount]), faceNormal,
														boxBoundMin, boxBoundMax );
	if ( vertsLeft>0 ) {
		baseCount += vertsLeft;
		faceIsectCount++;
	}
	faceNormal = ppiped.GetNormalLeft();
	ppiped.GetRightFace( &VertArray[baseCount] );
	vertsLeft = ClipConvexPolygonAgainstBoundingBox( 4, &(VertArray[baseCount]), faceNormal,
														boxBoundMin, boxBoundMax );
	if ( vertsLeft>0 ) {
		baseCount += vertsLeft;
		faceIsectCount++;
	}
	ppiped.GetLeftFace( &VertArray[baseCount] );
	vertsLeft = ClipConvexPolygonAgainstBoundingBox( 4, &(VertArray[baseCount]), faceNormal,
														boxBoundMin, boxBoundMax );
	if ( vertsLeft>0 ) {
		baseCount += vertsLeft;
		faceIsectCount++;
	}
	faceNormal = ppiped.GetNormalBottom();
	ppiped.GetBottomFace( &VertArray[baseCount] );
	vertsLeft = ClipConvexPolygonAgainstBoundingBox( 4, &(VertArray[baseCount]), faceNormal,
														boxBoundMin, boxBoundMax );
	if ( vertsLeft>0 ) {
		baseCount += vertsLeft;
		faceIsectCount++;
	}
	ppiped.GetTopFace( &VertArray[baseCount] );
	vertsLeft = ClipConvexPolygonAgainstBoundingBox( 4, &(VertArray[baseCount]), faceNormal,
														boxBoundMin, boxBoundMax );
	if ( vertsLeft>0 ) {
		baseCount += vertsLeft;
		faceIsectCount++;
	}
	if ( baseCount>0 ) {
		CalcBoundingBox( baseCount, VertArray, extentsMin, extentsMax );
	}
	
	// Step 2
	// For each of x, y, z; find the min and max valued vertex.  Check if inside.
	bool twoEndPtsIn = false;
	int numIn;
	VectorR3 min, max;
	min = ppiped.GetBasePt();
	max = min;
	CEIB_AddOrSubtract( &min, &max, ppiped.GetEdgeA(), ppiped.GetEdgeA().x>0.0 );
	CEIB_AddOrSubtract( &min, &max, ppiped.GetEdgeB(), ppiped.GetEdgeB().x>0.0 );
	CEIB_AddOrSubtract( &min, &max, ppiped.GetEdgeC(), ppiped.GetEdgeC().x>0.0 );
	numIn = 0;
	if ( InAABB( min, boxBoundMin, boxBoundMax ) ) {
		VertArray[baseCount++] = min;
		numIn ++;
	}
	if ( InAABB( max, boxBoundMin, boxBoundMax ) ) {
		VertArray[baseCount++] = max;
		numIn ++;
	}
	twoEndPtsIn = (numIn==2);
	min = ppiped.GetBasePt();
	max = min;
	CEIB_AddOrSubtract( &min, &max, ppiped.GetEdgeA(), ppiped.GetEdgeA().y>0.0 );
	CEIB_AddOrSubtract( &min, &max, ppiped.GetEdgeB(), ppiped.GetEdgeB().y>0.0 );
	CEIB_AddOrSubtract( &min, &max, ppiped.GetEdgeC(), ppiped.GetEdgeC().y>0.0 );
	numIn = 0;
	if ( InAABB( min, boxBoundMin, boxBoundMax ) ) {
		VertArray[baseCount++] = min;
		numIn ++;
	}
	if ( InAABB( max, boxBoundMin, boxBoundMax ) ) {
		VertArray[baseCount++] = max;
		numIn ++;
	}
	if ( numIn==2 ){
		twoEndPtsIn = true;
	}
	min = ppiped.GetBasePt();
	max = min;
	CEIB_AddOrSubtract( &min, &max, ppiped.GetEdgeA(), ppiped.GetEdgeA().z>0.0 );
	CEIB_AddOrSubtract( &min, &max, ppiped.GetEdgeB(), ppiped.GetEdgeB().z>0.0 );
	CEIB_AddOrSubtract( &min, &max, ppiped.GetEdgeC(), ppiped.GetEdgeC().z>0.0 );
	numIn = 0;
	if ( InAABB( min, boxBoundMin, boxBoundMax ) ) {
		VertArray[baseCount++] = min;
		numIn ++;
	}
	if ( InAABB( max, boxBoundMin, boxBoundMax ) ) {
		VertArray[baseCount++] = max;
		numIn ++;
	}
	if ( numIn==2 ){
		twoEndPtsIn = true;
	}

	// Step 3
	// Find min/max extents from the list of vertices
	if ( !twoEndPtsIn && faceIsectCount<2 ) {
		return false;
	}
	CalcBoundingBox( baseCount, VertArray, extentsMin, extentsMax );

	// Next six lines to avoid roundoff errors putting extents outside the bounding box
	ClampRange( &extentsMin->x, boxBoundMin.x, boxBoundMax.x );
	ClampRange( &extentsMin->y, boxBoundMin.y, boxBoundMax.y );
	ClampRange( &extentsMin->z, boxBoundMin.z, boxBoundMax.z );
	ClampRange( &extentsMax->x, boxBoundMin.x, boxBoundMax.x );
	ClampRange( &extentsMax->y, boxBoundMin.y, boxBoundMax.y );
	ClampRange( &extentsMax->z, boxBoundMin.z, boxBoundMax.z );
	return true;
}



// **********************************************************
// CalcExtentsInBox:  ViewableSphere
//    Zero area intersections are ignored, e.g. where the sphere is
//		tangent to the face of a cube or intersects a corner.
// **********************************************************
bool CalcExtentsInBox( const ViewableSphere& sphere,
						   const VectorR3& boxBoundMin, const VectorR3& boxBoundMax,
						   VectorR3* extentsMin, VectorR3* extentsMax )
{
	const VectorR3& center = sphere.GetCenter();

	// Get min and max relative to center of sphere
	VectorR3 min = boxBoundMin;
	min -= center;
	VectorR3 max = boxBoundMax;
	max -= center;

	double xSqMin, xSqMax;
	double ySqMin, ySqMax;
	double zSqMin, zSqMax;
	CalcMinMaxSquares( min.x, max.x, &xSqMin, &xSqMax );
	CalcMinMaxSquares( min.y, max.y, &ySqMin, &ySqMax );
	CalcMinMaxSquares( min.z, max.z, &zSqMin, &zSqMax );

	double radiusSq = Square(sphere.GetRadius());		// Square of the radius
	if ( !CalcExtentsHelpForSphere( min.x, max.x, radiusSq, ySqMin+zSqMin, ySqMax+zSqMax,
					&(extentsMin->x), &(extentsMax->x) ) ) {
			return false;
	}
	if ( !CalcExtentsHelpForSphere( min.y, max.y, radiusSq, xSqMin+zSqMin, xSqMax+zSqMax,
					&(extentsMin->y), &(extentsMax->y) ) ) {
			return false;
	}
	if ( !CalcExtentsHelpForSphere( min.z, max.z, radiusSq, xSqMin+ySqMin, xSqMax+ySqMax,
					&(extentsMin->z), &(extentsMax->z) ) ) {
			return false;
	}

	*extentsMin += center;
	*extentsMax += center;
	return true;
}

// CalcMinMaxSquares
//  Input values: valMin and valMax where valMin <= valMax
//  Returns the values  Min{ x*x :  x \in [valMin, valMax] }
//                 and  Max{ x*x :  x \in [valMin, valMax] }
void CalcMinMaxSquares( double valMin, double valMax, double* valSqMin, double* valSqMax )
{
	assert ( valMin<=valMax );
	if ( valMin<0.0 && valMax>0.0 ) {
		*valSqMin = 0.0;
		*valSqMax = ( valMax > -valMin ) ? Square(valMax) : Square(valMin);
	}
	else {
		*valSqMin = Square(valMin);
		*valSqMax = Square(valMax);
		if ( (*valSqMin)>(*valSqMax) ) {
			double temp = *valSqMin;
			*valSqMin = *valSqMax;
			*valSqMax = temp;
		}
	}
}

// CalcExtentsHelpForSphere
//    Helper routine for CalcExtentsInBox( ViewableSphere& ...)
//		Returns false if extent in box is empty.
inline bool CalcExtentsHelpForSphere( double boxMin, double boxMax, 
							   double radiusSq, double otherSqMin, double otherSqMax,
							   double* minExtent, double* maxExtent )
{
	double maxSq = radiusSq - otherSqMin;
	double maxPos;						// Max x value (always positive)
	if ( maxSq<0.0 ) {
		return false;
	}
	else {
		maxPos = sqrt(maxSq);
	}
	double minSq = radiusSq - otherSqMax;
	double minPos = (minSq > 0.0) ? sqrt(minSq) : 0.0;

	// minPos and maxPos are the min/max possible positive values
	//	I.e., permissible values are from [-maxPos,-minPos]\cup [minPos,maxPos].
	//	This now needs to be intersected with [boxMin, boxMax]
	if ( maxPos<=boxMin || (-maxPos)>=boxMax ) {
		return false;		// Test uses <= to ignore single points of intersectio
	}
	if ( boxMin<(-minPos) ) {
		*minExtent = Max(boxMin,-maxPos);
		if ( boxMax>minPos ) {
			*maxExtent = Min(boxMax,maxPos);
		}
		else {
			*maxExtent = Min(boxMax,-minPos);
		}
		return true;
	}
	else if ( boxMax>minPos ) {
		*minExtent = Max(boxMin,minPos);
		*maxExtent = Min(boxMax,maxPos);
		return true;
	}
	else {
		return false;
	}
}

// **********************************************************
// CalcExtentsInBox:  ViewableTriangle
// **********************************************************
bool CalcExtentsInBox( const ViewableTriangle& tri,
						   const VectorR3& boxBoundMin, const VectorR3& boxBoundMax,
						   VectorR3* extentsMin, VectorR3* extentsMax )
{
	VertArray[0] = tri.GetVertexA();
	VertArray[1] = tri.GetVertexB();
	VertArray[2] = tri.GetVertexC();

	int numClippedVerts = ClipConvexPolygonAgainstBoundingBox( 3, VertArray, tri.GetNormal(),
														boxBoundMin, boxBoundMax );
	if ( numClippedVerts == 0 ) {
		return false;
	}
	CalcBoundingBox( numClippedVerts, VertArray, extentsMin, extentsMax );

	// Next six lines to avoid roundoff errors putting extents outside the bounding box
	ClampRange( &extentsMin->x, boxBoundMin.x, boxBoundMax.x );
	ClampRange( &extentsMin->y, boxBoundMin.y, boxBoundMax.y );
	ClampRange( &extentsMin->z, boxBoundMin.z, boxBoundMax.z );
	ClampRange( &extentsMax->x, boxBoundMin.x, boxBoundMax.x );
	ClampRange( &extentsMax->y, boxBoundMin.y, boxBoundMax.y );
	ClampRange( &extentsMax->z, boxBoundMin.z, boxBoundMax.z );
	return true;
}


// Find the bounding box of a set of points.
bool CalcBoundingBox( int numPoints, const VectorR3* vertArray,
						   VectorR3* extentsMin, VectorR3* extentsMax )
{
	if ( numPoints==0 ) {
		return false;
	}
	const VectorR3* vertPtr = vertArray;
	*extentsMin = *vertPtr;
	*extentsMax = *vertPtr;
	for ( int i=numPoints-1; i>0; i-- ) {
		vertPtr++;
		UpdateMinMax( vertPtr->x, extentsMin->x, extentsMax->x ); 
		UpdateMinMax( vertPtr->y, extentsMin->y, extentsMax->y ); 
		UpdateMinMax( vertPtr->z, extentsMin->z, extentsMax->z ); 
	}
	return true;
}
