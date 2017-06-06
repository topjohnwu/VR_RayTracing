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

#include "ViewableSphere.h"
#include "Extents.h"
#include "../VrMath/Aabb.h"
#include "../VrMath/MathMisc.h"

// Returns an intersection if found with distance maxDistance
// viewDir must be a unit vector.
// intersectDistance and visPoint are returned values.
bool ViewableSphere::FindIntersectionNT ( 
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDist,
		double *intersectDistance, VisiblePoint& returnedPoint ) const 
{
	VectorR3 tocenter(Center);
	tocenter -= viewPos;		// Vector view position to the center

	// D = Distance to pt on view line closest to sphere
	// v = vector from sphere center to the closest pt on view line
	// ASq = the distance from v to sphere center squared
	double D = (viewDir^tocenter);
	VectorR3 v(viewDir);
	v *= D;
	v -= tocenter;
	double ASq = v.NormSq();

	// Ray-line completely misses sphere, or just grazes it.
	if ( ASq >= RadiusSq ) {
		return false;
	}

	double BSq = RadiusSq-ASq;
	if ( D>0.0 && D*D>BSq && 
		(D<maxDist || BSq>Square(D-maxDist) ) ) {
		
		// Return the point where view intersects with the outside of
		//		the sphere.
		*intersectDistance = D-sqrt(BSq);
		v = viewDir;
		v *= *intersectDistance;
		v += viewPos;					//  Position of intersection
		returnedPoint.SetPosition( v );
		v -= Center;					
		v /= Radius;	// Normalize: normal out from intersection pt
		v.ReNormalize();
		returnedPoint.SetNormal( v );
		returnedPoint.SetMaterial ( *OuterMaterial );
		returnedPoint.SetFrontFace();	// Front face direction
		CalcUV( v, &(returnedPoint.GetUV()) );
		returnedPoint.SetFaceNumber( 0 );
		return true;
	}

	else if ( (D>0.0 || D*D<BSq) && D<maxDist && BSq<Square(D-maxDist) ) {

		// return the point where view exits the sphere
		*intersectDistance = D+sqrt(BSq);
		v = viewDir;
		v *= *intersectDistance;
		v += viewPos;
		returnedPoint.SetPosition( v );
		v -= Center;
		v /= Radius;	// Normalize, but still points outward
		v.ReNormalize(); // Just in case
		returnedPoint.SetNormal( v );
		returnedPoint.SetMaterial ( *InnerMaterial );
		returnedPoint.SetBackFace();
		CalcUV( v, &(returnedPoint.GetUV()) );
		return true;
	}

	else {
		return false;
	}
}

bool ViewableSphere::QuickIntersectTest( 
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDist,
		double *intersectDistance,
		const VectorR3& centerPos, double radiusSq )
{
	VectorR3 tocenter(centerPos);
	tocenter -= viewPos;		// Vector view position to the center

	// D = Distance to pt on view line closest to sphere
	// v = vector from sphere center to the closest pt on view line
	// ASq = the distance from v to sphere center squared
	double D = (viewDir^tocenter);
	VectorR3 v(viewDir);
	v *= D;
	v -= tocenter;
	double ASq = v.NormSq();

	// Ray-line completely misses sphere, or just grazes it.
	if ( ASq >= radiusSq ) {
		return false;
	}

	double BSq = radiusSq-ASq;
	if ( D>0.0 && D*D>BSq && (D<maxDist || BSq>Square(D-maxDist) ) ) {
		// It hits the sphere as it enters.
		*intersectDistance = D-sqrt(BSq);
		return true;
	}

	else if ( (D>0.0 || D*D<BSq) && D<maxDist && BSq<Square(D-maxDist) ) {
		// it hits the sphere as it exits
		*intersectDistance = D+sqrt(BSq);
		return true;
	}

	else {
		return false;
	}
}


void ViewableSphere::CalcUV( const VectorR3& posVec, VectorR2* returnedUV ) const
{
	double z = posVec^AxisA;
	double y = posVec^AxisC;
	double x = posVec^AxisB;

	CalcUV( x, y, z, uvProjectionType, returnedUV );
}
	
void ViewableSphere::CalcUV( double x, double y, double z, int uvprojectiontype,
							 VectorR2* returnedUV )
{

	double u = atan2(x,z)*PI2inv + 0.5;
	double v;

	switch ( uvprojectiontype ) {
		
	case 0:		// Spherical projection
		if ( y>=1.0 ) {
			v = 1.0;				// avoid roundoff error exceptions
		}
		else if ( y<=-1.0 ) {
			v = 0.0;				// and again
		}
		else {
			v = 1.0-acos(y)*PIinv;	// We usually do this. 
		}
		break;

	case 1:		// Cylindrical projection

		v = (y+1.0)*0.5;

		break;
	}

	returnedUV->Set(u, v);
}

void ViewableSphere::CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const
{
	double cd = (u^Center);
	*maxDot = cd+Radius;
	*minDot = cd-Radius;
}

bool ViewableSphere::CalcPartials( const VisiblePoint& visPoint, 
								   VectorR3& retPartialU, VectorR3& retPartialV ) const
{
	retPartialV = visPoint.GetPosition();
	retPartialV -= Center;
	retPartialU = retPartialV;
	retPartialU *= AxisC;			// Magnitude = R sin(phi).
	retPartialU *= -PI2;			// Adjust for [0,1] domain instead of [-pi,pi]

	retPartialV *= retPartialU;		// Pointing in right direction, magnitude 2 pi R^2 sin(phi)
									// Sign and magnitude adjusted below.
	double badMagSq = retPartialV.NormSq();
	double h;

	switch ( uvProjectionType ) {

	case 0:	// Spherical projection
		h = sqrt(badMagSq);
		if ( h==0.0 ) {
			retPartialV = AxisB;
			return false;
		}
		retPartialV *= (PI*Radius/h);  // Convert to domain [0,1] instead of [-pi/2,pi/2].
											  // Compensate for sign and R^2 magnitude.
		break;

	case 1:
		h = 2.0*(visPoint.GetV()-0.5);	// In range [-1,1]
		h = sqrt(badMagSq*(1.0 - h*h));			// Derivative of arcsin(h) = 1/sqrt(1-h^2)
		if ( h==0 ) {
			retPartialV = AxisB;
			return false;
		}
		retPartialV *= PI*Radius/h;			// Adjust sign and magnitude
		break;
		
	}
	return true;
}

bool ViewableSphere::CalcExtentsInBox( const AABB& boundingAABB, AABB& retAABB ) const
{
	return( ::CalcExtentsInBox( *this, boundingAABB.GetBoxMin(), boundingAABB.GetBoxMax(),
						   &(retAABB.GetBoxMin()), &(retAABB.GetBoxMax()) ) );
}
