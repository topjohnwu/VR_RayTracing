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

#ifndef VIEWABLEPARALLELOGRAM_H
#define VIEWABLEPARALLELOGRAM_H

#include "ViewableBase.h"
#include "Material.h"
#include "assert.h"

// This class subsumes reactangles and squares

class ViewableParallelogram : public ViewableBase {

public:
	ViewableParallelogram ();

	// The Init() routines specify 3 (THREE) vertices in counterclockwise order
	//		with the 4th vertex computed from the rest.
	void Init( const double* vertexpositions);
	void Init( const float* vertexpositions);
	void Init( const VectorR3& vA, const VectorR3& vB, const VectorR3& vC );

	void SetMaterial( const MaterialBase* material );
	void SetMaterialFront( const MaterialBase* frontmaterial );
	void SetMaterialBack( const MaterialBase* backmaterial );
	const MaterialBase* GetMaterialFront() const { return FrontMat; }
	const MaterialBase* GetMaterialBack() const { return BackMat; } 

	bool IsTwoSided() const { return (BackMat!=0); }
	bool BackFaceCulled() const { return (BackMat==0); }

	bool IsWellFormed() const;

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
	ViewableType GetViewableType() const { return Viewable_Parallelogram; }

	const VectorR3& GetVertexA() const { return VertexA; }
	const VectorR3& GetVertexB() const { return VertexB; }
	const VectorR3& GetVertexC() const { return VertexC; }
	const VectorR3& GetVertexD() const { return VertexD; }
	void GetVertices( double* vert ) const;
	void GetVertices( float* vert ) const;
	void GetVertices( VectorR3* vA, VectorR3* vB, VectorR3* vC, VectorR3 *vD ) const;
	const VectorR3& GetVertex( int vertnum ) const;
	const VectorR3& GetNormal() const { return Normal; }

protected:
	VectorR3 VertexA;
	VectorR3 VertexB;
	VectorR3 VertexC;
	VectorR3 VertexD;

	const MaterialBase* FrontMat;
	const MaterialBase* BackMat;	// Null point if not visible from back

	VectorR3 Normal;			// Unit normal to the plane of Parallelogram
	double PlaneCoef;			// Constant coef in def'n of the plane
	VectorR3 NormalAB, NormalBC;			// Unit vectors in from edges AB and BC
	double CoefAB, CoefBC, CoefCD, CoefDA;	// Coefs for line equations of edges
	double LengthAB, LengthBC;		// Edge lengths

	void PreCalcInfo();				// Precalculations for intersection testing speed
};

inline ViewableParallelogram::ViewableParallelogram () 
{
	SetMaterial(&Material::Default);
	VertexA.SetZero();
	VertexB.SetUnitX();
	VertexC.Set(1.0,1.0,0.0);
	PreCalcInfo();
}

// The Init() routines specify 3 (THREE) vertices in counterclockwise order
//		with the 4th vertex computed from the rest.
inline void ViewableParallelogram::Init( const double* vertexpositions)
{
	VertexA.Load( vertexpositions );
	VertexB.Load( vertexpositions+3 );
	VertexC.Load( vertexpositions+6 );
	PreCalcInfo();
}

inline void ViewableParallelogram::Init( const float* vertexpositions)
{
	VertexA.Load( vertexpositions );
	VertexB.Load( vertexpositions+3 );
	VertexC.Load( vertexpositions+6 );
	PreCalcInfo();
}

inline void ViewableParallelogram::Init(const VectorR3& vA, const VectorR3& vB, const VectorR3& vC )
{
	VertexA = vA;
	VertexB = vB;
	VertexC = vC;
	PreCalcInfo();
}

inline void ViewableParallelogram::SetMaterial(const MaterialBase* material )
{
	SetMaterialFront(material);
	SetMaterialBack(material);
}

inline void ViewableParallelogram::SetMaterialFront(const MaterialBase* frontmaterial )
{
	FrontMat = frontmaterial;
}

inline void ViewableParallelogram::SetMaterialBack( const MaterialBase* backmaterial )
{
	BackMat = backmaterial;
}

inline void ViewableParallelogram::GetVertices( double* verts ) const
{
	VertexA.Dump( verts );
	VertexB.Dump( verts+3 );
	VertexC.Dump( verts+6 );
	VertexD.Dump( verts+9 );
}

inline void ViewableParallelogram::GetVertices( float* verts ) const
{
	VertexA.Dump( verts );
	VertexB.Dump( verts+3 );
	VertexC.Dump( verts+6 );
	VertexD.Dump( verts+9 );
}

inline void ViewableParallelogram::GetVertices( VectorR3* vA, VectorR3* vB, VectorR3* vC, VectorR3* vD ) const
{
	*vA = VertexA;
	*vB = VertexB;
	*vC = VertexC;
	*vD = VertexD;
}

inline const VectorR3& ViewableParallelogram::GetVertex( int vertnum ) const
{
	assert (0 <= vertnum && vertnum <= 3);
	switch( vertnum ) {
	case 0:
		return VertexA;
	case 1:
		return VertexB;
	case 2:
		return VertexC;
	case 3: 
		return VertexD;
	}
	assert(0);
	return VertexA;		// Never happens
}

#endif // VIEWABLEPARALLELOGRAM_H
