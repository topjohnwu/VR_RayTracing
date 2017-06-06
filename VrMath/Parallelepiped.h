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

// Parallelepiped.h
//  Author: Sam Buss, January 2005.

#ifndef PARALLELEPIPED_H
#define PARALLELEPIPED_H

#include <assert.h>


// Stores a parallelepiped in terms of 
//		(a) base point
//		(b) three edge vectors

#include "LinearR3.h"

class Parallelepiped
{
public:

	Parallelepiped();
	Parallelepiped( const VectorR3& basePt, const VectorR3& edgeA, 
						const VectorR3& edgeB, const VectorR3& edgeC );

	void Set( const VectorR3& basePt, const VectorR3& edgeA, 
						const VectorR3& edgeB, const VectorR3& edgeC );

	const VectorR3& GetBasePt() const { return BasePoint; }
	const VectorR3& GetEdgeA() const { return EdgeA; }
	const VectorR3& GetEdgeB() const { return EdgeB; }
	const VectorR3& GetEdgeC() const { return EdgeC; }

	VectorR3 GetNormalFront() const;
	VectorR3 GetNormalLeft() const;
	VectorR3 GetNormalBottom() const;

	// The six GetFace functions return the fours vertices of the face
	//		in counterclockwise order as seen from outside the parallelepiped
	// "Front" means bounded by edges A and B.
	// "Left" means bounded by edges B and C.
	// "Bottom" means bounded by edges C and A.
	// The base point lies at the bottom left front corner.
	void GetFrontFace( VectorR3 *retVerts ) const;
	void GetBackFace( VectorR3 *retVerts ) const;
	void GetRightFace( VectorR3 *retVerts ) const;
	void GetLeftFace( VectorR3 *retVerts ) const;
	void GetTopFace( VectorR3 *retVerts ) const;
	void GetBottomFace( VectorR3 *retVerts ) const;

private:
	VectorR3 BasePoint;
	VectorR3 EdgeA;
	VectorR3 EdgeB;
	VectorR3 EdgeC;
};

inline Parallelepiped::Parallelepiped() 
{
	BasePoint.SetZero();
	EdgeA.SetUnitX();
	EdgeB.SetUnitY();
	EdgeC.SetUnitZ();
}

inline Parallelepiped::Parallelepiped( const VectorR3& basePt, const VectorR3& edgeA, 
										const VectorR3& edgeB, const VectorR3& edgeC ) 
: BasePoint(basePt), EdgeA(edgeA), EdgeB(edgeB), EdgeC(edgeC)
{
}

inline void Parallelepiped::Set( const VectorR3& basePt, const VectorR3& edgeA, 
								 const VectorR3& edgeB, const VectorR3& edgeC )
{
	BasePoint = basePt;
	EdgeA = edgeA;
	EdgeB = edgeB;
	EdgeC = edgeC;
}


#endif // PARALLELEPIPED_H