/*
 *
 * RayTrace Software Package, release 3.2b.  May 3, 2006; October 5, 2008.
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

#include "Quaternion.h"
#include "MathMisc.h"
#include "LinearR2.h"
#include "LinearR3.h"
#include "LinearR4.h"

const Quaternion Quaternion::Identity( 0.0, 0.0, 0.0, 1.0 );

// ******************************************************************
// * Quaternion class - member functions							*
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **

// Convert a rotation map into an equivalent Quaternion.
Quaternion& Quaternion::Set( const RotationMapR3& A)
{
	// Use Shepperd's algorithm, which is stable, does not lose
	//		significant precision and uses only one sqrt.
	//   J. Guidance and Control, 1 (1978) 223-224.
	double m00 = A.m11 + A.m22 + A.m33;	// Trace of A
	double temp;
	if ( m00 >= 0.5 ) {
		w = sqrt( 1.0+m00 );
		double wInv = 1.0/w;
		x = (A.m32-A.m23)*wInv;
		y = (A.m13-A.m31)*wInv;
		z = (A.m21-A.m12)*wInv;
	}
	else if ( (temp = A.m11+A.m11-m00)>=0.5 ) {
		x = sqrt(1.0+temp);
		double xInv = 1.0/x;
		w = (A.m32-A.m23)*xInv;
		y = (A.m21+A.m12)*xInv;
		z = (A.m31+A.m13)*xInv;
	}
	else if ( (temp=A.m22+A.m22-m00) >=0.5 ) {
		y = sqrt(1.0+temp);
		double yInv = 1.0/y;
		w = (A.m13-A.m31)*yInv;
		x = (A.m21+A.m12)*yInv;
		z = (A.m32+A.m23)*yInv;
	}
	else {
		z = sqrt(1.0+A.m33+A.m33-m00);
		double zInv = 1.0/z;
		w = (A.m21-A.m12)*zInv;
		x = (A.m31+A.m13)*zInv;
		y = (A.m32+A.m23)*zInv;
	}
	w *= 0.5;
	x *= 0.5;
	y *= 0.5;
	z *= 0.5;
	return *this;
}

// Convert rotation specified by vector to an equivalent quaternion
Quaternion& Quaternion::SetRotate( const VectorR3& rotVec)
{
	double theta = rotVec.Norm();
	double halftheta = theta*0.5;
	double sOver = SineOver(halftheta)*0.5;   // sin(theta/2)/theta
	x = rotVec.x*sOver;
	y = rotVec.y*sOver;
	z = rotVec.z*sOver;
	w = cos(halftheta);
	return *this;
}

// ******************************************************************
// * VectorR3/RotationMapR3 class - member functions				*
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **

// this.Rotate( q ) -- Apply quaternion q to rotate this vector.
VectorR3& VectorR3::Rotate( const Quaternion& quat )
{
	RotationMapR3 M;
	M.Set(quat);
	(*this) = M*(*this);
	return *this;
}

VectorR3& VectorR3::Set( const Quaternion& q )
{
	double sinhalf = sqrt( Square(q.x)+Square(q.y)+Square(q.z) );
	if (sinhalf>0.0) {
		double theta = atan2( sinhalf, q.w );
		theta += theta;
		this->Set( q.x, q.y, q.z );
		(*this) *= (theta/sinhalf);
	}
	else {
		this->SetZero();
	}
	return *this;
}

RotationMapR3& RotationMapR3::Set( const Quaternion& quat )
{
	double wSq = quat.w*quat.w;
	double xSq = quat.x*quat.x;
	double ySq = quat.y*quat.y;
	double zSq = quat.z*quat.z;
	double Dqwx = 2.0*quat.w*quat.x;
	double Dqwy = 2.0*quat.w*quat.y;
	double Dqwz = 2.0*quat.w*quat.z;
	double Dqxy = 2.0*quat.x*quat.y;
	double Dqyz = 2.0*quat.y*quat.z;
	double Dqxz = 2.0*quat.x*quat.z;
	m11 = wSq+xSq-ySq-zSq;
	m22 = wSq-xSq+ySq-zSq;
	m33 = wSq-xSq-ySq+zSq;
	m12 = Dqxy-Dqwz;
	m21 = Dqxy+Dqwz;
	m13 = Dqxz+Dqwy;
	m31 = Dqxz-Dqwy;
	m23 = Dqyz-Dqwx;
	m32 = Dqyz+Dqwx;
	return *this;
}


//*******************************************************************
// Solid Geometry routines											*
//*******************************************************************

// Compute the angle formed by two geodesics on the unit sphere.
//	Three unit vectors u,v,w specify the geodesics u-v and v-w which
//  meet at vertex v.  The angle from v-w to v-u is returned.  This
//  is always in the range [0, 2PI).
// The three vectors should be unit vectors and should be distinct
double SphereAngle( const VectorR3& u, const VectorR3& v, const VectorR3& w ) {
	VectorR3 vuPerp = ProjectPerpUnit ( u, v );
	VectorR3 vwPerp = ProjectPerpUnit ( w, v );
	double costheta = vwPerp^vuPerp;
	double sintheta = (v*vuPerp)^vwPerp;
	double normProdInv = 1.0/sqrt(vuPerp.NormSq()*vwPerp.NormSq());
	costheta *= normProdInv;
	sintheta *= normProdInv;
	
	double theta = atan2( sintheta, costheta );
	if (theta<0.0) {
		theta += PI2;
	}
	return theta;
}




