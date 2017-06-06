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

#ifndef VIEWABLECONE_H
#define VIEWABLECONE_H

#include "ViewableBase.h"
#include "Material.h"
#include "../VrMath/LinearR3.h"

class ViewableCone : public ViewableBase {

public:

	// Constructors
	ViewableCone() { Reset(); }

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
	ViewableType GetViewableType() const { return Viewable_Cone; }

	// The center axis point up out of the top of the cone.
	// SetCenterAxis(...) should be called before the other set routines, otherwise
	//		strange effects can occur.  SetCenterAxis() chooses radial axes
	//		automatically -- this can be overridden by  calling SetRadialAxes AFTERWARDS.
	void SetCenterAxis( double x, double y, double z );
	void SetCenterAxis( const double* axisC );
	void SetCenterAxis( const float* axisC );
	void SetCenterAxis( const VectorR3& axisC );

	// You should call SetSlope() for circular cones; 
	//		or call both SetSlopes and SetRadialAxes for 
	//		elliptical cross-section cylinders. 
	void SetSlope( double slope ) { SetSlopes(slope,slope);}	// Makes it a circular cone with that radius	
	void SetSlopes(double slopeA, double slopeB);  // Slopes of the cone sides

	// SetRadialAxes should be called *after* SetCenterAxis
	// The radial axes are also used for calculating u, v coordinates.
	//    the side's texture coordinate u=0.5 line is in the direction of the axisA.
	// The base's texture coordinate v-axis points in the direction of axisA.
	void SetRadialAxis( const VectorR3& axisA );
	
	// SetApex sets the location of the apex point.  
	void SetApex( double x, double y, double z );
	void SetApex( const double* center );
	void SetApex( const float* center );
	void SetApex( const VectorR3& center );

	// A "right cone" is a cylinder with the base face 
	//  perpindicular to the center axis.
	// Calling either SetHeight makes the cone 
	//	into a right cylinder.  For right cones you should call SetHeight and
	//	SetApex().
	void SetHeight( double height );
	
	// Here is an alternative to right cylinders.
	// Cones can also be defined with an arbitrary plane bounding the base of
	//		the cone
	// The plane is specified by (a) an outward normal (unit vector)
	//	and by (b) the plane coefficient.  The plane is {x : x^(planenormal)=planeCoef }
	void SetBaseFace( const VectorR3& planenormal, double planecoef );

	bool IsRightCone() const { return IsRightConeFlag; }

	// SetMaterial() sets all the materials at once.
	// SetMaterialInner() - sets all the inner materials at once.
	// SetMaterialOuter() - sets all the outer materials at once.
	void SetMaterial(const MaterialBase *material);
	void SetMaterialInner(const MaterialBase *material);
	void SetMaterialOuter(const MaterialBase *material);
	void SetMaterialSideInner(const MaterialBase *material);
	void SetMaterialSideOuter(const MaterialBase *material);
	void SetMaterialBaseInner(const MaterialBase *material);
	void SetMaterialBaseOuter(const MaterialBase *material);

	// U-V coordinates are returned with the sides in [0,1]x[0,1], the base
	//		U-V coordinates are in the circle inscribed in [1,2]x[1,2].

	// Get routines
	void GetApex( double* apex ) const { Apex.Dump( apex ); }
	void GetApex( float* apex ) const { Apex.Dump( apex ) ; }
	const VectorR3& GetApex() const { return Apex; }
	void GetCenterAxis( double* center ) const { CenterAxis.Dump( center ); }
	void GetCenterAxis( float* center ) const { CenterAxis.Dump( center ); }
	const VectorR3& GetCenterAxis() const { return CenterAxis; }
	VectorR3 GetAxisA() const { return AxisA/SlopeA; }
	VectorR3 GetAxisB() const { return AxisB/SlopeB; } 
	// GetInvScaledAxisA and GetInvScaledAxisB values are scaled by the slopes!!
	void GetInvScaledAxisA( double* axisA ) const { AxisA.Dump( axisA ); }
	void GetInvScaledAxisA( float* axisA ) const { AxisA.Dump( axisA ); }
	const VectorR3& GetInvScaledAxisA() const { return AxisA; }
	void GetInvScaledAxisB( double* axisB ) const { AxisB.Dump( axisB ); }
	void GetInvScaledAxisB( float* axisB ) const { AxisB.Dump( axisB ); }
	const VectorR3& GetInvScaledAxisB() const { return AxisB; }
	double GetSlopeA( ) const { return SlopeA; }
	double GetSlopeB( ) const { return SlopeB; }
	double GetHeight( ) const { return Height; }
	void GetBaseFace( VectorR3* planenormal, double* planeCoef ) const
		{ *planenormal = BaseNormal; *planeCoef = BasePlaneCoef; }
	const MaterialBase* GetMaterialSideOuter() const { return SideOuterMat; }
	const MaterialBase* GetMaterialSideInner() const { return SideInnerMat; }
	const MaterialBase* GetMaterialBaseOuter() const { return BaseOuterMat; }
	const MaterialBase* GetMaterialBaseInner() const { return BaseInnerMat; }

	enum {
		SideFaceNum = 0,
		BaseFaceNum = 1,
	};


protected:

	VectorR3 Apex;				// Apex Point
	VectorR3 CenterAxis;		// Unit vector up the center
	VectorR3 AxisA;				// Radial axis A - length equals SlopeA
	VectorR3 AxisB;				// Radial axis B - length equals SlopeB
	double SlopeA;
	double SlopeB;

	double Height;				// Height of right cone

	double ApexdotCenterAxis;

	// Used for non-right cylinders:
	VectorR3 BaseNormal;
	double BasePlaneCoef;

	bool IsRightConeFlag;		// True for right cones
								// False for general bounding planes

	const MaterialBase* SideOuterMat;
	const MaterialBase* SideInnerMat;
	const MaterialBase* BaseOuterMat;
	const MaterialBase* BaseInnerMat;

};

inline void ViewableCone::Reset()
{
	CenterAxis.SetUnitY();
	AxisA.SetUnitZ();
	AxisB.SetUnitX();
	SetApex( 0.0, 1.0, 0.0 );
	ApexdotCenterAxis = (Apex^CenterAxis);
	SetHeight(1.0);
	SlopeA = 1.0;
	SlopeB = 1.0;
	SetMaterial(&Material::Default);
}

inline void ViewableCone::SetCenterAxis( const VectorR3& axis )
{
	CenterAxis = axis;
	CenterAxis.Normalize();

	ApexdotCenterAxis = (Apex^CenterAxis);

	// Fix up the radial axes
	GetOrtho(CenterAxis, AxisA, AxisB);
	AxisA *= SlopeA;
	AxisB *= SlopeB;

	// Re - set the height (to fix top & bottom normals)
	if ( IsRightCone() ) {
		BaseNormal = CenterAxis;
		BaseNormal.Negate();
		BasePlaneCoef = -((ApexdotCenterAxis)-Height);
	}
}

inline void ViewableCone::SetCenterAxis( double x, double y, double z )
{
	SetCenterAxis( VectorR3(x,y,z) );
}

inline void ViewableCone::SetCenterAxis( const double* axisC )
{
	SetCenterAxis( VectorR3(*axisC, *(axisC+1), *(axisC+2)) );
}

inline void ViewableCone::SetCenterAxis( const float* axisC )
{
	SetCenterAxis( VectorR3(*axisC, *(axisC+1), *(axisC+2)) );
}

inline void ViewableCone::SetSlopes( double slopeA, double slopeB )
{
	SlopeA = slopeA;
	SlopeB = slopeB;
	assert ( SlopeA>0.0 && SlopeB>0.0 );
	AxisA *= SlopeA/AxisA.Norm();
	AxisB *= SlopeB/AxisB.Norm();
}

inline void ViewableCone::SetRadialAxis( const VectorR3& axisA )
{
	AxisA = axisA;
	AxisA -= (AxisA^CenterAxis)*CenterAxis;	// Make perpindicular to CenterAxis
	assert( AxisA.Norm()!=0.0 );			// Must not be parallel to CenterAxis
	AxisA *= SlopeA/AxisA.Norm();
	AxisB = CenterAxis*AxisA;
	AxisB *= SlopeB/AxisB.Norm();
}

inline void ViewableCone::SetApex( double x, double y, double z )
{
	Apex.Set(x, y, z);

	ApexdotCenterAxis = Apex^CenterAxis;

	if ( IsRightCone() ) {
		BasePlaneCoef = -(ApexdotCenterAxis-Height);
	}
}

inline void ViewableCone::SetApex( const double* apex )
{
	SetApex( *apex, *(apex+1), *(apex+2) );
}

inline void ViewableCone::SetApex( const float* apex )
{
	SetApex( *apex, *(apex+1), *(apex+2) );
}

inline void ViewableCone::SetApex( const VectorR3& apex )
{
	SetApex( apex.x, apex.y, apex.z );
}

inline void ViewableCone::SetHeight( double height )
{
	IsRightConeFlag = true;
	Height = height;
	BaseNormal = CenterAxis;
	BaseNormal.Negate();
	BasePlaneCoef = -(ApexdotCenterAxis-Height);
}

inline void ViewableCone::SetBaseFace( const VectorR3& planenormal, 
											 double planeCoef )
{
	assert ( (CenterAxis^planenormal) < 0.0 );	// The base should face downward
	assert ( (Apex^planenormal) < planeCoef );  // The base should be below the apex, so cone is nontrivial
	// Next assert checks that normal to base face lies inside cones slopes, so cone is finite
	assert ( Square( planenormal^CenterAxis ) >
				Square((planenormal^AxisA)/(SlopeA*SlopeA))+Square((planenormal^AxisB)/(SlopeB*SlopeB)));
	double norm = planenormal.Norm();
	BaseNormal = planenormal/norm;
	BasePlaneCoef = planeCoef/norm;
	IsRightConeFlag = false;
}

inline void ViewableCone::SetMaterial(const MaterialBase *material)
{
	SetMaterialInner( material );
	SetMaterialOuter( material );
}

inline void ViewableCone::SetMaterialInner(const MaterialBase *material)
{
	SetMaterialSideInner( material );
	SetMaterialBaseInner( material );
}

inline void ViewableCone::SetMaterialOuter(const MaterialBase *material)
{
	SetMaterialSideOuter( material );
	SetMaterialBaseOuter( material );
}

inline void ViewableCone::SetMaterialSideInner(const MaterialBase *material)
{
	SideInnerMat = material;
}

inline void ViewableCone::SetMaterialSideOuter(const MaterialBase *material)
{
	SideOuterMat = material;
}

inline void ViewableCone::SetMaterialBaseInner(const MaterialBase *material)
{
	BaseInnerMat = material;
}

inline void ViewableCone::SetMaterialBaseOuter(const MaterialBase *material)
{
	BaseOuterMat = material;
}

#endif // VIEWABLECONE_H
