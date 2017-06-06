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

#include "ViewableBase.h"
#include "../VrMath/Aabb.h"

void ViewableBase::CalcAABB( AABB& retAABB ) const
{
	VectorR3& theMin = retAABB.GetBoxMin();
	VectorR3& theMax = retAABB.GetBoxMax();

	VectorR3 dirVec;
	dirVec.SetUnitX();
	CalcBoundingPlanes( dirVec, &theMin.x, &theMax.x );
	dirVec.SetUnitY();
	CalcBoundingPlanes( dirVec, &theMin.y, &theMax.y );
	dirVec.SetUnitZ();
	CalcBoundingPlanes( dirVec, &theMin.z, &theMax.z );
}

bool ViewableBase::CalcExtentsInBox( const AABB& aabb, AABB& retAABB ) const
{
	CalcAABB( retAABB );
	retAABB.IntersectAgainst( aabb );
	return( !retAABB.IsEmpty() );
}

