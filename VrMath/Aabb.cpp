/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006.
 *
 * Mathematics Subpackage (VrMath)
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

// Aabb.cpp
//
// Axis Aligned Bounding Box (AABB)
//
// Implements a fairly simple minded data structure for
//	holding information about an AABB.
//
// Author: Sam Buss.
// Contact: sbuss@math.ucsd.edu
// All rights reserved.  May be used for any purpose as long
//	as use is acknowledged.

#include "Aabb.h"
#include "MathMisc.h"

// Update the Aabb to include the "newAabb"
void AABB::EnlargeToEnclose ( const AABB& aabbToEnclose )
{
	UpdateMin( aabbToEnclose.BoxMin.x, BoxMin.x );
	UpdateMin( aabbToEnclose.BoxMin.y, BoxMin.y );
	UpdateMin( aabbToEnclose.BoxMin.z, BoxMin.z );
	UpdateMax( aabbToEnclose.BoxMax.x, BoxMax.x );
	UpdateMax( aabbToEnclose.BoxMax.y, BoxMax.y );
	UpdateMax( aabbToEnclose.BoxMax.z, BoxMax.z );
}

double AABB::SurfaceArea() const
{
	VectorR3 delta = BoxMax;
	delta -= BoxMin;
	return 2.0*(delta.x*delta.y + delta.x*delta.z + delta.y*delta.z);
}

// Find intersection points with a ray.
bool AABB::RayEntryExit( const VectorR3& startPos, const VectorR3& dir,
						double *entryDist, int *entryFaceId,
						double *exitDist, int *exitFaceId )
{
	VectorR3 dirInv;
	int signDirX = Sign(dir.x);
	if ( signDirX!=0 ) {
		dirInv.x = 1.0/dir.x;
	}
	int signDirY = Sign(dir.y);
	if ( signDirY!=0 ) {
		dirInv.y = 1.0/dir.y;
	}
	int signDirZ = Sign(dir.z);
	if ( signDirZ!=0 ) {
		dirInv.z = 1.0/dir.z;
	}
	return RayEntryExit( startPos, signDirX, signDirY, signDirZ, dirInv,
						 entryDist, entryFaceId, exitDist, exitFaceId );
}

bool AABB::RayEntryExit( const VectorR3& startPos, 
					   int signDirX, int signDirY, int signDirZ, const VectorR3& dirInv,
					   double *entryDist, int *entryFaceId,
					   double *exitDist, int *exitFaceId )
{
	double& maxEnterDist=*entryDist;
	int& maxEnterAxis = *entryFaceId;
	double& minExitDist = *exitDist;
	int& minExitAxis = *exitFaceId;

	double mx, mn;
	if ( signDirX!=0 ) {
		if ( signDirX==1 ) {
			mx = BoxMax.x;
			mn = BoxMin.x;
		}
		else {
			mx = BoxMin.x;
			mn = BoxMax.x;
		}
		maxEnterDist = (mn-startPos.x)*dirInv.x;
		minExitDist = (mx-startPos.x)*dirInv.x;
		maxEnterAxis = 0;
		minExitAxis = 0;
	}
	else {
		if ( startPos.x<BoxMin.x || startPos.x>BoxMax.x ) {
			return false;
		}
		maxEnterDist = -DBL_MAX;
		minExitDist = DBL_MAX;
		maxEnterAxis = -1;
		minExitAxis = -1;
	}

	if ( signDirY!=0 ) {
		if ( signDirY==1 ) {
			mx = BoxMax.y;
			mn = BoxMin.y;
		}
		else {
			mx = BoxMin.y;
			mn = BoxMax.y;
		}
		double newEnterDist = (mn-startPos.y)*dirInv.y;
		double newExitDist = (mx-startPos.y)*dirInv.y;
		if ( maxEnterDist<newEnterDist ) {
			maxEnterDist = newEnterDist;
			maxEnterAxis = 1;
		}
		if ( minExitDist>newExitDist ) {
			minExitDist = newExitDist;
			minExitAxis = 1;
		}
	}
	else {
		if ( startPos.y<BoxMin.y || startPos.y>BoxMax.y ) {
			return false;
		}
	}

	if ( signDirZ!=0 ) {
		if ( signDirZ==1 ) {
			mx = BoxMax.z;
			mn = BoxMin.z;
		}
		else {
			mx = BoxMin.z;
			mn = BoxMax.z;
		}
		double newEnterDist = (mn-startPos.z)*dirInv.z;
		double newExitDist = (mx-startPos.z)*dirInv.z;
		if ( maxEnterDist<newEnterDist ) {
			maxEnterDist = newEnterDist;
			maxEnterAxis = 2;
		}
		if ( minExitDist>newExitDist ) {
			minExitDist = newExitDist;
			minExitAxis = 2;
		}
	}
	else {
		if ( startPos.z<BoxMin.z || startPos.z>BoxMax.z ) {
			return false;
		}
	}

	if ( minExitDist<maxEnterDist ) {
		return false;
	}
	return true;
}
