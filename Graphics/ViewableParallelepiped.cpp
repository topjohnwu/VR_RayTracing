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

#include "ViewableParallelepiped.h"
#include "Extents.h"
#include "../VrMath/Aabb.h"

void ViewableParallelepiped::CalcPlaneInfo()
{
	VectorR3 EdgeAB = VertexB;
	EdgeAB -= VertexA;
	VectorR3 EdgeAC = VertexC;
	EdgeAC -= VertexA;
	VectorR3 EdgeAD = VertexD;
	EdgeAD -= VertexA;

	// Front and back faces
	NormalABC = EdgeAB;
	NormalABC *= EdgeAC;
	NormalABC.Normalize();
	TopCoefABC = (NormalABC)^VertexA;		// Front face coef.
	BottomCoefABC = (NormalABC)^VertexD;	// Back face coef.
	if ( TopCoefABC<BottomCoefABC ) {
		double temp = TopCoefABC;
		TopCoefABC = BottomCoefABC;
		BottomCoefABC = temp;
	}

	// Top and bottom faces
	NormalABD = EdgeAB;
	NormalABD *= EdgeAD;
	NormalABD.Normalize();
	TopCoefABD = (NormalABD)^VertexC;		// Top face coef.
	BottomCoefABD = (NormalABD)^VertexA;	// Bottom face coef.
	if ( TopCoefABD<BottomCoefABD ) {
		double temp = TopCoefABD;
		TopCoefABD = BottomCoefABD;
		BottomCoefABD = temp;
	}

	// Left and right faces
	NormalACD = EdgeAD;
	NormalACD *= EdgeAC;
	NormalACD.Normalize();
	TopCoefACD = (NormalACD)^VertexB;		// Right face coef.
	BottomCoefACD = (NormalACD)^VertexA;	// Left face coef.
	if ( TopCoefACD<BottomCoefACD ) {
		double temp = TopCoefACD;
		TopCoefACD = BottomCoefACD;
		BottomCoefACD = temp;
	}

}


bool DoTwoPlanes( const VectorR3& viewPos, const VectorR3& viewDir, 
				  const VectorR3 normal, double topCoef, double bottomCoef, int planeNum,
				  double *maxFront, int *frontNum, double *minBack, int *backNum,
				  double maxHitDistanceAllowed)
{
	double pdotn = (viewPos^normal);
	double udotn = (viewDir^normal);

	if ( udotn > 0.0 ) {
		if ( pdotn>topCoef ) {
			return false;		// Above top and pointing up
		}
		if ( bottomCoef-pdotn>=0.0 ) {
			double newFront = (bottomCoef-pdotn)/udotn;
			if ( newFront > *maxFront ) {
				if ( newFront > *minBack || newFront > maxHitDistanceAllowed ) {
					return false;
				}
				*maxFront = newFront;		// Hits bottom from below (from outside)
				*frontNum = planeNum+1;		// +1 for bottom plane
			}
		}
		double newBack = (topCoef-pdotn)/udotn;
		if ( newBack < *minBack ) {
			if ( newBack < *maxFront ) {
				return false;
			}
			*minBack = newBack;					// Hits top from inside
			*backNum = planeNum;
		}
	}
	else if ( udotn < 0.0 ) {
		if ( pdotn<bottomCoef ) {
			return false;		// Below bottom and pointing down
		}
		if ( topCoef-pdotn<=0.0 ) {
			double newFront = (topCoef-pdotn)/udotn;
			if ( newFront > *maxFront ) {
				if ( newFront > *minBack || newFront > maxHitDistanceAllowed ) {
					return false;
				}
				*maxFront = newFront;		// Hits top from above (from outside)
				*frontNum = planeNum;
			}
		}
		double newBack = (bottomCoef-pdotn)/udotn;
		if ( newBack < *minBack ) {
			if ( newBack < *maxFront ) {
				return false;
			}
			*minBack = newBack;					// Hits top from inside
			*backNum = planeNum+1;				// +1 for bottom plane
		}
	}
	else if ( pdotn<bottomCoef || pdotn>topCoef ) {
		return false;
	}
	return true;
}

bool DoTwoPlanesCheck(
				  const VectorR3& viewPos, const VectorR3& viewDir, 
				  const VectorR3 normal, double topCoef, double bottomCoef, int planeNum,
				  double *maxFront, int *frontNum, double *minBack, int *backNum,
				  double maxHitDistanceAllowed)
{
	if ( bottomCoef<topCoef ) {
		return DoTwoPlanes( viewPos, viewDir, normal, topCoef, bottomCoef, planeNum,
							maxFront, frontNum, minBack, backNum, maxHitDistanceAllowed);
	}

	double pdotn = (viewPos^normal);
	double udotn = (viewDir^normal);
	double hitDist = (topCoef-pdotn)/udotn;
	if ( hitDist<0.0 || hitDist>maxHitDistanceAllowed
				|| hitDist<*maxFront || hitDist>*minBack ) {
		return false;
	}
	*maxFront = hitDist;
	*minBack = hitDist;
	return true;
}

// Returns an intersection if found with distance maxDistance
// viewDir must be a unit vector.
// intersectDistance and visPoint are returned values.
bool ViewableParallelepiped::FindIntersectionNT ( 
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double *intersectDistance, VisiblePoint& returnedPoint ) const
{
	double maxFrontDist = -DBL_MAX;
	int frontFaceNum;
	double minBackDist = DBL_MAX;
	int backFaceNum;
	// Face nums: 0,1,2,3,4,5 = front, back, top, bottom, right, left.

	if (!DoTwoPlanes( viewPos, viewDir, NormalABC, TopCoefABC, BottomCoefABC, 0,
				&maxFrontDist, &frontFaceNum, &minBackDist, &backFaceNum, maxDistance) ) {
		return false;
	}
	if (!DoTwoPlanes( viewPos, viewDir, NormalABD, TopCoefABD, BottomCoefABD, 2,
				&maxFrontDist, &frontFaceNum, &minBackDist, &backFaceNum, maxDistance) ) {
		return false;
	}
	if (!DoTwoPlanes( viewPos, viewDir, NormalACD, TopCoefACD, BottomCoefACD,4,
				&maxFrontDist, &frontFaceNum, &minBackDist, &backFaceNum, maxDistance) ) {
		return false;
	}

	double alpha;
	int faceNum;
	if ( maxFrontDist>0.0 ) {
		alpha = maxFrontDist;
		faceNum = frontFaceNum;
		returnedPoint.SetFrontFace();
		returnedPoint.SetMaterial(*OuterMaterial);
	}
	else if ( minBackDist>0.0 && minBackDist<maxDistance ) {
		alpha = minBackDist;
		faceNum = backFaceNum;
		returnedPoint.SetBackFace();
		returnedPoint.SetMaterial(*InnerMaterial);
	}
	else {
		return false;
	}

	VectorR3 v = viewDir;
	v *= alpha;
	v += viewPos;		// Intersection point
	*intersectDistance = alpha;
	returnedPoint.SetPosition( v );

	double uCoord, vCoord;
	switch ( faceNum>>1 ) {

	case 0:	
		// Front or back face (ABC)
		uCoord = ((v^NormalACD)-BottomCoefACD)/(TopCoefACD-BottomCoefACD);
		vCoord = ((v^NormalABD)-BottomCoefABD)/(TopCoefABD-BottomCoefABD);
		v = NormalABC;
		if ( faceNum&0x01 ) {
			uCoord = 1.0-uCoord;
			v.Negate();
		}
		break;

	case 1:
		// Top or bottom face (ABD)
		uCoord = ((v^NormalACD)-BottomCoefACD)/(TopCoefACD-BottomCoefACD);
		vCoord = (TopCoefABC-(v^NormalABC))/(TopCoefABC-BottomCoefABC);
		v = NormalABD;
		if ( faceNum&0x01 ) {
			vCoord = 1.0-vCoord;
			v.Negate();
		}
		break;

	case 2:
		// Left or right face (ACD)
		uCoord = (TopCoefABC-(v^NormalABC))/(TopCoefABC-BottomCoefABC);
		vCoord =  ((v^NormalABD)-BottomCoefABD)/(TopCoefABD-BottomCoefABD);
		v = NormalACD;
		if ( faceNum&0x01 ) {
			uCoord = 1.0-uCoord;
			v.Negate();
		}
		break;
	}
	returnedPoint.SetUV(uCoord,vCoord);
	returnedPoint.SetFaceNumber( faceNum );
	returnedPoint.SetNormal( v );
	
	return true;
}

bool ViewableParallelepiped::CalcPartials( const VisiblePoint& visPoint, 
									 VectorR3& retPartialU, VectorR3& retPartialV ) const
{
	int faceNum = visPoint.GetFaceNumber();
	switch ( faceNum>>1 ) {
	case 0:
		// Front or back face (ABC)
		retPartialU = VertexB;
		retPartialU -= VertexA;
		retPartialV = VertexC;
		retPartialV -= VertexA;
		if ( faceNum&0x01 ) {
			retPartialU.Negate();
		}
		break;
	case 1:
		// Top or bottom face
		retPartialU = VertexB;
		retPartialU -= VertexA;
		retPartialV = VertexD;
		retPartialV -= VertexA;
		if ( faceNum&0x01 ) {
			retPartialV.Negate();
		}
		break;
	case 2:
		// Left or right face (ABD)
		retPartialU = VertexD;
		retPartialU -= VertexA;
		retPartialV = VertexC;
		retPartialV -= VertexA;
		if ( faceNum&0x01 ) {
			retPartialU.Negate();
		}
		break;
	}
	if ( faceNum & 0x01 ) {
		retPartialU.Negate();
	}

	return true;			// Not a singularity point (parallelpipeds should not be degenerate)
}

void ViewableParallelepiped::CalcBoundingPlanes( const VectorR3& u, 
												 double *minDot, double *maxDot ) const
{
	double startdot = (u^VertexA);
	double mind = startdot;
	double maxd = mind;
	double t;
	t = (u^VertexB)-startdot;
	if ( t<0 ) {
		mind += t;
	}
	else {
		maxd +=t;
	}
	t = (u^VertexC)-startdot;
	if ( t<0 ) {
		mind += t;
	}
	else {
		maxd += t;
	}
	t = (u^VertexD)-startdot;
	if ( t<0 ) {
		mind += t;
	}
	else {
		maxd += t;
	}
	*minDot = mind;
	*maxDot = maxd;		
}

bool ViewableParallelepiped::CalcExtentsInBox( const AABB& boundingAABB, AABB& retAABB ) const
{
	return( ::CalcExtentsInBox( *this, boundingAABB.GetBoxMin(), boundingAABB.GetBoxMax(),
						   &(retAABB.GetBoxMin()), &(retAABB.GetBoxMax()) ) );
}

bool ViewableParallelepiped::QuickIntersectTest(
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double *intersectDistanceIn, double *intersectDistanceOut,
		const VectorR3& NormalA, double MinDotA, double MaxDotA, 
		const VectorR3& NormalB, double MinDotB, double MaxDotB, 
		const VectorR3& NormalC, double MinDotC, double MaxDotC ) 
{
	double maxFrontDist = -DBL_MAX;
	int frontFaceNum;
	double minBackDist = DBL_MAX;
	int backFaceNum;
	// Face nums: 0,1,2,3,4,5 = A-top, A-bottom, B-top, B-bottom, C-top, C-bottom.

	if (!DoTwoPlanesCheck( viewPos, viewDir, NormalA, MaxDotA, MinDotA, 0,
				&maxFrontDist, &frontFaceNum, &minBackDist, &backFaceNum, maxDistance) ) {
		return false;
	}
	if (!DoTwoPlanesCheck( viewPos, viewDir, NormalB, MaxDotB, MinDotB, 2,
				&maxFrontDist, &frontFaceNum, &minBackDist, &backFaceNum, maxDistance) ) {
		return false;
	}
	if (!DoTwoPlanesCheck( viewPos, viewDir, NormalC, MaxDotC, MinDotC, 4,
				&maxFrontDist, &frontFaceNum, &minBackDist, &backFaceNum, maxDistance) ) {
		return false;
	}

	*intersectDistanceIn = maxFrontDist;
	*intersectDistanceOut = minBackDist;
	assert( minBackDist>=0.0 );
	return true;
}

bool ViewableParallelepiped::QuickIntersectTest(
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double *intersectDistance,
		const VectorR3& NormalA, double MinDotA, double MaxDotA, 
		const VectorR3& NormalB, double MinDotB, double MaxDotB, 
		const VectorR3& NormalC, double MinDotC, double MaxDotC ) 
{
	double temp;
	return QuickIntersectTest( viewPos, viewDir, maxDistance,
							   intersectDistance, &temp,
							   NormalA, MinDotA, MaxDotA,
							   NormalB, MinDotB, MaxDotB,
							   NormalC, MinDotC, MaxDotC );
}
