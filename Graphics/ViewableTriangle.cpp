/*
 *
 * RayTrace Software Package, release 3.1.  December 20, 2006. 
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

#include "ViewableTriangle.h"
#include "Extents.h"
#include "../VrMath/Aabb.h"

// PreCalcInfo takes the vertex values and computes information to
//		help with intersections with rays.
void ViewableTriangle::PreCalcInfo()
{
	VectorR3 EdgeAB = VertexB - VertexA;
	VectorR3 EdgeBC = VertexC - VertexB;
	VectorR3 EdgeCA = VertexA - VertexC;

	if ( (EdgeAB^EdgeBC) < (EdgeBC^EdgeCA) ) {
		Normal = EdgeAB*EdgeBC;
	}
	else {
		Normal = EdgeBC*EdgeCA;
	}
	double mag = Normal.Norm();
	if ( mag>0.0 ) {
		Normal /= mag;		// Unit vector to triangle's plane
	}
	
	PlaneCoef = (Normal^VertexA);	// Same coef for all three vertices.

	double A = EdgeAB.NormSq();
	double B = (EdgeAB^EdgeCA);
	double C = EdgeCA.NormSq();
	double Dinv = 1.0/(A*C-B*B);
	A *= Dinv;
	B *= Dinv;
	C *= Dinv;
	Ubeta = EdgeAB;
	Ubeta *= C;
	Ubeta.AddScaled( EdgeCA, -B );
	Ugamma = EdgeCA;
	Ugamma *= -A;
	Ugamma.AddScaled( EdgeAB, B );
}

bool ViewableTriangle::IsWellFormed() const
{
	return (Normal.NormSq()>0.0);
}

// Returns an intersection if found with distance maxDistance
// viewDir must be a unit vector.
// intersectDistance and visPoint are returned values.
bool ViewableTriangle::FindIntersectionNT ( 
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double *intersectDistance, VisiblePoint& returnedPoint ) const
{
	assert( IsWellFormed() );
	double mdotn = (viewDir^Normal);
	double planarDist = (viewPos^Normal)-PlaneCoef;

	// hit distance = -planarDist/mdotn
	bool frontFace = (mdotn<=0.0);
	if ( frontFace ) {
		if ( planarDist<=0 || planarDist >= -maxDistance*mdotn ) {
			return false;
		}
	}
	else {
		if ( BackFaceCulled() || planarDist>=0 || -planarDist >= maxDistance*mdotn ) {
			return false;
		}
	}

	*intersectDistance = -planarDist/mdotn;
	VectorR3 q;		
	q = viewDir;
	q *= *intersectDistance;
	q += viewPos;						// Point of view line intersecting plane

	// Compute barycentric coordinates
	VectorR3 v(q);
	v -= VertexA;
	double vCoord = (v^Ubeta);
	if ( vCoord<0.0 ) {
		return false;
	}
	double wCoord = (v^Ugamma);
	if ( wCoord<0.0 || vCoord+wCoord>1.0 ) {
		return false;
	}

	returnedPoint.SetPosition( q );		// Set point of intersection
	returnedPoint.SetUV( vCoord, wCoord );

	// Front/Back face info already set above
	if ( frontFace ) {
		returnedPoint.SetMaterial( *FrontMat );
		returnedPoint.SetFrontFace();
	}
	else {
		returnedPoint.SetMaterial( *BackMat );
		returnedPoint.SetBackFace();
	}
	returnedPoint.SetNormal( Normal );
	returnedPoint.SetFaceNumber( 0 );

	return true;
}

void ViewableTriangle::CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const
{
	double mind = (u^VertexA);
	double maxd = (u^VertexB);
	double t;
	if ( maxd < mind ) {
		t = maxd;				// Swap values, so mind < maxd
		maxd = mind;
		mind = t;
	}
	t = (u^VertexC);
	if ( t<mind ) {
		mind = t;
	}
	else if ( t>maxd ) {
		maxd = t;
	}
	*minDot = mind;
	*maxDot = maxd;		
}

bool ViewableTriangle::CalcExtentsInBox( const AABB& boundingAABB, AABB& retAABB ) const
{
	return ( ::CalcExtentsInBox( *this, boundingAABB.GetBoxMin(), boundingAABB.GetBoxMax(),
						   &(retAABB.GetBoxMin()), &(retAABB.GetBoxMax()) ) );
}

bool ViewableTriangle::CalcPartials( const VisiblePoint& visPoint, 
									 VectorR3& retPartialU, VectorR3& retPartialV ) const
{
	retPartialU = VertexB;
	retPartialU -= VertexA;
	retPartialV = VertexC;
	retPartialV -= VertexA;
	return true;			// Not a singularity point (triangles should not be degenerate)
}
