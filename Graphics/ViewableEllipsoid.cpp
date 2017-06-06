/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006.
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

#include "ViewableEllipsoid.h"
#include "ViewableSphere.h"
#include "../VrMath/PolynomialRC.h"

// Returns an intersection if found with distance maxDistance
// viewDir must be a unit vector.
// intersectDistance and visPoint are returned values.
bool ViewableEllipsoid::FindIntersectionNT ( 
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double *intersectDistance, VisiblePoint& returnedPoint ) const
{
	VectorR3 v = viewPos;
	v -= Center;
	double pdotuA = v^AxisA;
	double pdotuB = v^AxisB;
	double pdotuC = v^AxisC;
	double udotuA = viewDir^AxisA;
	double udotuB = viewDir^AxisB;
	double udotuC = viewDir^AxisC;

	double C = Square(pdotuA) + Square(pdotuB) + Square(pdotuC) - 1.0;
	double B = ( pdotuA*udotuA + pdotuB*udotuB + pdotuC*udotuC );
	if ( C>0.0 && B>=0.0 ) {
		return false;			// Pointing away from the ellipsoid
	}

	B += B;		// Double B to get final factor of 2.
	double A = Square(udotuA) + Square(udotuB) + Square(udotuC);

	double alpha1, alpha2;
	int numRoots = QuadraticSolveRealSafe( A, B, C, &alpha1, &alpha2 );
	if ( numRoots==0 ) {
		return false;
	}
	if ( alpha1>0.0 ) {
		if ( alpha1>=maxDistance ) {
			return false;				// Too far away
		}
		// Found an intersection from outside.
		returnedPoint.SetFrontFace();
		returnedPoint.SetMaterial( *OuterMaterial );
		*intersectDistance = alpha1;
	}
	else if ( numRoots==2 && alpha2>0.0 && alpha2<maxDistance ) {
		// Found an intersection from inside.
		returnedPoint.SetBackFace();
		returnedPoint.SetMaterial( *InnerMaterial );
		*intersectDistance = alpha2;
	}
	else {
		return false;	// Both intersections behind us (should never get here)
	}

	// Calculate intersection position
	v=viewDir;
	v *= (*intersectDistance);
	v += viewPos;
	returnedPoint.SetPosition( v );	// Intersection Position

	v -= Center;	// Now v is the relative position
	double vdotuA = v^AxisA;		
	double vdotuB = v^AxisB;
	double vdotuC = v^AxisC;
	v = vdotuA*AxisA + vdotuB*AxisB + vdotuC*AxisC;
	v.Normalize();
	returnedPoint.SetNormal( v );

	// Calculate u-v coordinates
	ViewableSphere::CalcUV( vdotuB, vdotuC, vdotuA, uvProjectionType,
							&returnedPoint.GetUV() );
	returnedPoint.SetFaceNumber( 0 );
	return true;
}

void ViewableEllipsoid::CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const
{
	double centerDot = (u^Center);
	double deltaDot = sqrt(Square(RadiusA*RadiusA*(u^AxisA))
							+Square(RadiusB*RadiusB*(u^AxisB))
							+Square(RadiusC*RadiusC*(u^AxisC)));
	*maxDot = centerDot + deltaDot;
	*minDot = centerDot - deltaDot;
}


bool ViewableEllipsoid::CalcPartials( const VisiblePoint& visPoint, 
								  VectorR3& retPartialU, VectorR3& retPartialV ) const
{
	double theta = PI2*(visPoint.GetU()-0.5);	// Range [-pi,pi]
	double phi = PI*(visPoint.GetV()-0.5);		// Range [-pi/2,pi/2]
	double sinphi = sin(phi);
	double cosphi = cos(phi);
	double sintheta = sin(theta);
	double costheta = cos(theta);
	double radiusASq = Square(RadiusA);
	double radiusBSq = Square(RadiusB);
	retPartialV = AxisA;
	retPartialV *= -sinphi*costheta*radiusASq;
	VectorR3 temp = AxisB;
	temp *= -sinphi*sintheta*radiusBSq;
	retPartialV += temp;
	temp = AxisC;
	temp *= cosphi;
	retPartialV += temp*RadiusC*RadiusC;

	retPartialU = AxisA;
	retPartialU *= -cosphi*sintheta*radiusASq;
	temp = AxisB;
	temp *= cosphi*costheta*radiusBSq;
	retPartialU += temp;

	if ( uvProjectionType==1 ) {	// If cylindrical projection
		double denom = sqrt(1-Square(2.0*(visPoint.GetV()-0.5)));
		if ( denom==0.0 ) {
			return false;
		}
		retPartialV /= denom;
	}

	return (cosphi!=0.0);
}
