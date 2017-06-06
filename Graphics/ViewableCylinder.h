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

#ifndef VIEWABLECYLINDER_H
#define VIEWABLECYLINDER_H

#include "ViewableBase.h"
#include "Material.h"
#include "../VrMath/LinearR3.h"

class ViewableCylinder : public ViewableBase {

public:

	// Constructors
	ViewableCylinder() { Reset(); }

	void Reset();

	// Returns an intersection if found with distance maxDistance
	// viewDir must be a unit vector.
	// intersectDistance and visPoint are returned values.
	virtual bool FindIntersectionNT ( 
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double *intersectDistance, VisiblePoint& returnedPoint ) const;
	void CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const;
	bool CalcPartials( const VisiblePoint& visPoint, 
					   VectorR3& retPartialU, VectorR3& retPartialV ) const;
	ViewableType GetViewableType() const { return Viewable_Cylinder; }

	// SetCenterAxis should be called before the other set routines, otherwise
	//		strange effects can occur.  SetCenterAxis() chooses radial axes
	//		automatically -- this can be overridden by  calling SetRadialAxes AFTERWARDS.
	void SetCenterAxis( double x, double y, double z );
	void SetCenterAxis( const double* axisC );
	void SetCenterAxis( const float* axisC );
	void SetCenterAxis( const VectorR3& axisC );

	// You should call SetRadius() for circular cylinders; 
	//		or call both SetRadii and SetRadialAxes for 
	//		elliptical cross-section cylinders. 
	void SetRadius( double radius ) { SetRadii(radius,radius);}	// Makes it a circular cylinder with that radius	
	void SetRadii(double radiusA, double radiusB);  // Radii for the two radial axes

	// SetRadialAxes should be called *after* SetCenterAxis
	// The radial axes are also used for calculating u, v coordinates.
	//    the u=0.5 line is in the direction of Axis A.
	void SetRadialAxes( const VectorR3& axisA, const VectorR3& axisB );
	
	// For right cylinders, SetCenter is the center point.  For non-right
	//	cylinders, the center is just any point on the central axis.
	void SetCenter( double x, double y, double z );
	void SetCenter( const double* center );
	void SetCenter( const float* center );
	void SetCenter( const VectorR3& center );

	// A "right cylinder" is a cylinder with the base and top faces 
	//  perpindicular to the center axis.
	// Calling either SetHeight makes the cylinder 
	//	into a right cylinder.  For right cylinders you should call SetHeight and
	//	SetCenter().
	void SetHeight( double height );
	
	// Here is an alternative to right cylinders.
	// Cylinders can also be defined with arbitrary planes bounding the top and
	//	and bottom of the cylinder.
	// The planes are specified by (a) outward normals (unit vectors!)
	//	and by (b) the plane coefficient.  The plane is {x : x^(planenormal)=planeCoef }
	void SetBottomFace( const VectorR3& planenormal, double planecoef );
	void SetTopFace( const VectorR3& planenormal, double planeCoef );

	bool IsRightCylinder() const { return IsRightCylinderFlag; }

	// SetMaterial() sets all the materials at once.
	// SetMaterialInner() - sets all the inner materials at once.
	// SetMaterialOuter() - sets all the outer materials at once.
	void SetMaterial(const MaterialBase *material);
	void SetMaterialInner(const MaterialBase *material);
	void SetMaterialOuter(const MaterialBase *material);
	void SetMaterialSideInner(const MaterialBase *material);
	void SetMaterialSideOuter(const MaterialBase *material);
	void SetMaterialTopInner(const MaterialBase *material);
	void SetMaterialTopOuter(const MaterialBase *material);
	void SetMaterialBottomInner(const MaterialBase *material);
	void SetMaterialBottomOuter(const MaterialBase *material);

	// U-V coordinates are returned with the sides in [0,1]x[0,1], the bottom
	//		in [-1,0]x[-1,0], and the top in [1,2]x[1,2].

	// Get routines
	void GetCenter( double* center ) const { Center.Dump( center ); }
	void GetCenter( float* center ) const { Center.Dump( center ) ; }
	const VectorR3& GetCenter() const { return Center; }
	void GetCenterAxis( double* center ) const { CenterAxis.Dump( center ); }
	void GetCenterAxis( float* center ) const { CenterAxis.Dump( center ); }
	const VectorR3& GetCenterAxis() const { return CenterAxis; }
	const VectorR3 GetAxisA() const { return RadiusA*AxisA; }
	const VectorR3 GetAxisB() const { return RadiusB*AxisB; }
	// InvScaled Axis A values are scaled by (1/RadiusA).
	void GetInvScaledAxisA( double* center ) const { AxisA.Dump( center ); }
	void GetInvScaledAxisA( float* center ) const { AxisA.Dump( center ); }
	const VectorR3& GetInvScaledAxisA() const { return AxisA; }
	// InvScaled Axis B values are scaled by (1/RadiusB).
	void GetInvScaledAxisB( double* center ) const { AxisB.Dump( center ); }
	void GetInvScaledAxisB( float* center ) const { AxisB.Dump( center ); }
	const VectorR3& GetInvScaledAxisB() const { return AxisB; }
	double GetRadiusA( ) const { return RadiusA; }
	double GetRadiusB( ) const { return RadiusB; }
	double GetHeight( ) const { return Height; }
	void GetBottomFace( VectorR3* planenormal, double* planeCoef ) const
		{ *planenormal = BottomNormal; *planeCoef = BottomPlaneCoef; }
	void GetTopFace( VectorR3* planenormal, double* planeCoef ) const
		{ *planenormal = TopNormal; *planeCoef = TopPlaneCoef; }
	const MaterialBase* GetMaterialSideOuter() const { return SideOuterMat; }
	const MaterialBase* GetMaterialSideInner() const { return SideInnerMat; }
	const MaterialBase* GetMaterialTopOuter() const { return TopOuterMat; }
	const MaterialBase* GetMaterialTopInner() const { return TopInnerMat; }
	const MaterialBase* GetMaterialBottomOuter() const { return BottomOuterMat; }
	const MaterialBase* GetMaterialBottomInner() const { return BottomInnerMat; }

	enum {
		SideFaceNum = 0,
		BaseFaceNum = 1,
		TopFaceNum = 2
	};


protected:
	VectorR3 CenterAxis;		// Unit vector up the center
	VectorR3 Center;			// Point on the central axis
	VectorR3 AxisA;				// Radial axis A - length equals 1/RadiusA
	VectorR3 AxisB;				// Radial axis B - length equals 1/RadiusB
	double RadiusA;
	double RadiusB;

	double Height;				// Height of right cylinder
	double HalfHeight;			// Height/2
	double CenterDotAxis;		// Center^CenterAxis

	// Used for non-right cylinders:
	VectorR3 TopNormal;				// Unit vector - normal outward from top
	double TopPlaneCoef;
	VectorR3 BottomNormal;
	double BottomPlaneCoef;			// Unit vector - normal outward from bottom

	bool IsRightCylinderFlag;	// True for right cylinders
								// False for general bounding planes

	const MaterialBase* SideOuterMat;
	const MaterialBase* SideInnerMat;
	const MaterialBase* TopOuterMat;
	const MaterialBase* TopInnerMat;
	const MaterialBase* BottomOuterMat;
	const MaterialBase* BottomInnerMat;

};

inline void ViewableCylinder::Reset()
{
	CenterAxis.SetUnitY();
	AxisA.SetUnitZ();
	AxisB.SetUnitX();
	SetCenter(VectorR3::Zero);
	SetHeight(1.0);
	RadiusA = 1.0;
	RadiusB = 1.0;
	SetMaterial(&Material::Default);
}

inline void ViewableCylinder::SetCenterAxis( const VectorR3& axisC )
{
	CenterAxis = axisC;
	CenterAxis.Normalize();

	// Fix up the radial axes
	GetOrtho(CenterAxis, AxisA, AxisB);
	AxisA /= RadiusA;
	AxisB /= RadiusB;

	CenterDotAxis = Center^CenterAxis;

	// Re - set the height (to fix top & bottom normals)
	if ( IsRightCylinder() ) {
		TopNormal = CenterAxis;
		BottomNormal = CenterAxis;
		BottomNormal.Negate();
		TopPlaneCoef = CenterDotAxis+HalfHeight;
		BottomPlaneCoef = -(CenterDotAxis-HalfHeight);
	}
}

inline void ViewableCylinder::SetCenterAxis( double x, double y, double z )
{
	SetCenterAxis( VectorR3(x,y,z) );
}

inline void ViewableCylinder::SetCenterAxis( const double* axisC )
{
	SetCenterAxis( VectorR3(*axisC, *(axisC+1), *(axisC+2)) );
}

inline void ViewableCylinder::SetCenterAxis( const float* axisC )
{
	SetCenterAxis( VectorR3(*axisC, *(axisC+1), *(axisC+2)) );
}

inline void ViewableCylinder::SetRadii( double radiusA, double radiusB )
{
	RadiusA = radiusA;
	RadiusB = radiusB;
	assert ( RadiusA>0.0 && RadiusB>0.0 );
	AxisA *= 1.0/(RadiusA*AxisA.Norm());
	AxisB *= 1.0/(RadiusB*AxisB.Norm());
}

inline void ViewableCylinder::SetRadialAxes( const VectorR3& axisA, 
											const VectorR3& axisB )
{
	AxisA = axisA;
	AxisA -= (AxisA^CenterAxis)*CenterAxis;	// Make perpindicular to CenterAxis
	assert( AxisA.Norm()!=0.0 );			// Must not be parallel to CenterAxis
	AxisA /= RadiusA*AxisA.Norm();
	AxisB = axisB;
	AxisB -= (AxisB^CenterAxis)*CenterAxis;	// Make perpindicular to CenterAxis
	assert( AxisB.Norm()!=0.0 );			// Must not be parallel to CenterAxis
	AxisB /= RadiusB*AxisB.Norm();
}

inline void ViewableCylinder::SetCenter( double x, double y, double z )
{
	Center.Set(x, y, z);

	CenterDotAxis = Center^CenterAxis;
	if ( IsRightCylinder() ) {
		TopPlaneCoef = CenterDotAxis+HalfHeight;
		BottomPlaneCoef = -(CenterDotAxis-HalfHeight);
	}
}

inline void ViewableCylinder::SetCenter( const double* center )
{
	SetCenter( *center, *(center+1), *(center+2) );
}

inline void ViewableCylinder::SetCenter( const float* center )
{
	SetCenter( *center, *(center+1), *(center+2) );
}

inline void ViewableCylinder::SetCenter( const VectorR3& center )
{
	SetCenter( center.x, center.y, center.z );
}

inline void ViewableCylinder::SetHeight( double height )
{
	IsRightCylinderFlag = true;
	Height = height;
	HalfHeight = Height*0.5f;
	TopNormal = CenterAxis;
	BottomNormal = CenterAxis;
	BottomNormal.Negate();
	TopPlaneCoef = CenterDotAxis+HalfHeight;
	BottomPlaneCoef = -(CenterDotAxis-HalfHeight);
}

inline void ViewableCylinder::SetTopFace( const VectorR3& planenormal, 
											 double planeCoef )
{
	assert ( (CenterAxis^planenormal) > 0.0 );
	double norm = planenormal.Norm();
	TopNormal = planenormal/norm;
	TopPlaneCoef = planeCoef/norm;
	IsRightCylinderFlag = false;
}

inline void ViewableCylinder::SetBottomFace( const VectorR3& planenormal, 
											 double planeCoef )
{
	assert ( (CenterAxis^planenormal) < 0.0 );
	double norm = planenormal.Norm();
	BottomNormal = planenormal/norm;
	BottomPlaneCoef = planeCoef/norm;
	IsRightCylinderFlag = false;
}

inline void ViewableCylinder::SetMaterial(const MaterialBase *material)
{
	SetMaterialInner( material );
	SetMaterialOuter( material );
}

inline void ViewableCylinder::SetMaterialInner(const MaterialBase *material)
{
	SetMaterialSideInner( material );
	SetMaterialTopInner( material );
	SetMaterialBottomInner( material );
}

inline void ViewableCylinder::SetMaterialOuter(const MaterialBase *material)
{
	SetMaterialSideOuter( material );
	SetMaterialTopOuter( material );
	SetMaterialBottomOuter( material );
}

inline void ViewableCylinder::SetMaterialSideInner(const MaterialBase *material)
{
	SideInnerMat = material;
}

inline void ViewableCylinder::SetMaterialSideOuter(const MaterialBase *material)
{
	SideOuterMat = material;
}

inline void ViewableCylinder::SetMaterialTopInner(const MaterialBase *material)
{
	TopInnerMat = material;
}

inline void ViewableCylinder::SetMaterialTopOuter(const MaterialBase *material)
{
	TopOuterMat = material;
}

inline void ViewableCylinder::SetMaterialBottomInner(const MaterialBase *material)
{
	BottomInnerMat = material;
}

inline void ViewableCylinder::SetMaterialBottomOuter(const MaterialBase *material)
{
	BottomOuterMat = material;
}

#endif   // VIEWABLECYLINDER_H
