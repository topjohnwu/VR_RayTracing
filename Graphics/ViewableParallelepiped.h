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

#ifndef VIEWABLEPARALLELEPIPED_H
#define VIEWABLEPARALLELEPIPED_H

#include "ViewableBase.h"
#include "Material.h"
#include "../VrMath/LinearR3.h"
#include "assert.h"

// This class subsumes cubes and rectangular prisms


class ViewableParallelepiped : public ViewableBase {

public:
	ViewableParallelepiped () { Reset(); }

	void Reset();

	// Returns an intersection if found with distance maxDistance
	// viewDir must be a unit vector.
	// intersectDistance and visPoint are returned values.
	virtual bool FindIntersectionNT ( 
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double *intersectDistance, VisiblePoint& returnedPoint ) const;
	void CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const;
	bool CalcExtentsInBox( const AABB& boundingAABB, AABB& retAABB ) const;
	bool CalcPartials( const VisiblePoint& visPoint, 
					   VectorR3& retPartialU, VectorR3& retPartialV ) const;
	ViewableType GetViewableType() const { return Viewable_Parallelepiped; }

	// The next tests are good for bounding parallelepipeds
	//  They return the distance to the hit and check against maxDistance,
	//	but do return any information about the intersection point.
	bool QuickIntersectTest(
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double *intersectDistance ) const;

	static bool QuickIntersectTest(
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double *intersectDistance,
		const VectorR3& NormalA, double MinDotA, double MaxDotA, 
		const VectorR3& NormalB, double MinDotB, double MaxDotB, 
		const VectorR3& NormalC, double MinDotC, double MaxDotC );

	static bool QuickIntersectTest(
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double *intersectDistanceIn, double *intersectDistanceOut,
		const VectorR3& NormalA, double MinDotA, double MaxDotA, 
		const VectorR3& NormalB, double MinDotB, double MaxDotB, 
		const VectorR3& NormalC, double MinDotC, double MaxDotC );


	// Setting vertices:  VertexA is the bottom, left, front vertex.
	//		Vertex B is the bottom, right, front vertex
	//		Vertex C is the top, left, front vertex
	//		Vertex D is the bottom, left, back vertex.
	// Important: vertices B, C, D must be adjacent to vertex A.
	//		The exact identity of each vertex is important only for 
	//		texture coordinate calculations.  Their circular order is
	//		important for inside/outside considerations.
	void SetVertices( double *verts );
	void SetVertices( float *verts );
	void SetVertices( const VectorR3& vertA, const VectorR3& vertB, 
						const VectorR3& vertC, const VectorR3& vertD );

	void SetMaterial( const MaterialBase *material ) 
						{ InnerMaterial = material; OuterMaterial = material; };
	void SetMaterialOuter( const MaterialBase* material ) { OuterMaterial = material; }
	void SetMaterialInner( const MaterialBase* material ) { InnerMaterial = material; }
	const MaterialBase& GetMaterialOuter() const { return *OuterMaterial; }
	const MaterialBase& GetMaterialInner() const { return *InnerMaterial; }

	void GetVertices( double *verts ) const;
	void GetVertices( float *verts ) const;
	const VectorR3& GetVertexA() const { return VertexA; }
	const VectorR3& GetVertexB() const { return VertexB; }
	const VectorR3& GetVertexC() const { return VertexC; }
	const VectorR3& GetVertexD() const { return VertexD; }
	const VectorR3& GetNormalABC() const { return NormalABC; }
	const VectorR3& GetNormalABD() const { return NormalABD; }
	const VectorR3& GetNormalACD() const { return NormalACD; }

	enum {
		FrontFaceNum = 0,
		BackFaceNum = 1,
		TopFaceNum = 2,
		BottomFaceNum = 3,
		RightFaceNum = 4,
		LeftFaceNum = 5,
	};

	// Texture Coordinates:  (OLD - This is no longer used.)
	//	Front face: [0,1]x[0,1]		- face number 0
	//	Back face:  [-2,-1]x[0,1]	- face number 1
	//	Right face: [1,2]x[0,1]		- face number 2
	//  Left face:  [-1,0]x[0,1]	- face number 3
	//  Top face:   [0,1]x[1,2]		- face number 4
	//  Bottom face:[0,1]x[-1,0]	- face number 5

protected:
	VectorR3 VertexA;		// Main corner vertex
	VectorR3 VertexB;		// Adjacent to vertex A
	VectorR3 VertexC;		// Also adjacent to vertex A
	VectorR3 VertexD;		// Third vertex adjacent to vertex A

	// The plane containing A, B, C and its opposite
	VectorR3 NormalABC;
	double TopCoefABC, BottomCoefABC;

	// The plane containing A, B, D and its opposite
	VectorR3 NormalABD;
	double TopCoefABD, BottomCoefABD;

	// The plane containing A, C, D and its opposite
	VectorR3 NormalACD;
	double TopCoefACD, BottomCoefACD;

	const MaterialBase* OuterMaterial;
	const MaterialBase* InnerMaterial;

	void CalcPlaneInfo();

};

inline void ViewableParallelepiped::Reset() 
{
	VertexA.SetZero();
	VertexB.SetUnitX();
	VertexC.SetUnitY();
	VertexD.SetUnitZ();
	CalcPlaneInfo();

	OuterMaterial = &Material::Default;
	InnerMaterial = &Material::Default;
}

inline void ViewableParallelepiped::SetVertices( double *verts )
{
	VertexA.Load( verts );
	VertexB.Load( verts+3 );
	VertexC.Load( verts+6 );
	VertexD.Load( verts+9 );
	CalcPlaneInfo();
}

inline void ViewableParallelepiped::SetVertices( float *verts )
{
	VertexA.Load( verts );
	VertexB.Load( verts+3 );
	VertexC.Load( verts+6 );
	VertexD.Load( verts+9 );
	CalcPlaneInfo();
}

inline void ViewableParallelepiped::SetVertices( const VectorR3& vertA, const VectorR3& vertB, 
						const VectorR3& vertC, const VectorR3& vertD )
{
	VertexA = vertA;
	VertexB = vertB;
	VertexC = vertC;
	VertexD = vertD;
	CalcPlaneInfo();
}

inline void ViewableParallelepiped::GetVertices( double *verts ) const
{
	VertexA.Dump( verts );
	VertexB.Dump( verts+3 );
	VertexC.Dump( verts+6 );
	VertexD.Dump( verts+9 );
}

inline void ViewableParallelepiped::GetVertices( float *verts ) const
{
	VertexA.Dump( verts );
	VertexB.Dump( verts+3 );
	VertexC.Dump( verts+6 );
	VertexD.Dump( verts+9 );
}

inline bool ViewableParallelepiped::QuickIntersectTest(
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double *intersectDistance ) const
{
	return QuickIntersectTest( viewPos, viewDir, maxDistance, intersectDistance,
								NormalABC, BottomCoefABC, TopCoefABC,
								NormalABD, BottomCoefABD, TopCoefABD,
								NormalACD, BottomCoefACD, TopCoefACD );
}

#endif // VIEWABLEPARALLELEPIPED
