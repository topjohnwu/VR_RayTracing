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

#include "ViewableTorus.h"
#include "../VrMath/PolynomialRC.h"

void ViewableTorus::PreCalcInfo()
{
	CenterCoefA = ( Center^AxisA );
	CenterCoefB = ( Center^AxisB );
	CenterCoefC = ( Center^AxisC );		// CenterAxis
	OuterRadius = MajorRadius+MinorRadius;
}

// Helper routine for FindIntersectionNT.
inline bool ViewableTorus::CollideTwoPlanes( 
						double pdotn, double alpha, double dimen, 
						bool *insideFlag, double* minDistBack, double *maxDistFront)
{
	if ( alpha>0.0 ) {
		if ( pdotn>=dimen ) {
			return false;			// Above top & pointing up
		}
		double temp = dimen+pdotn;
		if ( temp<0.0 ) {
			*insideFlag = false;
			// Handle bottom from below 
			if ( alpha*(*maxDistFront) < -temp ){
				*maxDistFront = -temp/alpha;
			}
			temp = (dimen-pdotn)/alpha;				// Dist thru to top
			if ( temp<*minDistBack ) {
				*minDistBack = (dimen-pdotn)/alpha;	// 2nd intersect w/ top
			}
			if ( *maxDistFront>*minDistBack ) {
				return false;
			}
		}
		else {
			// Handle top from inside
			temp = dimen-pdotn;
			if ( alpha*(*minDistBack)>temp ) {
				*minDistBack = temp/alpha;
				if ( *maxDistFront>*minDistBack ) {
					return false;
				}
			}
		}
	}
	else if ( alpha<0.0 ) {
		if ( pdotn <= -dimen ) {
			return false;			// Below bottom and pointing down
		}
		double temp = pdotn-dimen;
		if ( temp>0.0 ) {
			*insideFlag = false;
			// Handle top from above
			if ( -alpha*(*maxDistFront) < temp ) {
				*maxDistFront = -temp/alpha;
			}
			temp = -(pdotn+dimen)/alpha;	// Dist. thru to bottom
			if ( temp<*minDistBack ) {
				*minDistBack = temp;		// 2nd intersect w/ bottom
			}
			if ( *maxDistFront>*minDistBack ) {
				return false;
			}
		}
		else {
			// Handle bottom from inside
			temp = pdotn+dimen;
			if ( -alpha*(*minDistBack) > temp ) {
				*minDistBack = -temp/alpha;
				if ( *maxDistFront>*minDistBack ) {
					return false;
				}
			}
		}
	}
	else {		// alpha==0.0
		if ( pdotn<-dimen || pdotn>dimen ) {
			return false;
		}
	}
	return true;
}

bool ViewableTorus::FindIntersectionNT ( 
				const VectorR3& viewPos, const VectorR3& viewDir, 
				double maxDistance, double *intersectDistance, 
				VisiblePoint& returnedPoint ) const
{

	// Precheck for collisions by
	//	checking if passes to within a bounding box

	bool insideFlag = true;		// Whether viewPos is inside bounding box
	double minDistBack = DBL_MAX;	// min. distance to a backplane bounding the box
	double maxDistFront= -DBL_MAX;	// mix. distance to a frontplane bounding the box

	double pdotn;
	double alpha;
	pdotn = (viewPos^AxisC) - CenterCoefC;
	alpha = viewDir^AxisC;
	if ( !CollideTwoPlanes(pdotn, alpha, MinorRadius, 
							&insideFlag, &minDistBack, &maxDistFront) ) {
		return false;
	}
	pdotn = (viewPos^AxisA) - CenterCoefA;
	alpha = viewDir^AxisA;
	if ( !CollideTwoPlanes(pdotn, alpha, OuterRadius, 
							&insideFlag, &minDistBack, &maxDistFront) ) {
		return false;
	}
	pdotn = (viewPos^AxisB) - CenterCoefB;
	alpha = viewDir^AxisB;
	if ( !CollideTwoPlanes(pdotn, alpha, OuterRadius, 
							&insideFlag, &minDistBack, &maxDistFront) ) {
		return false;
	}

	assert ( minDistBack>=maxDistFront );
	assert ( insideFlag || maxDistFront>=0.0 );
	assert ( maxDistFront < 1000.0 );
	if (maxDistFront>maxDistance) {
		return false;
	}

	// Bounding box precheck is done. Now do the actual intersection test.

	// Set up the degree 4 polynomial for the torus intersection
	// Coefficients are:
	//	A = 1
	//	B = 4 u \cdot p
	//	C = 4(u\cdot p)^2 + 2(p\cdot p) - 2(M^2+m^2) + 4M^2(u_C \cdot u)
	//	D = 4[(p \cdot p)(u \cdot p) - (M^2+m^2)(u \cdot p) + 2M^2(u_C \dot p)^2]
	//	E = ((p \cdot p)^2 - 2(M^2+m^2)(p \cdot p) + 4M^2(u_C\cdot p)^2 +(M^2-m^2)^2

	double moveFwdDist = Max(0.0,maxDistFront);

	double coefs[5];
	double roots[4];
	double &A=coefs[0], &B=coefs[1], &C=coefs[2], &D=coefs[3], &E=coefs[4];

	A = 1;
	VectorR3 viewPosRel;
	viewPosRel = viewDir;
	viewPosRel *= moveFwdDist;	// Move forward distance moveFwdDist
	viewPosRel += viewPos;
	viewPosRel -= Center;		// Position relative to center
	double udotp = (viewDir^viewPosRel);
	B = 4.0*udotp;
	double MSq = Square(MajorRadius);
	double mSq = Square(MinorRadius);
	double RadiiSqSum = MSq + mSq;
	double ucdotp = (AxisC^viewPosRel);
	double ucdotu = (AxisC^viewDir);
	double pSq = (viewPosRel^viewPosRel);
	C = 4.0*udotp*udotp + 2.0*pSq - 2.0*RadiiSqSum + 4.0*MSq*ucdotu*ucdotu;
	D = 4.0 * ( (pSq-RadiiSqSum)*udotp + 2.0*MSq*ucdotp*ucdotu );
	E = (pSq - 2.0*RadiiSqSum)*pSq + 4.0*MSq*ucdotp*ucdotp + Square(MSq-mSq);

	int numRoots = PolySolveReal( 4, coefs, roots );
	for ( int i=0; i<numRoots; i++ ) {
		roots[i] += moveFwdDist;		// Restate as distance from viewPos
		if ( roots[i] >= maxDistance ) {
			return false;
		}
		if ( roots[i]>0.0 ) {
			// Return this visible point
			*intersectDistance = roots[i];
			VectorR3 Point = viewDir;
			Point *= roots[i];
			Point += viewPos;
			returnedPoint.SetPosition(Point);  // Intersection position (not relative to center)

			if ( i & 0x01 ) {
				returnedPoint.SetBackFace();					// Orientation
				returnedPoint.SetMaterial( *InnerMaterial );	// Material
			}
			else {
				returnedPoint.SetFrontFace();					// Orientation
				returnedPoint.SetMaterial( *OuterMaterial );	// Material
			}

			// Outward normal
			Point -= Center;			// Now its the relative point
			double xCoord = Point^AxisA;	// forward axis
			double yCoord = Point^AxisB;	// rightward axis
			double zCoord = Point^AxisC;	// upward axis
			VectorR3 outN = AxisC;
			outN *= -zCoord;
			outN += Point;						// Project point down to plane of torus center
			double outNnorm = outN.Norm();
			outN *= MajorRadius/(-outNnorm);	// Negative point projected to center path of torus
			outN += Point;						// Displacement of point from center path of torus
			outN /= MinorRadius;				// Should be outward unit vector
			outN.ReNormalize();					// Fix roundoff error problems
			returnedPoint.SetNormal(outN);		// Outward normal

			// u - v coordinates
			double u = atan2( yCoord, xCoord );
			u = u*PI2inv+0.5;
			double bVal = outNnorm-MajorRadius;
			double v = atan2( zCoord, bVal );
			v = v*PI2inv+0.5;
			returnedPoint.SetUV(u , v);
			returnedPoint.SetFaceNumber( 0 );

			return true;
		}
	}
	return false;
}

void ViewableTorus::CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const
{
	double centerDot = (u^Center);
	double deltaDot = MajorRadius*sqrt(Square(u^AxisA)+Square(u^AxisB)) + MinorRadius;
	*maxDot = centerDot + deltaDot;
	*minDot = centerDot - deltaDot;
}


bool ViewableTorus::CalcPartials( const VisiblePoint& visPoint, 
								  VectorR3& retPartialU, VectorR3& retPartialV ) const
{
	VectorR3 temp;
	temp = Center;
	temp -= visPoint.GetPosition();
	temp *= AxisC;				// Cross product
	temp *= PI2;				// Times 2*Pi to convert from [0,1] to [-pi,pi]
	retPartialU = temp;

	double theta = PI2*(visPoint.GetU()-0.5);
	double phi = PI2*(visPoint.GetV()-0.5);
	double sinphi = sin(phi);
	double cosphi = cos(phi);
	double sintheta = sin(theta);
	double costheta = cos(theta);
	double pir = PI*MinorRadius;
	retPartialV = AxisA;
	retPartialV *= -pir*sinphi*costheta;
	temp = AxisB;
	temp *= -pir*sinphi*sintheta;
	retPartialV += temp;
	temp = AxisC;
	temp *= pir*cosphi;
	retPartialV += temp;

	return true;
}
