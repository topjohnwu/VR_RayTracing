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

#include "../VrMath/MathMisc.h"
#include "../VrMath/PolynomialRC.h"
#include "ViewableCone.h"

// Returns an intersection if found with distance maxDistance
// viewDir must be a unit vector.
// intersectDistance and visPoint are returned values.
bool ViewableCone::FindIntersectionNT ( 
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double *intersectDistance, VisiblePoint& returnedPoint ) const
{

	double maxFrontDist = -DBL_MAX;
	double minBackDist = DBL_MAX;
	int frontType, backType;		// 0, 1 = base, side

	double viewPosdotCtr = viewPos^CenterAxis;
	double udotuCtr = viewDir^CenterAxis;
	if ( viewPosdotCtr>(ApexdotCenterAxis) && udotuCtr>=0.0 ) {
		return false;				// Above the cone's apex
	}

	// Start with the bounding base plane
	double pdotnCap = BaseNormal^viewPos;
	double udotnCap = BaseNormal^viewDir;
	if ( pdotnCap>BasePlaneCoef ) {
		if ( udotnCap>=0.0 ) {
			return false;		// Above (=outside) base plane, pointing away
		}
		maxFrontDist = (BasePlaneCoef-pdotnCap)/udotnCap;
		frontType = 0;
	}
	else if ( pdotnCap<BasePlaneCoef ) {
		if ( udotnCap>0.0 ) {
			// Below (=inside) base plane, pointing towards the plane
			minBackDist = (BasePlaneCoef-pdotnCap)/udotnCap;	
			backType = 0;
		}
	}

	// Now handle the cone's sides
	VectorR3 v = viewPos;
	v -= Apex;						// View position relative to the cone apex
	double pdotuCtr = v^CenterAxis;
	double pdotuA = v^AxisA;
	double pdotuB = v^AxisB;
	// udotuCtr already defined above
	double udotuA = viewDir^AxisA;
	double udotuB = viewDir^AxisB;

	double C = pdotuA*pdotuA + pdotuB*pdotuB - pdotuCtr*pdotuCtr;
	double B = 2.0*(pdotuA*udotuA + pdotuB*udotuB - pdotuCtr*udotuCtr);
	double A = udotuA*udotuA+udotuB*udotuB-udotuCtr*udotuCtr;

	double alpha1, alpha2;	// The roots, in order
	int numRoots = QuadraticSolveReal(A, B, C, &alpha1, &alpha2);
	if ( numRoots==0 ) {
		return false;		// No intersection
	}
	bool viewMoreVertical = ( A < 0.0 );
	if ( viewMoreVertical ) {
		// View line leaves and then enters the cone
		if ( alpha1<minBackDist && pdotuCtr+alpha1*udotuCtr<=0.0 ) {
			if ( alpha1<maxFrontDist ) {
				return false;
			}
			minBackDist = alpha1;
			backType = 1;
		}
		else if ( numRoots==2 && alpha2>maxFrontDist && pdotuCtr+alpha2*udotuCtr<=0.0 ) {
			if ( alpha2>minBackDist ) {
				return false;
			}
			maxFrontDist = alpha2;
			frontType = 1;
		}
	}
	else {
		// view line enters and then leaves
		if ( alpha1>maxFrontDist ) {
			if ( pdotuCtr+alpha1*udotuCtr>0.0 ) {
				return false;				// Enters dual cone instead
			}
			if ( alpha1>minBackDist ) {
				return false;
			}
			maxFrontDist = alpha1;
			frontType = 1;
		}
		if ( numRoots==2 && alpha2<minBackDist ) {
			if ( pdotuCtr+alpha2*udotuCtr>0.0 ) {
				return false;				// Is leaving dual cone instead
			}
			if ( alpha2<maxFrontDist ) {
				return false;
			}
			minBackDist = alpha2;
			backType = 1;
		}
	}

	// Put it all together:

	double alpha;
	int hitSurface;
	if ( maxFrontDist>=0.0 ) {
		if ( maxFrontDist >= maxDistance ) {
			return false;
		}
		returnedPoint.SetFrontFace();	// Hit from outside
		alpha = maxFrontDist;
		hitSurface = frontType;
	}
	else {
		if ( minBackDist<0.0 || minBackDist >= maxDistance ) {
			return false;
		}
		alpha = minBackDist;
		returnedPoint.SetBackFace();	// Hit from inside
		hitSurface = backType;
	}


	*intersectDistance = alpha;
	// Set v to the intersection point
	v = viewDir;
	v *= alpha;
	v += viewPos;
	returnedPoint.SetPosition( v );		// Intersection point
	
	// Now set v equal to returned position relative to the apex
	v -= Apex;
	double vdotuA = v^AxisA;
	double vdotuB = v^AxisB;
	double vdotuCtr = v^CenterAxis;

	switch ( hitSurface ) {

	case 0:		// Base face
		returnedPoint.SetNormal( BaseNormal );
		if ( returnedPoint.IsFrontFacing() ) {
			returnedPoint.SetMaterial( *BaseOuterMat );
		}
		else {
			returnedPoint.SetMaterial( *BaseInnerMat );
		}

		// Calculate U-V values for texture coordinates
		vdotuA /= vdotuCtr;		// vdotuCtr is negative
		vdotuB /= vdotuCtr;
		vdotuA = 0.5*(1.0-vdotuA);
		vdotuB = 0.5*(1.0-vdotuB);
		returnedPoint.SetUV( vdotuB, vdotuA );
		returnedPoint.SetFaceNumber( BaseFaceNum );
		break;

	case 1:		// Cone's side
		VectorR3 normal;
		normal = vdotuA*AxisA;
		normal += vdotuB*AxisB;
		normal -= vdotuCtr*CenterAxis;
		normal.Normalize();
		returnedPoint.SetNormal( normal );
		if ( returnedPoint.IsFrontFacing() ) {
			returnedPoint.SetMaterial( *SideOuterMat );
		}
		else {
			returnedPoint.SetMaterial( *SideInnerMat );
		}

		// Calculate u-v coordinates for texture mapping (in range[0,1]x[0,1])
		double uCoord = atan2( vdotuB, vdotuA )/PI2 + 0.5;
		double vCoord;
		if ( IsRightCone() ) {
			vCoord = (vdotuCtr+Height)/Height;
		}
		else {
			const VectorR3& hitPos=returnedPoint.GetPosition();
			double distDown = -(BasePlaneCoef-(hitPos^BaseNormal))/(CenterAxis^BaseNormal);			
			double distUp = -vdotuCtr;
			if ( distDown+distUp > 0.0 ) {
				vCoord = distDown/(distDown+distUp);
			}
			else {
				vCoord = 0.5;	// At corner
			}
		}
		returnedPoint.SetUV( uCoord, vCoord );
		returnedPoint.SetFaceNumber( SideFaceNum );
	}
	return true;
}

void ViewableCone::CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const
{
	// Find directions to the points that whose dot products need to be checked for min/max.
	VectorR3 perp = BaseNormal;
	perp *= u;								// Vector crossproduct
	double q1 = (perp^AxisA)*SlopeA;
	double q2 = (perp^AxisB)*SlopeB;
	double q3 = -(perp^CenterAxis);
	int numSolns;							// Number of points to check
	double soln1X, soln1Y;
	double soln2X, soln2Y;
	double a = Square(q1)-Square(q3*SlopeA);
	double b = q1*q2;
	double c = Square(q2)-Square(q3*SlopeB);
	if ( a==Square(q1) || c==Square(q2) ) {		// Catch explicitly the q3==0 case.
		numSolns = 1;							// In this case, there is only solution.
		if ( q1==0.0 && q2==0.0 ) {
			soln1X = 1.0;						// This is the case of all x,y are solutions.
			soln1Y = 0.0;						// We just choose (1,0) arbitrarily
		}
		else {
			soln1X = -q2;
			soln1Y = q1;
		}
	}
	else {
		numSolns = QuadraticSolveHomogeneousReal( a, b, c, &soln1X, &soln1Y, &soln2X, &soln2Y );
		if ( numSolns==3 ) {	// If u perpindicular to the base
			numSolns = 1;
			soln1X = 1.0;		// Pick some arbitrary solution to use
			soln1Y = 0.0;
		}
		else if ( numSolns==0 ) {  // Roundoff errors have caused us to find no solution
			assert( b*b-a*c > -1.0e-13 );				// Determinant should be only barely non-zero!!
			numSolns = 1;								// soln1X and soln1Y have been set by QuadraticSolveHomogeneousReal.
			assert ( soln1X!=0.0 || soln1Y!=0.0 );		// Should never happen!
		}
		assert ( numSolns!= 0 );	// Should never happen!!  Plane thru origin must intersect the cone
	}

	// Dot product with apex point
	double apexDot = (u^Apex);
	double minD, maxD;
	minD = maxD = apexDot;

	// Try the two directions
	double apexDotBaseNormal = (Apex^BaseNormal);
	VectorR3& trial = perp;		// Reuse the perp vector (save a constructor)
	if ( numSolns==1 ) {
		double solnZ = sqrt( Square(soln1X*SlopeA)+Square(soln1Y*SlopeB) );
		trial = (soln1X/SlopeA)*AxisA+(soln1Y/SlopeB)*AxisB-solnZ*CenterAxis;
		double deltaDot = ((BasePlaneCoef-apexDotBaseNormal)/(BaseNormal^trial))*(trial^u);
		if ( deltaDot<0.0 ) {
			minD += deltaDot;
		}
		else {
			maxD += deltaDot;
		}
		trial += 2.0*solnZ*CenterAxis;
		deltaDot = ((BasePlaneCoef-apexDotBaseNormal)/(BaseNormal^trial))*(trial^u);
		UpdateMinMax( apexDot + deltaDot, minD, maxD );
	}
	else {						// if numSolns == 2
		// double solnZ = sqrt( Square(soln1X*SlopeA)+Square(soln1Y*SlopeB) );
		double solnZ = q1*soln1X + q2*soln1Y;
		trial = (q3*soln1X/SlopeA)*AxisA+(q3*soln1Y/SlopeB)*AxisB-solnZ*CenterAxis;			// q3 cannot be zero here
		double deltaDot = ((BasePlaneCoef-apexDotBaseNormal)/(BaseNormal^trial))*(trial^u);
		if ( deltaDot<0.0 ) {
			minD += deltaDot;
		}
		else {
			maxD += deltaDot;
		}
		solnZ = q1*soln2X + q2*soln2Y;
		trial = (q3*soln2X/SlopeA)*AxisA+(q3*soln2Y/SlopeB)*AxisB-solnZ*CenterAxis;
		deltaDot = ((BasePlaneCoef-apexDotBaseNormal)/(BaseNormal^trial))*(trial^u);
		UpdateMinMax( apexDot + deltaDot, minD, maxD );
	}
	*minDot = minD;
	*maxDot = maxD;
}

bool ViewableCone::CalcPartials( const VisiblePoint& visPoint, 
									 VectorR3& retPartialU, VectorR3& retPartialV ) const
{
	VectorR3 temp;
	double distDown, distUp;

	switch( visPoint.GetFaceNumber() ) {

	case BaseFaceNum:
		temp = Apex;
		temp -= visPoint.GetPosition();
		distDown = (temp^CenterAxis);	// Distance down along center axis

		retPartialU = AxisB;
		retPartialU *= distDown/(SlopeB*SlopeB);
		retPartialV = AxisA;
		retPartialV *= distDown/(SlopeA*SlopeA);

		// Above would be OK for right cones.  Now adjust for base slope
		retPartialU *= BaseNormal;
		retPartialU *= BaseNormal;
		retPartialU.Negate();
		retPartialV *= BaseNormal;
		retPartialV *= BaseNormal;
		retPartialV.Negate();
		break;

	case SideFaceNum:
		double phi = PI2*(visPoint.GetU()-0.5);	// From [0,1] to [-pi,pi]
		double sinphi = sin(phi);
		double cosphi = cos(phi);
		distDown = -(retPartialV^CenterAxis);
		retPartialU = AxisA;
		retPartialU *= -sinphi/Square(SlopeA);
		temp = AxisB;
		temp *= cosphi/Square(SlopeB);
		retPartialU += temp;
		retPartialU *= BaseNormal;
		retPartialU *= BaseNormal;
		retPartialU *= -PI2;		// Adjust sign and for range [-pi,pi]

		// distance again measured along central axis.
		retPartialV = Apex;
		retPartialV -= visPoint.GetPosition();
		distUp = -(BasePlaneCoef-(visPoint.GetPosition()^BaseNormal))/(CenterAxis^BaseNormal);			
		if (distUp==0.0) {
			retPartialV.SetZero();
			return false;
		}
		retPartialV *= (distDown+distUp)/distUp;
		break;
	}

	return true;
}
