/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006.
 *
 * Mathematics Subpackage (VrMath)
 *
 * Author: Samuel R. Buss
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

//
// Quaternion Classes
//
//
// Also has some miscellaneous solid geometry routines that do not really
//	belong here (and may move in future releases).

#ifndef QUATERNION_H
#define QUATERNION_H

#include "LinearR3.h"
#include "LinearR4.h"
#include "MathMisc.h"


class Quaternion;			// Quaternion (x,y,z,w) values.


// ***************************************************************
// * Quaternion class - prototypes								 *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class Quaternion {

public:
	static const Quaternion Identity;

public:
	double x, y, z, w;

public:
	Quaternion() :x(0.0), y(0.0), z(0.0), w(1.0) {};
	Quaternion( double, double, double, double );

	inline Quaternion& Set( double xx, double yy, double zz, double ww );
	inline Quaternion& Set( const VectorR4& );
	Quaternion& Set( const RotationMapR3& );
	Quaternion& SetRotate( const VectorR3& );

	Quaternion& SetIdentity();		// Set to the identity map
	Quaternion  Inverse() const;	// Return the Inverse
	Quaternion& Invert();			// Invert this quaternion

	double Angle();					// Angle of rotation
	double Norm();					// Norm of x,y,z component

	Quaternion& operator*=(const Quaternion&);

	Quaternion& operator*=( double );

	Quaternion& operator+=(const Quaternion&);
	Quaternion& operator-=(const Quaternion&);

	double operator^( const Quaternion& q ) const;

};

Quaternion operator+(const Quaternion&, const Quaternion&);
Quaternion operator*(const Quaternion&, const Quaternion&);
Quaternion operator*(double a, const Quaternion& q);

inline Quaternion ToQuat( const VectorR4& v) 
{ 
	return Quaternion(v.x,v.y,v.z,v.w); 
}

inline double Quaternion::Norm() 
{
	return sqrt( x*x + y*y + z*z + w*w );
}

inline double Quaternion::Angle () 
{
	double halfAngle = asin(Norm());
	return halfAngle+halfAngle;
}


// ****************************************************************
// Solid Geometry Routines										  *
// ****************************************************************

// Compute the angle formed by two geodesics on the unit sphere.
//	Three unit vectors u,v,w specify the geodesics u-v and v-w which
//  meet at vertex uv.  The angle from v-w to v-u is returned.  This
//  is always in the range [0, 2PI).
double SphereAngle( const VectorR3& u, const VectorR3& v, const VectorR3& w );

//  Compute the area of a triangle on the unit sphere.  Three unit vectors
//		specify the corners of the triangle in COUNTERCLOCKWISE order.
inline double SphericalTriangleArea( 
						const VectorR3& u, const VectorR3& v, const VectorR3& w )
{
	double AngleA = SphereAngle( u,v,w );
	double AngleB = SphereAngle( v,w,u );
	double AngleC = SphereAngle( w,u,v );
	return ( AngleA+AngleB+AngleC - PI );
}


// ***************************************************************
// * Quaternion class - inlined member functions				 *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

//inline VectorR4::VectorR4 ( const Quaternion& q )
//: x(q.x), y(q.y), z(q.z), w(q.w) 
//{}

inline VectorR4& VectorR4::Set ( const Quaternion& q )
{
	x = q.x;  y = q.y;  z = q.z; w = q.w;
	return *this;
}

inline Quaternion::Quaternion( double xx, double yy, double zz, double ww)
: x(xx), y(yy), z(zz), w(ww) 
{}
	
inline Quaternion& Quaternion::Set( double xx, double yy, double zz, double ww )
{
	x = xx;
	y = yy;
	z = zz;
	w = ww;
	return *this;
}

inline Quaternion& Quaternion::Set( const VectorR4& u)
{
	x = u.x;
	y = u.y;
	z = u.z;
	w = u.w;
	return *this;
}

inline Quaternion& Quaternion::SetIdentity()
{
	x = y = z = 0.0;
	w = 1.0;
	return *this;
}

inline Quaternion operator+(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion q(q1);
	q += q2;
	return q;
}

inline Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion q(q1);
	q *= q2;
	return q;
}

inline Quaternion operator*(double a, const Quaternion& q)
{
	Quaternion ret(q);
	ret.x *= a;
	ret.y *= a;
	ret.z *= a;
	ret.w *= a;
	return ret;
}

inline Quaternion& Quaternion::operator*=( double a )
{
	x *= a;
	y *= a;
	z *= a;
	w *= a;
	return *this;
}

inline Quaternion& Quaternion::operator*=( const Quaternion& q )
{
	double wnew = w*q.w - (x*q.x + y*q.y + z*q.z);
	double xnew = w*q.x + q.w*x + (y*q.z - z*q.y);
	double ynew = w*q.y + q.w*y + (z*q.x - x*q.z);
	z           = w*q.z + q.w*z + (x*q.y - y*q.x);
	w = wnew;
	x = xnew;
	y = ynew;
	return *this;
}

inline Quaternion& Quaternion::operator+=(const Quaternion& q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;
	return *this;
}

inline Quaternion& Quaternion::operator-=(const Quaternion& q)
{
	x -= q.x;
	y -= q.y;
	z -= q.z;
	w -= q.w;
	return *this;
}

inline double Quaternion::operator^( const Quaternion& q ) const
{
	return (x*q.x + y*q.y + z*q.z + w*q.w);
}


inline Quaternion Quaternion::Inverse()	const	// Return the Inverse
{
	return ( Quaternion( x, y, z, -w ) );
}

inline Quaternion& Quaternion::Invert()		// Invert this quaternion
{
	w = -w;
	return *this;
}


#endif // QUATERNION_H
