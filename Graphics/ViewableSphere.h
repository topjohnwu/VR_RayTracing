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

#ifndef VIEWABLESPHERE_H
#define VIEWABLESPHERE_H

#include "ViewableBase.h"
#include "Material.h"

class ViewableSphere : public ViewableBase {

public:

	// Constructors
	ViewableSphere();
	ViewableSphere( const VectorR3& center, double radius, 
					const MaterialBase *material=&Material::Default );

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
	ViewableType GetViewableType() const { return Viewable_Sphere; }

	// QuickIntersectTest returns (a) if hit occurs, and (b) distance.
	//		Useful for things like bounding sphere tests
	static bool QuickIntersectTest( 
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double* intersectDistance,
		const VectorR3& centerPos, double radiusSq );

	bool QuickIntersectTest(
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double* intersectDistance) const;


	void SetRadius( double radius );
	void SetMaterial( const MaterialBase *material ) 
						{ InnerMaterial = material; OuterMaterial = material; };
	void SetMaterialOuter( const MaterialBase *material ) 
						{ OuterMaterial = material; };
	void SetMaterialInner( const MaterialBase *material ) 
						{ InnerMaterial = material; };
	void SetCenter( double x, double y, double z );
	void SetCenter( const double *center );
	void SetCenter( const float *center );
	void SetCenter( const VectorR3& center ) { Center = center; }

	void SetuvAxes( const double* axisA, const double* axisC);
	void SetuvAxes( const float* axisA, const float* axisC);
	void SetuvAxes( const VectorR3& axisA, const VectorR3& axisC);
	void SetuvSpherical();		// Sets u,v coords to be spherical projection
	void SetuvCylindrical();	// Sets u,v coords to be cylidrical projection
	void ResetUV();

	double GetRadius () const { return Radius; }
	double GetRadiusSq () const { return RadiusSq; }
	const MaterialBase* GetMaterialOuter () const { return OuterMaterial; }
	const MaterialBase* GetMaterialInner () const { return InnerMaterial; }
	void GetCenter( double *center);
	void GetCenter( float *center);
	const VectorR3& GetCenter() const { return Center; }
	int GetUVType() const { return uvProjectionType; }
	bool IsUVSpherical() const {return (uvProjectionType==0);}
	bool IsUVCylindrical() const {return (uvProjectionType==1);}
	const VectorR3& GetAxisA() const { return AxisA; }
	const VectorR3& GetAxisB() const { return AxisB; }
	const VectorR3& GetAxisC() const { return AxisC; }

	void CalcUV( const VectorR3& posVec, VectorR2* returnedUV ) const;
	static void CalcUV( double x, double y, double z, int uvtype,
				 VectorR2* returnedUV );

protected:

	double Radius;
	double RadiusSq;
	VectorR3 Center;
	const MaterialBase* OuterMaterial;
	const MaterialBase* InnerMaterial;

	int uvProjectionType;	// ==0 for spherical, ==1 for cylindrical
	VectorR3 AxisA;			// Axis for u = 0.5	(like z-axis)
	VectorR3 AxisB;			// Axis for u = 3/4. (like x-axis)
	VectorR3 AxisC;			// Axis for v.		(like y-axis)

};

inline
ViewableSphere::ViewableSphere( ) 
{
	SetRadius ( 1.0 );
	SetCenter ( VectorR3::Zero );
	SetMaterialOuter( &Material::Default );				
	SetMaterialInner( &Material::Default );				
	ResetUV();
}


inline
ViewableSphere::ViewableSphere( const VectorR3& center, double radius, 
							   const MaterialBase *material ) 
{
	SetRadius ( radius );
	SetCenter ( center );
	if ( material ) {
		SetMaterial( material );
	}
	else {
		SetMaterial( &Material::Default );
	}
	ResetUV();
}

inline
void ViewableSphere::SetuvSpherical()		// Sets u,v coords to be spherical projection
{
	uvProjectionType = 0;
}

inline
void ViewableSphere::SetuvCylindrical()		// Sets u,v coords to be cylidrical projection
{
	uvProjectionType = 1;
}

inline
void ViewableSphere::ResetUV( )
{
	VectorR3 aAxis(0.0, 0.0, 1.0);
	VectorR3 cAxis(0.0, 1.0, 0.0);
	SetuvAxes( aAxis, cAxis );
	uvProjectionType = 0;		// Spherical is the default
}

inline
void ViewableSphere::SetuvAxes(	const VectorR3& axisA, const VectorR3& axisC)
{
	AxisA = axisA;
	AxisC = axisC;
	AxisA.Normalize();
	AxisC.Normalize();
	AxisB = AxisC*AxisA;
	AxisB.Normalize();			// Really ought to be normalized already
}

inline
void ViewableSphere::SetuvAxes( const double* axisA, const double* axisC)
{
	VectorR3 uza;
	uza.Load(axisA);
	VectorR3 va;
	va.Load(axisC);
	SetuvAxes(uza, va);
}

inline
void ViewableSphere::SetuvAxes( const float* axisA, const float* axisC)
{
	VectorR3 uza;
	uza.Load(axisA);
	VectorR3 va;
	va.Load(axisC);
	SetuvAxes(uza, va);
}


inline void ViewableSphere::SetRadius (double radius) {
	Radius = radius;
	RadiusSq = radius*radius;
}

inline void ViewableSphere::SetCenter( double x, double y, double z ) {
	Center.Set(x, y, z);
}

inline void ViewableSphere::SetCenter( const double *center ) {
	SetCenter( *center, *(center+1), *center+2 );
}

inline void ViewableSphere::SetCenter( const float *center ) {
	SetCenter( *center, *(center+1), *center+2 );
}

inline void ViewableSphere::GetCenter( double* center ) {
	Center.Dump(center);
}

inline void ViewableSphere::GetCenter( float* center ) {
	Center.Dump(center);
}

inline bool ViewableSphere::QuickIntersectTest(
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double* intersectDistance ) const
{
	return QuickIntersectTest( viewPos, viewDir, maxDistance, 
							   intersectDistance, Center, RadiusSq );
}


#endif // VIEWABLESPHERE_H
