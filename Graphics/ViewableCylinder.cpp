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
#include "ViewableCylinder.h"

// Returns an intersection if found with distance maxDistance
// viewDir must be a unit vector.
// intersectDistance and visPoint are returned values.
bool ViewableCylinder::FindIntersectionNT ( 
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double *intersectDistance, VisiblePoint& returnedPoint ) const
{
	double maxFrontDist = -DBL_MAX;
	double minBackDist = DBL_MAX;
	int frontType, backType;		// 0, 1, 2 = top, bottom, side


	// Start with the bounding planes
	if ( IsRightCylinder() ) {
		double pdotn = (viewPos^CenterAxis)-CenterDotAxis;
		double udotn = viewDir^CenterAxis;
		if ( pdotn > HalfHeight ) {
			if ( udotn>=0.0 ) {
				return false;		// Above top plane pointing up
			}
			// Hits top from above
			maxFrontDist = (HalfHeight-pdotn)/udotn;	
			frontType = 0;
			minBackDist = -(HalfHeight+pdotn)/udotn;
			backType = 1;
		}
		else if ( pdotn < -HalfHeight ) {
			if ( udotn<=0.0 ) {
				return false;		// Below bottom, pointing down
			}
			// Hits bottom plane from below
			maxFrontDist = -(HalfHeight+pdotn)/udotn;
			frontType = 1;
			minBackDist = (HalfHeight-pdotn)/udotn;	
			backType = 0;
		}
		else if ( udotn<0.0 ) {  // Inside, pointing down
			minBackDist = -(HalfHeight+pdotn)/udotn;
			backType = 1;
		}
		else if ( udotn>0.0 ) {		// Inside, pointing up
			minBackDist = (HalfHeight-pdotn)/udotn;
			backType = 0;
		}
	}
	else {	
		// Has two bounding planes (not right cylinder)
		// First handle the top plane
		double pdotnCap = TopNormal^viewPos;
		double udotnCap = TopNormal^viewDir;
		if ( pdotnCap>TopPlaneCoef ) {
			if ( udotnCap>=0.0 ) {
				return false;		// Above top plane, pointing up
			}
			maxFrontDist = (TopPlaneCoef-pdotnCap)/udotnCap;
			frontType = 0;
		}
		else if ( pdotnCap<TopPlaneCoef ) {
			if ( udotnCap>0.0 ) {
				// Below top plane, pointing up
				minBackDist = (TopPlaneCoef-pdotnCap)/udotnCap;	
				backType = 0;
			}
		}
		// Second, handle the bottom plane
		pdotnCap = BottomNormal^viewPos;
		udotnCap = BottomNormal^viewDir;
		if ( pdotnCap<BottomPlaneCoef ) {
			if ( udotnCap>0.0 ) {
				double newBackDist = (BottomPlaneCoef-pdotnCap)/udotnCap;
				if ( newBackDist<maxFrontDist ) {
					return false;
				}
				if ( newBackDist<minBackDist ) {
					minBackDist = newBackDist;
					backType = 1;
				}
			}
		}
		else if ( pdotnCap>BottomPlaneCoef ) {
			if ( udotnCap>=0.0 ) {
				return false;		// Above bottom plane, pointing up (away)
			}
			// Above bottom plane, pointing down
			double newFrontDist = (BottomPlaneCoef-pdotnCap)/udotnCap;
			if ( newFrontDist>minBackDist ) {
				return false;
			}
			if ( newFrontDist>maxFrontDist ) {
				maxFrontDist = newFrontDist;	
				frontType = 1;
			}
		}
	}
	if ( maxFrontDist>maxDistance ) {
		return false;
	}

	// Now handle the cylinder sides
	VectorR3 v = viewPos;
	v -= Center;
	double pdotuA = v^AxisA;
	double pdotuB = v^AxisB;
	double udotuA = viewDir^AxisA;
	double udotuB = viewDir^AxisB;

	double C = pdotuA*pdotuA + pdotuB*pdotuB - 1.0;
	double B = (pdotuA*udotuA + pdotuB*udotuB);

	if ( C>=0.0 && B>0.0 ) {
		return false;			// Pointing away from the cylinder
	}

	B += B;		// Double B for final 2.0 factor

	double A = udotuA*udotuA+udotuB*udotuB;

	double alpha1, alpha2;	// The roots, in order
	int numRoots = QuadraticSolveRealSafe(A, B, C, &alpha1, &alpha2);
	if ( numRoots==0 ) {
		return false;		// No intersection
	}
	if ( alpha1>maxFrontDist ) {
		if ( alpha1>minBackDist ) {
			return false;
		}
		maxFrontDist = alpha1;
		frontType = 2;
	}
	if ( numRoots==2 && alpha2<minBackDist ) {
		if ( alpha2<maxFrontDist ) {
			return false;
		}
		minBackDist = alpha2;
		backType = 2;
	}

	// Put it all together:

	double alpha;
	int hitSurface;
	if ( maxFrontDist>0.0 ) {
		returnedPoint.SetFrontFace();	// Hit from outside
		alpha = maxFrontDist;
		hitSurface = frontType;
	}
	else {
		returnedPoint.SetBackFace();	// Hit from inside
		alpha = minBackDist;
		hitSurface = backType;
	}

	if ( alpha >= maxDistance ) {
		return false;
	}

	*intersectDistance = alpha;
	// Set v to the intersection point
	v = viewDir;
	v *= alpha;
	v += viewPos;
	returnedPoint.SetPosition( v );		// Intersection point
	
	// Now set v equal to returned position relative to the center
	v -= Center;
	double vdotuA = v^AxisA;
	double vdotuB = v^AxisB;

	switch ( hitSurface ) {

	case 0:		// Top surface
		returnedPoint.SetNormal( TopNormal );
		if ( returnedPoint.IsFrontFacing() ) {
			returnedPoint.SetMaterial( *TopOuterMat );
		}
		else {
			returnedPoint.SetMaterial( *TopInnerMat );
		}

		// Calculate U-V values for texture coordinates
		vdotuA = 0.5*(1.0-vdotuA);
		vdotuB = 0.5*(1.0+vdotuB);
		returnedPoint.SetUV( vdotuA, vdotuB );
		returnedPoint.SetFaceNumber( TopFaceNum );
		break;

	case 1:		// Bottom face
		returnedPoint.SetNormal( BottomNormal );
		if ( returnedPoint.IsFrontFacing() ) {
			returnedPoint.SetMaterial( *BottomOuterMat );
		}
		else {
			returnedPoint.SetMaterial( *BottomInnerMat );
		}

		// Calculate U-V values for texture coordinates
		vdotuA = 0.5*(1.0+vdotuA);
		vdotuB = 0.5*(1.0+vdotuB);
		returnedPoint.SetUV( vdotuA, vdotuB );
		returnedPoint.SetFaceNumber( BaseFaceNum );
		break;

	case 2:		// Cylinder's side
		VectorR3 normal;
		normal = vdotuA*AxisA;
		normal += vdotuB*AxisB;
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
		if ( IsRightCylinder() ) {
			vCoord = ((v^CenterAxis)+HalfHeight)/Height;
		}
		else {
			const VectorR3& hitPos=returnedPoint.GetPosition();
			double distUp = (TopPlaneCoef-(hitPos^TopNormal))/(CenterAxis^TopNormal);
			double distDown = -(BottomPlaneCoef-(hitPos^BottomNormal))/(CenterAxis^BottomNormal);
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

void ViewableCylinder::CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const
{
	double centerDot = (u^Center);
	double AxisCdotU = (CenterAxis^u);
	if ( IsRightCylinderFlag ) {
		double deltaDot = HalfHeight*fabs(AxisCdotU)
							+ sqrt(Square(RadiusA*RadiusA*(AxisA^u))+Square(RadiusB*RadiusB*(AxisB^u)));
		*minDot = centerDot - deltaDot;
		*maxDot = centerDot + deltaDot;
		return;
	}

	double maxD, minD;
	// Handle top face
	VectorR3 perp = TopNormal;
	perp *= u;
	double alpha = (perp^AxisA)*RadiusA;
	double beta = (perp^AxisB)*RadiusB;
	if ( alpha==0.0 && beta==0.0 ) {	// If u perpindicular to top face
		maxD = minD = TopPlaneCoef*(u^TopNormal);
	}
	else {
		double solnX = -beta*RadiusA*RadiusA;
		double solnY = alpha*RadiusB*RadiusB;
		double ratio = sqrt( Square(alpha*RadiusB) + Square(beta*RadiusA) );
		solnX /= ratio;									// Now solnX and solnY give point on cylinder to check
		solnY /= ratio;
		VectorR3& trial = perp;					// Be careful: reuse of VectorR3 to avoid constructor overhead
		trial = Center;
		trial.AddScaled( AxisA, solnX*RadiusA );
		trial.AddScaled( AxisB, solnY*RadiusB );
		maxD = minD = (trial^u) + (TopPlaneCoef-(trial^TopNormal))*AxisCdotU/(CenterAxis^TopNormal);
		trial = Center;
		trial.AddScaled( AxisA, -solnX*RadiusA );
		trial.AddScaled( AxisB, -solnY*RadiusB );
		double newDot = (trial^u) + (TopPlaneCoef-(trial^TopNormal))*AxisCdotU/(CenterAxis^TopNormal);
		UpdateMinMax( newDot, minD, maxD );
	}

	// Handle bottom face
	perp = BottomNormal;
	perp *= u;
	alpha = (perp^AxisA)*RadiusA;
	beta = (perp^AxisB)*RadiusB;
	if ( alpha==0.0 && beta==0.0 ) {			// If u perpindicular to bottom face
		UpdateMinMax( BottomPlaneCoef*(u^BottomNormal), minD, maxD );
	}
	else {
		double solnX = -beta*RadiusA*RadiusA;
		double solnY = alpha*RadiusB*RadiusB;
		double ratio = sqrt( Square(alpha*RadiusB) + Square(beta*RadiusA) );
		solnX /= ratio;									// Now solnX and solnY give point on cylinder to check
		solnY /= ratio;
		VectorR3& trial = perp;					// Be careful: reuse of VectorR3 to avoid constructor overhead
		trial = Center;
		trial.AddScaled( AxisA, solnX*RadiusA );
		trial.AddScaled( AxisB, solnY*RadiusB );
		double newDot = (trial^u) + (BottomPlaneCoef-(trial^BottomNormal))*AxisCdotU/(CenterAxis^BottomNormal);
		UpdateMinMax( newDot, minD, maxD );
		trial = Center;
		trial.AddScaled( AxisA, -solnX*RadiusA );
		trial.AddScaled( AxisB, -solnY*RadiusB );
		newDot = (trial^u) + (BottomPlaneCoef-(trial^BottomNormal))*AxisCdotU/(CenterAxis^BottomNormal);
		UpdateMinMax( newDot, minD, maxD );
	}

	*minDot = minD;
	*maxDot = maxD;
}

bool ViewableCylinder::CalcPartials( const VisiblePoint& visPoint, 
									 VectorR3& retPartialU, VectorR3& retPartialV ) const
{
	switch( visPoint.GetFaceNumber() ) {

	case SideFaceNum:	// Sides of cylinder
		retPartialV = CenterAxis;
		if ( IsRightCylinder() ) {
			retPartialV *= Height;
			retPartialU = visPoint.GetPosition();
			retPartialU -= Center;
			retPartialU *= CenterAxis;
			retPartialU *= -PI2;		// Convert from [-pi,pi] to [0,1] and fix sign
		}
		else {
			double u = visPoint.GetU();
			double phi = PI2*(u-0.5);		// Adjust from [0,1] to [-pi,pi]
			VectorR3 temp;
			retPartialU = AxisA;
			double radiusAxB = RadiusA*RadiusB;
			retPartialU *= -radiusAxB*sin(phi);
			temp = AxisB;
			temp *= radiusAxB*cos(phi);
			retPartialU += temp;			// This would be OK for parallel top&bottom faces
			double adjustUp = (retPartialV^TopNormal)/(CenterAxis^TopNormal);
			double adjustDown = (retPartialV^BottomNormal)/(CenterAxis^BottomNormal);
			temp = CenterAxis;
			temp *= (1-u)*adjustUp + u*adjustDown;
			retPartialU -= temp;

			const VectorR3& hitPos = visPoint.GetPosition();
			double distUp = (TopPlaneCoef-(hitPos^TopNormal))/(CenterAxis^TopNormal);
			double distDown = -(BottomPlaneCoef-(hitPos^BottomNormal))/(CenterAxis^BottomNormal);
			retPartialV *= (distUp+distDown);
		}

		break;
		
	case BaseFaceNum:
		//	Base of cylinder
		retPartialU = AxisB;
		retPartialV = AxisA;
		retPartialU *= 2.0*Square(RadiusB);		// Adjust to diameter instead of radius
		retPartialV *= 2.0*Square(RadiusA);
		if ( !IsRightCylinder() ) {
			VectorR3 temp;
			temp = CenterAxis;
			temp *= (retPartialU^BottomNormal)/(CenterAxis^BottomNormal);
			retPartialU -= temp;
			temp = CenterAxis;
			temp *= (retPartialV^BottomNormal)/(CenterAxis^BottomNormal);
			retPartialV -= temp;
		}
		break;

	case TopFaceNum:
		// Top of cylinder
		retPartialU = AxisB;
		retPartialV = AxisA;
		retPartialU *= 2.0*Square(RadiusB);		// Adjust to diameter instead of radius
		retPartialV *= -2.0*Square(RadiusA);	// Fix sign too
		if ( !IsRightCylinder() ) {
			VectorR3 temp;
			temp = CenterAxis;
			temp *= (retPartialU^TopNormal)/(CenterAxis^TopNormal);
			retPartialU -= temp;
			temp = CenterAxis;
			temp *= (retPartialV^TopNormal)/(CenterAxis^TopNormal);
			retPartialV -= temp;
		}
		break;
	}

	return true;
}

