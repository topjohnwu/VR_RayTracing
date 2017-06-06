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

// Parallelepiped.cpp
//  Author: Sam Buss, January 2005.

#include "Parallelepiped.h"

VectorR3 Parallelepiped::GetNormalFront() const
{
	VectorR3 ret( EdgeA );
	ret *= EdgeB;
	ret.Normalize();
	return ret;
}

VectorR3 Parallelepiped::GetNormalLeft() const
{
	VectorR3 ret( EdgeB );
	ret *= EdgeC;
	ret.Normalize();
	return ret;
}

VectorR3 Parallelepiped::GetNormalBottom() const
{
	VectorR3 ret( EdgeC );
	ret *= EdgeA;
	ret.Normalize();
	return ret;
}


// The six GetFace functions return the fours vertices of the face
//		in counterclockwise order as seen from outside the parallelepiped
// "Front" means bounded by edges A and B.
// "Left" means bounded by edges B and C.
// "Bottom" means bounded by edges C and A.
// The base point lies at the bottom left front corner.

void Parallelepiped::GetFrontFace( VectorR3 *retVerts ) const
{
	*retVerts = BasePoint;
	*(retVerts+1) = BasePoint;
	*(retVerts+1) += EdgeA;
	*(retVerts+2) = *(retVerts+1);
	*(retVerts+2) += EdgeB;
	*(retVerts+3) = BasePoint;
	*(retVerts+3) += EdgeB;
}

void Parallelepiped::GetBackFace( VectorR3 *retVerts ) const
{
	*retVerts = BasePoint;
	*retVerts += EdgeC;
	*(retVerts+3) = *retVerts;
	*(retVerts+3) += EdgeA;
	*(retVerts+2) = *(retVerts+3);
	*(retVerts+2) += EdgeB;
	*(retVerts+1) = *retVerts;
	*(retVerts+1) += EdgeB;
}

void Parallelepiped::GetLeftFace( VectorR3 *retVerts ) const
{
	*retVerts = BasePoint;
	*(retVerts+1) = BasePoint;
	*(retVerts+1) += EdgeB;
	*(retVerts+2) = *(retVerts+1);
	*(retVerts+2) += EdgeC;
	*(retVerts+3) = BasePoint;
	*(retVerts+3) += EdgeC;
}
	
void Parallelepiped::GetRightFace( VectorR3 *retVerts ) const
{
	*retVerts = BasePoint;
	*retVerts += EdgeA;
	*(retVerts+3) = *retVerts;
	*(retVerts+3) += EdgeB;				// Base + EdgeA + EdgeB
	*(retVerts+2) = *(retVerts+3);
	*(retVerts+2) += EdgeC;				// Base + EdgeA + EdgeB + EdgeC
	*(retVerts+1) = *retVerts;
	*(retVerts+1) += EdgeC;				// Base + EdgeA + EdgeC
}


void Parallelepiped::GetBottomFace( VectorR3 *retVerts ) const
{
	*retVerts = BasePoint;
	*(retVerts+1) = BasePoint;
	*(retVerts+1) += EdgeC;
	*(retVerts+2) = *(retVerts+1);
	*(retVerts+2) += EdgeA;
	*(retVerts+3) = BasePoint;
	*(retVerts+3) += EdgeA;
}

void Parallelepiped::GetTopFace( VectorR3 *retVerts ) const
{
	*retVerts = BasePoint;
	*retVerts += EdgeB;
	*(retVerts+3) = *retVerts;
	*(retVerts+3) += EdgeC;
	*(retVerts+2) = *(retVerts+3);
	*(retVerts+2) += EdgeA;
	*(retVerts+1) = *retVerts;
	*(retVerts+1) += EdgeA;
}
