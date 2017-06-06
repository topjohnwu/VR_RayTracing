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


#include "MathMisc.h"
#include "LinearR3.h"

// ******************************************************
// * VectorR3 class - math library functions				*
// * * * * * * * * * * * * * * * * * * * * * * * * * * **

const VectorR3 VectorR3::Zero;					// Default value is zero
const RotationMapR3 RotationMapR3::Identity;	// Default value is the identity

// Deprecated due to unsafeness of global initialization
//const VectorR3 UnitVecIR3(1.0, 0.0, 0.0);
//const VectorR3 UnitVecJR3(0.0, 1.0, 0.0);
//const VectorR3 UnitVecKR3(0.0, 0.0, 1.0);
//const VectorR3 VectorR3::UnitX( 1.0, 0.0, 0.0);
//const VectorR3 VectorR3::UnitY( 0.0, 1.0, 0.0);
//const VectorR3 VectorR3::UnitZ( 0.0, 0.0, 1.0);
//const VectorR3 VectorR3::NegUnitX(-1.0, 0.0, 0.0);
//const VectorR3 VectorR3::NegUnitY( 0.0,-1.0, 0.0);
//const VectorR3 VectorR3::NegUnitZ( 0.0, 0.0, -1.0);

// Deprecated due to unsafeness of global initialization
//const Matrix3x3 Matrix3x3::Identity(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
//const Matrix3x4 Matrix3x4::Identity(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0);

double VectorR3::MaxAbs() const
{
	register double m;
	m = (x>0.0) ? x : -x;
	if ( y>m ) m=y;
	else if ( -y >m ) m = -y;
	if ( z>m ) m=z;
	else if ( -z>m ) m = -z;
	return m;
}


// *********************************************************************
// Rotation routines												   *
// *********************************************************************

// s.Rotate(theta, u) rotates s and returns s 
//        rotated theta degrees around unit vector w.
VectorR3& VectorR3::Rotate( double theta, const VectorR3& w) 
{
	double c = cos(theta);
	double s = sin(theta);
	double dotw = (x*w.x + y*w.y + z*w.z);
	double v0x = dotw*w.x;
	double v0y = dotw*w.y;		// v0 = provjection onto w
	double v0z = dotw*w.z;
	double v1x = x-v0x;
	double v1y = y-v0y;			// v1 = projection onto plane normal to w
	double v1z = z-v0z;
	double v2x = w.y*v1z - w.z*v1y;
	double v2y = w.z*v1x - w.x*v1z;	// v2 = w * v1 (cross product)
	double v2z = w.x*v1y - w.y*v1x;
	
	x = v0x + c*v1x + s*v2x;
	y = v0y + c*v1y + s*v2y;
	z = v0z	+ c*v1z + s*v2z;

	return ( *this );
}

// Rotate unit vector x in the direction of "dir": length of dir is rotation angle.
//		x must be a unit vector.  dir must be perpindicular to x.
VectorR3& VectorR3::RotateUnitInDirection ( const VectorR3& dir)
{	
	double theta = dir.NormSq();
	if ( theta==0.0 ) {
		return *this;
	}
	else {
		theta = sqrt(theta);
		double costheta = cos(theta);
		double sintheta = sin(theta);
		VectorR3 dirUnit = dir/theta;
		*this = costheta*(*this) + sintheta*dirUnit;
		return ( *this );
	}
}

// ******************************************************
// * Matrix3x3 class - math library functions			*
// * * * * * * * * * * * * * * * * * * * * * * * * * * **

Matrix3x3& Matrix3x3::ReNormalize()	// Re-normalizes nearly orthonormal matrix
{
	register double alpha = m11*m11+m21*m21+m31*m31;	// First column's norm squared
	register double beta  = m12*m12+m22*m22+m32*m32;	// Second column's norm squared
	register double gamma = m13*m13+m23*m23+m33*m33;	// Third column's norm squared
	alpha = 1.0 - 0.5*(alpha-1.0);				// Get mult. factor
	beta  = 1.0 - 0.5*(beta-1.0);
	gamma = 1.0 - 0.5*(gamma-1.0);
	m11 *= alpha;								// Renormalize first column
	m21 *= alpha;
	m31 *= alpha;
	m12 *= beta;								// Renormalize second column
	m22 *= beta;
	m32 *= beta;
	m13 *= gamma;
	m23 *= gamma;
	m33 *= gamma;
	alpha = m11*m12+m21*m22+m31*m32;		// First and second column dot product
	beta  = m11*m13+m21*m23+m31*m33;		// First and third column dot product
	gamma = m12*m13+m22*m23+m32*m33;		// Second and third column dot product
	alpha *= 0.5;
	beta *= 0.5;
	gamma *= 0.5;
	register double temp1, temp2;
	temp1 = m11-alpha*m12-beta*m13;			// Update row1
	temp2 = m12-alpha*m11-gamma*m13;
	m13 -= beta*m11+gamma*m12;
	m11 = temp1;
	m12 = temp2;
	temp1 = m21-alpha*m22-beta*m23;			// Update row2
	temp2 = m22-alpha*m21-gamma*m23;
	m23 -= beta*m21+gamma*m22;
	m21 = temp1;
	m22 = temp2;
	temp1 = m31-alpha*m32-beta*m33;			// Update row3
	temp2 = m32-alpha*m31-gamma*m33;
	m33 -= beta*m31+gamma*m32;
	m31 = temp1;
	m32 = temp2;
	return *this;
}

void Matrix3x3::OperatorTimesEquals(const Matrix3x3& B)	 // Matrix product
{
	double t1, t2;		// temporary values
	t1 =  m11*B.m11 + m12*B.m21 + m13*B.m31;
	t2 =  m11*B.m12 + m12*B.m22 + m13*B.m32;
	m13 = m11*B.m13 + m12*B.m23 + m13*B.m33;
	m11 = t1;
	m12 = t2;

	t1 =  m21*B.m11 + m22*B.m21 + m23*B.m31;
	t2 =  m21*B.m12 + m22*B.m22 + m23*B.m32;
	m23 = m21*B.m13 + m22*B.m23 + m23*B.m33;
	m21 = t1;
	m22 = t2;

	t1 =  m31*B.m11 + m32*B.m21 + m33*B.m31;
	t2 =  m31*B.m12 + m32*B.m22 + m33*B.m32;
	m33 = m31*B.m13 + m32*B.m23 + m33*B.m33;
	m31 = t1;
	m32 = t2;
	return;
}

// Set  this = this * B^T
void Matrix3x3::RightMultiplyByTranspose(const Matrix3x3& B)	 // Matrix product
{
	double t1, t2;		// temporary values
	t1 =  m11*B.m11 + m12*B.m12 + m13*B.m13;	// New m11 value
	t2 =  m11*B.m21 + m12*B.m22 + m13*B.m23;	// New m12 value
	m13 = m11*B.m31 + m12*B.m32 + m13*B.m33;	// New m13 value
	m11 = t1;
	m12 = t2;

	t1 =  m21*B.m11 + m22*B.m12 + m23*B.m13;	// New m21 value
	t2 =  m21*B.m21 + m22*B.m22 + m23*B.m23;	// New m22 value
	m23 = m21*B.m31 + m22*B.m32 + m23*B.m33;	// New m23 value
	m21 = t1;
	m22 = t2;

	t1 =  m31*B.m11 + m32*B.m12 + m33*B.m13;	// New m31 value
	t2 =  m31*B.m21 + m32*B.m22 + m33*B.m23;	// New m32 value
	m33 = m31*B.m31 + m32*B.m32 + m33*B.m33;	// New m33 value
	m31 = t1;
	m32 = t2;
	return;
}

// Set this = M * this
void Matrix3x3::LeftMultiplyBy (const Matrix3x3& M)	// Composition
{	
	double t1, t2;		// temporary values

	t1 =  M.m11*m11 + M.m12*m21 + M.m13*m31;		// New m11
	t2 =  M.m21*m11 + M.m22*m21 + M.m23*m31;		// New m21
	m31 = M.m31*m11 + M.m32*m21 + M.m33*m31;		// New m31
	m11 = t1;
	m21 = t2;

	t1 =  M.m11*m12 + M.m12*m22 + M.m13*m32;		// New m12
	t2 =  M.m21*m12 + M.m22*m22 + M.m23*m32;		// New m22
	m32 = M.m31*m12 + M.m32*m22 + M.m33*m32;		// New m32
	m12 = t1;
	m22 = t2;


	t1  = M.m11*m13 + M.m12*m23 + M.m13*m33;		// New m13
	t2  = M.m21*m13 + M.m22*m23 + M.m23*m33;		// New m23
	m33 = M.m31*m13 + M.m32*m23 + M.m33*m33;		// New m33
	m13 = t1;
	m23 = t2;
}

// Set this = M^T * this
void Matrix3x3::LeftMultiplyByTranspose (const Matrix3x3& M)	// Composition
{	
	double t1, t2;		// temporary values

	t1 =  M.m11*m11 + M.m21*m21 + M.m31*m31;		// New m11
	t2 =  M.m12*m11 + M.m22*m21 + M.m32*m31;		// New m21
	m31 = M.m13*m11 + M.m23*m21 + M.m33*m31;		// New m31
	m11 = t1;
	m21 = t2;

	t1 =  M.m11*m12 + M.m21*m22 + M.m31*m32;		// New m12
	t2 =  M.m12*m12 + M.m22*m22 + M.m32*m32;		// New m22
	m32 = M.m13*m12 + M.m23*m22 + M.m33*m32;		// New m32
	m12 = t1;
	m22 = t2;


	t1  = M.m11*m13 + M.m21*m23 + M.m31*m33;		// New m13
	t2  = M.m12*m13 + M.m22*m23 + M.m32*m33;		// New m23
	m33 = M.m13*m13 + M.m23*m23 + M.m33*m33;		// New m33
	m13 = t1;
	m23 = t2;
}

VectorR3 Matrix3x3::Solve(const VectorR3& u) const	// Returns solution
{												// based on Cramer's rule
	double sd11 = m22*m33-m23*m32;
	double sd21 = m32*m13-m12*m33;
	double sd31 = m12*m23-m22*m13;
	double sd12 = m31*m23-m21*m33;
	double sd22 = m11*m33-m31*m13;
	double sd32 = m21*m13-m11*m23;
	double sd13 = m21*m32-m31*m22;
	double sd23 = m31*m12-m11*m32;
	double sd33 = m11*m22-m21*m12;

	register double detInv = 1.0/(m11*sd11 + m12*sd12 + m13*sd13);

	double rx = (u.x*sd11 + u.y*sd21 + u.z*sd31)*detInv;
	double ry = (u.x*sd12 + u.y*sd22 + u.z*sd32)*detInv;
	double rz = (u.x*sd13 + u.y*sd23 + u.z*sd33)*detInv;

	return ( VectorR3( rx, ry, rz ) );
}

// Returns sum of squares of entries
double Matrix3x3::SumSquaresNorm() const
{
	return ( m11*m11 + m12*m12 + m13*m13 + 
			 m21*m21 + m22*m22 + m23*m23 +
			 m31*m31 + m32*m32 + m33*m33   );
} 


// ******************************************************
// * Matrix3x4 class - math library functions			*
// * * * * * * * * * * * * * * * * * * * * * * * * * * **

Matrix3x4& Matrix3x4::ReNormalize()	// Re-normalizes nearly orthonormal matrix
{
	register double alpha = m11*m11+m21*m21+m31*m31;	// First column's norm squared
	register double beta  = m12*m12+m22*m22+m32*m32;	// Second column's norm squared
	register double gamma = m13*m13+m23*m23+m33*m33;	// Third column's norm squared
	alpha = 1.0 - 0.5*(alpha-1.0);				// Get mult. factor
	beta  = 1.0 - 0.5*(beta-1.0);
	gamma = 1.0 - 0.5*(gamma-1.0);
	m11 *= alpha;								// Renormalize first column
	m21 *= alpha;
	m31 *= alpha;
	m12 *= beta;								// Renormalize second column
	m22 *= beta;
	m32 *= beta;
	m13 *= gamma;
	m23 *= gamma;
	m33 *= gamma;
	alpha = m11*m12+m21*m22+m31*m32;		// First and second column dot product
	beta  = m11*m13+m21*m23+m31*m33;		// First and third column dot product
	gamma = m12*m13+m22*m23+m32*m33;		// Second and third column dot product
	alpha *= 0.5;
	beta *= 0.5;
	gamma *= 0.5;
	register double temp1, temp2;
	temp1 = m11-alpha*m12-beta*m13;			// Update row1
	temp2 = m12-alpha*m11-gamma*m13;
	m13 -= beta*m11+gamma*m12;
	m11 = temp1;
	m12 = temp2;
	temp1 = m21-alpha*m22-beta*m23;			// Update row2
	temp2 = m22-alpha*m21-gamma*m23;
	m23 -= beta*m21+gamma*m22;
	m21 = temp1;
	m22 = temp2;
	temp1 = m31-alpha*m32-beta*m33;			// Update row3
	temp2 = m32-alpha*m31-gamma*m33;
	m33 -= beta*m31+gamma*m32;
	m31 = temp1;
	m32 = temp2;
	return *this;
}

void Matrix3x4::OperatorTimesEquals (const Matrix3x4& B)	// Composition
{
	m14 += m11*B.m14 + m12*B.m24 + m13*B.m34;
	m24 += m21*B.m14 + m22*B.m24 + m23*B.m34;
	m34 += m31*B.m14 + m32*B.m24 + m33*B.m34;

	double t1, t2;		// temporary values
	t1 =  m11*B.m11 + m12*B.m21 + m13*B.m31;
	t2 =  m11*B.m12 + m12*B.m22 + m13*B.m32;
	m13 = m11*B.m13 + m12*B.m23 + m13*B.m33;
	m11 = t1;
	m12 = t2;

	t1 =  m21*B.m11 + m22*B.m21 + m23*B.m31;
	t2 =  m21*B.m12 + m22*B.m22 + m23*B.m32;
	m23 = m21*B.m13 + m22*B.m23 + m23*B.m33;
	m21 = t1;
	m22 = t2;

	t1 =  m31*B.m11 + m32*B.m21 + m33*B.m31;
	t2 =  m31*B.m12 + m32*B.m22 + m33*B.m32;
	m33 = m31*B.m13 + m32*B.m23 + m33*B.m33;
	m31 = t1;
	m32 = t2;
}

void Matrix3x4::OperatorTimesEquals (const Matrix3x3& B)	// Composition
{
	double t1, t2;		// temporary values
	t1 =  m11*B.m11 + m12*B.m21 + m13*B.m31;
	t2 =  m11*B.m12 + m12*B.m22 + m13*B.m32;
	m13 = m11*B.m13 + m12*B.m23 + m13*B.m33;
	m11 = t1;
	m12 = t2;

	t1 =  m21*B.m11 + m22*B.m21 + m23*B.m31;
	t2 =  m21*B.m12 + m22*B.m22 + m23*B.m32;
	m23 = m21*B.m13 + m22*B.m23 + m23*B.m33;
	m21 = t1;
	m22 = t2;

	t1 =  m31*B.m11 + m32*B.m21 + m33*B.m31;
	t2 =  m31*B.m12 + m32*B.m22 + m33*B.m32;
	m33 = m31*B.m13 + m32*B.m23 + m33*B.m33;
	m31 = t1;
	m32 = t2;
}

// ******************************************************
// * LinearMapR3 class - math library functions			*
// * * * * * * * * * * * * * * * * * * * * * * * * * * **


LinearMapR3 operator* ( const LinearMapR3& A, const LinearMapR3& B)
{
	return( LinearMapR3( A.m11*B.m11 + A.m12*B.m21 + A.m13*B.m31,
							A.m21*B.m11 + A.m22*B.m21 + A.m23*B.m31,
							A.m31*B.m11 + A.m32*B.m21 + A.m33*B.m31,
						 A.m11*B.m12 + A.m12*B.m22 + A.m13*B.m32,
							A.m21*B.m12 + A.m22*B.m22 + A.m23*B.m32,
							A.m31*B.m12 + A.m32*B.m22 + A.m33*B.m32,
						 A.m11*B.m13 + A.m12*B.m23 + A.m13*B.m33,
							A.m21*B.m13 + A.m22*B.m23 + A.m23*B.m33,
							A.m31*B.m13 + A.m32*B.m23 + A.m33*B.m33 ) );
}

double LinearMapR3::Determinant () const		// Returns the determinant
{
	return ( m11*(m22*m33-m23*m32) 
				- m12*(m21*m33-m31*m23)
				+ m13*(m21*m23-m31*m22) );
}

LinearMapR3 LinearMapR3::Inverse() const			// Returns inverse
{
	double sd11 = m22*m33-m23*m32;
	double sd21 = m32*m13-m12*m33;
	double sd31 = m12*m23-m22*m13;
	double sd12 = m31*m23-m21*m33;
	double sd22 = m11*m33-m31*m13;
	double sd32 = m21*m13-m11*m23;
	double sd13 = m21*m32-m31*m22;
	double sd23 = m31*m12-m11*m32;
	double sd33 = m11*m22-m21*m12;

	register double detInv = 1.0/(m11*sd11 + m12*sd12 + m13*sd13);

	return( LinearMapR3( sd11*detInv, sd12*detInv, sd13*detInv,
						 sd21*detInv, sd22*detInv, sd23*detInv,
						 sd31*detInv, sd32*detInv, sd33*detInv ) );
}

LinearMapR3& LinearMapR3::Invert() 			// Converts into inverse.
{
	// Compute the nine subdeterminants
	double sd11 = m22*m33-m23*m32;
	double sd21 = m32*m13-m12*m33;
	double sd31 = m12*m23-m22*m13;
	double sd12 = m31*m23-m21*m33;
	double sd22 = m11*m33-m31*m13;
	double sd32 = m21*m13-m11*m23;
	double sd13 = m21*m32-m31*m22;
	double sd23 = m31*m12-m11*m32;
	double sd33 = m11*m22-m21*m12;

	register double detInv = 1.0/(m11*sd11 + m12*sd12 + m13*sd13);

	m11 = sd11*detInv;
	m12 = sd21*detInv;
	m13 = sd31*detInv;
	m21 = sd12*detInv;
	m22 = sd22*detInv;
	m23 = sd32*detInv;
	m31 = sd13*detInv;
	m32 = sd23*detInv;
	m33 = sd33*detInv;

	return ( *this );
}

// Calculate inverse under assumption matrix is symmetric
// Only uses lower part of the matrix.  No checking done for symmetry
LinearMapR3 LinearMapR3::InverseSym() const
{
	LinearMapR3 ret;
	InverseSym( &ret );
	return ret;
}

// Calculate inverse under assumption matrix is symmetric
// Only uses lower part of the matrix.  No checking done for symmetry
void LinearMapR3::InverseSym( LinearMapR3* inverse ) const			
{
	// Compute the six distinct subdeterminants
	double sd11 = m22*m33-m32*m32;
	double sd12 = m31*m32-m21*m33;
	double sd22 = m11*m33-m31*m31;
	double sd13 = m21*m32-m31*m22;
	double sd23 = m31*m21-m11*m32;
	double sd33 = m11*m22-m21*m21;

	register double detInv = 1.0/(m11*sd11 + m21*sd12 + m31*sd13);

	inverse->m11 = sd11*detInv;
	inverse->m12 = inverse->m21 = sd12*detInv;
	inverse->m13 = inverse->m31 = sd13*detInv;
	inverse->m22 = sd22*detInv;
	inverse->m23 = inverse->m32 = sd23*detInv;
	inverse->m33 = sd33*detInv;
}

// Replace a matrix by its inverse under assumption matrix is symmetric
// Only uses lower part of the matrix for the calculation of the
//    inverse.  No checking done for symmetry
LinearMapR3& LinearMapR3::InvertSym()			
{
	// Compute the six distinct subdeterminants
	double sd11 = m22*m33-m32*m32;
	double sd12 = m31*m32-m21*m33;
	double sd22 = m11*m33-m31*m31;
	double sd13 = m21*m32-m31*m22;
	double sd23 = m31*m21-m11*m32;
	double sd33 = m11*m22-m21*m21;

	register double detInv = 1.0/(m11*sd11 + m21*sd12 + m31*sd13);

	m11 = sd11*detInv;
	m12 = m21 = sd12*detInv;
	m13 = m31 = sd13*detInv;
	m22 = sd22*detInv;
	m23 = m32 = sd23*detInv;
	m33 = sd33*detInv;

	return *this;
}

// Calculate inverse under assumption matrix is symmetric and positive definite
// Only uses lower part of the matrix.  No checking done for symmetry
LinearMapR3 LinearMapR3::InversePosDef() const
{
	LinearMapR3 ret;
	InversePosDef( &ret );
	return ret;
}

// Calculate inverse under assumption matrix is symmetric and positive definite
// Only uses lower part of the matrix.  No checking done for symmetry
// Positive definiteness is only partially checked with asserts.
void LinearMapR3::InversePosDef( LinearMapR3* inverse ) const			
{

	// Form a L*D*L^T representation.
	//   L = ( (1 0 0)(a 0 0)(b 0 0) * * ( (1 0 0)(0 1 0)(0 c 1) ).
	//	 D is diagonal. - d1, d2, d3 are *inverses* of the diagonal entries.
	assert ( m11>0.0 );
	double d1 = 1.0/m11;
	double a = m12*d1;
	double b = m13*d1;
	double u22star = m22 - m12*a;
	double u23star = m23 - m13*a;
	double u33star = m33 - m13*b;
	assert ( u22star>0.0 );
	double d2 = 1.0/u22star;
	double c = u23star*d2;
	double u33starstar = u33star - u23star*c;
	assert ( u33starstar>0.0 );
	double d3 = 1.0/u33starstar;

	// Compute the inverse
	// Compute the inverse
	inverse->m33 = d3;
	inverse->m23 = inverse->m32 = -c*d3;
	inverse->m22 = d2 - c*(inverse->m23);
	double acminusb = a*c - b;
	inverse->m13 = inverse->m31 = acminusb*d3;
	double ad2 = a*d2;
	inverse->m12 = inverse->m21 = -c*(inverse->m13) - ad2;
	inverse->m11 = d1 + a*ad2 + acminusb*(inverse->m13);

}

// Invert the matrix under assumption matrix is symmetric and positive definite
// Only uses lower part of the matrix.  No checking done for symmetry
// Positive definiteness is only partially checked with asserts.
LinearMapR3& LinearMapR3::InvertPosDef()		
{

	// Form a L * D * L^T representation.
	//   L = ( (1 0 0)(a 0 0)(b 0 0) * * ( (1 0 0)(0 1 0)(0 c 1) ).
	//	 D is diagonal. - d1, d2, d3 are *inverses* of the diagonal entries.
	assert ( m11>0.0 );
	double d1 = 1.0/m11;
	double a = m12*d1;
	double b = m13*d1;
	double u22star = m22 - m12*a;
	double u23star = m23 - m12*b;
	double u33star = m33 - m13*b;
	assert ( u22star>0.0 );
	double d2 = 1.0/u22star;
	double c = u23star*d2;
	double u33starstar = u33star - u23star*c;
	assert ( u33starstar>0.0 );
	double d3 = 1.0/u33starstar;

	// Compute the inverse
	m33 = d3;
	m23 = m32 = -c*d3;
	m22 = d2 - c*m23;
	double acminusb = a*c - b;
	m13 = m31 = acminusb*d3;
	double ad2 = a*d2;
	m12 = m21 = -c*m13 - ad2;
	m11 = d1 + a*ad2 + acminusb*m13;
	
	return *this;
}

// Invert the matrix under assumption matrix is symmetric and positive definite
// Only uses lower part of the matrix.  No checking done for symmetry
// Positive definiteness is only partially guarded against.
//  If the eigenvalues are all positive and not too different in magnitude,
//	will compute the correct inverse.   If one or more eigenvalues are
//	too close to zero, will compute the inverse of a slightly modified matrix.
// A better solution would re-start the calculation, but the present method
//	is faster and should work acceptably.
LinearMapR3& LinearMapR3::InvertPosDefSafe()		
{

	double trace = m11+m22+m33;
	assert ( trace>=0.0 );				// Every element on diagonal should be positive!
	double epsilon = 1.0e-5*trace;		// Threshold for being positive

	// Form a L * D * L^T representation.
	//   L = ( (1 0 0)(a 0 0)(b 0 0) * * ( (1 0 0)(0 1 0)(0 c 1) ).
	//	 D is diagonal. - d1, d2, d3 are *inverses* of the diagonal entries.
	assert ( m11 > -epsilon );		// Check for nearly non-negative definite
	m11 = Max( epsilon, m11 );		// Force divide only by positive number
	double d1 = 1.0/m11;
	double a = m12*d1;
	double b = m13*d1;
	double u22star = m22 - m12*a;
	double u23star = m23 - m12*b;
	double u33star = m33 - m13*b;
	assert ( u22star > -epsilon );		// Check for nearly non-negative definite
	u22star = Max( epsilon, u22star );	// Force divide only by positive number
	double d2 = 1.0/u22star;
	double c = u23star*d2;
	double u33starstar = u33star - u23star*c;
	assert ( u33starstar > - epsilon );		// Check for nearly non-negative definite
	u33starstar = Max( epsilon, u33starstar );	// Force divide only by positive number
	double d3 = 1.0/u33starstar;

	// Compute the inverse
	m33 = d3;
	m23 = m32 = -c*d3;
	m22 = d2 - c*m23;
	double acminusb = a*c - b;
	m13 = m31 = acminusb*d3;
	double ad2 = a*d2;
	m12 = m21 = -c*m13 - ad2;
	m11 = d1 + a*ad2 + acminusb*m13;
	
	return *this;
}



// ******************************************************
// * AffineMapR3 class - math library functions			*
// * * * * * * * * * * * * * * * * * * * * * * * * * * **

AffineMapR3 operator* ( const AffineMapR3& A, const AffineMapR3& B )
{
	return( AffineMapR3( A.m11*B.m11 + A.m12*B.m21 + A.m13*B.m31,
							A.m21*B.m11 + A.m22*B.m21 + A.m23*B.m31,
							A.m31*B.m11 + A.m32*B.m21 + A.m33*B.m31,
						 A.m11*B.m12 + A.m12*B.m22 + A.m13*B.m32,
							A.m21*B.m12 + A.m22*B.m22 + A.m23*B.m32,
							A.m31*B.m12 + A.m32*B.m22 + A.m33*B.m32,
						 A.m11*B.m13 + A.m12*B.m23 + A.m13*B.m33,
							A.m21*B.m13 + A.m22*B.m23 + A.m23*B.m33,
							A.m31*B.m13 + A.m32*B.m23 + A.m33*B.m33,
						 A.m11*B.m14 + A.m12*B.m24 + A.m13*B.m34 + A.m14,
						    A.m21*B.m14 + A.m22*B.m24 + A.m23*B.m34 + A.m24,
							A.m31*B.m14 + A.m32*B.m24 + A.m33*B.m34 + A.m34));
}

AffineMapR3 operator* ( const LinearMapR3& A, const AffineMapR3& B )
{
	return( AffineMapR3( A.m11*B.m11 + A.m12*B.m21 + A.m13*B.m31,
							A.m21*B.m11 + A.m22*B.m21 + A.m23*B.m31,
							A.m31*B.m11 + A.m32*B.m21 + A.m33*B.m31,
						 A.m11*B.m12 + A.m12*B.m22 + A.m13*B.m32,
							A.m21*B.m12 + A.m22*B.m22 + A.m23*B.m32,
							A.m31*B.m12 + A.m32*B.m22 + A.m33*B.m32,
						 A.m11*B.m13 + A.m12*B.m23 + A.m13*B.m33,
							A.m21*B.m13 + A.m22*B.m23 + A.m23*B.m33,
							A.m31*B.m13 + A.m32*B.m23 + A.m33*B.m33,
						 A.m11*B.m14 + A.m12*B.m24 + A.m13*B.m34,
						    A.m21*B.m14 + A.m22*B.m24 + A.m23*B.m34,
							A.m31*B.m14 + A.m32*B.m24 + A.m33*B.m34  ));

}
 
AffineMapR3 operator* ( const AffineMapR3& A, const LinearMapR3& B )
{
	return( AffineMapR3( A.m11*B.m11 + A.m12*B.m21 + A.m13*B.m31,
							A.m21*B.m11 + A.m22*B.m21 + A.m23*B.m31,
							A.m31*B.m11 + A.m32*B.m21 + A.m33*B.m31,
						 A.m11*B.m12 + A.m12*B.m22 + A.m13*B.m32,
							A.m21*B.m12 + A.m22*B.m22 + A.m23*B.m32,
							A.m31*B.m12 + A.m32*B.m22 + A.m33*B.m32,
						 A.m11*B.m13 + A.m12*B.m23 + A.m13*B.m33,
							A.m21*B.m13 + A.m22*B.m23 + A.m23*B.m33,
							A.m31*B.m13 + A.m32*B.m23 + A.m33*B.m33,
						 A.m14,
							A.m24,
							A.m34   ) );
}
 

AffineMapR3 AffineMapR3::Inverse() const				// Returns inverse
{
	double sd11 = m22*m33-m23*m32;
	double sd21 = m32*m13-m12*m33;
	double sd31 = m12*m23-m22*m13;
	double sd12 = m31*m23-m21*m33;
	double sd22 = m11*m33-m31*m13;
	double sd32 = m21*m13-m11*m23;
	double sd13 = m21*m32-m31*m22;
	double sd23 = m31*m12-m11*m32;
	double sd33 = m11*m22-m21*m12;

	register double detInv = 1.0/(m11*sd11 + m12*sd12 + m13*sd13);

	// Make sd's hold the (transpose of) the inverse of the 3x3 part
	sd11 *= detInv;
	sd12 *= detInv;
	sd13 *= detInv;
	sd21 *= detInv;
	sd22 *= detInv;
	sd23 *= detInv;
	sd31 *= detInv;
	sd32 *= detInv;
	sd33 *= detInv;
	double sd41 = -(m14*sd11 + m24*sd21 + m34*sd31);
	double sd42 = -(m14*sd12 + m24*sd22 + m34*sd32);
	double sd43 = -(m14*sd12 + m24*sd23 + m34*sd33);

	return( AffineMapR3( sd11, sd12, sd13,
						 sd21, sd22, sd23,
						 sd31, sd32, sd33,
						 sd41, sd42, sd43 ) );
}

AffineMapR3& AffineMapR3::Invert()					// Converts into inverse.
{
	double sd11 = m22*m33-m23*m32;
	double sd21 = m32*m13-m12*m33;
	double sd31 = m12*m23-m22*m13;
	double sd12 = m31*m23-m21*m33;
	double sd22 = m11*m33-m31*m13;
	double sd32 = m21*m13-m11*m23;
	double sd13 = m21*m32-m31*m22;
	double sd23 = m31*m12-m11*m32;
	double sd33 = m11*m22-m21*m12;

	register double detInv = 1.0/(m11*sd11 + m12*sd12 + m13*sd13);

	m11 = sd11*detInv;
	m12 = sd21*detInv;
	m13 = sd31*detInv;
	m21 = sd12*detInv;
	m22 = sd22*detInv;
	m23 = sd32*detInv;
	m31 = sd13*detInv;
	m32 = sd23*detInv;
	m33 = sd33*detInv;
	double sd41 = -(m14*m11 + m24*m12 + m34*m13);  // Compare to ::Inverse.
	double sd42 = -(m14*m21 + m24*m22 + m34*m23);
	double sd43 = -(m14*m31 + m24*m32 + m34*m33);
	m14 = sd41;
	m24 = sd42;
	m34 = sd43;

	return ( *this );
}

// **************************************************************
// * RotationMapR3 class - math library functions				*
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **

RotationMapR3 operator*(const RotationMapR3& A, const RotationMapR3& B) 
										// Matrix product (composition)
{
	return(RotationMapR3(A.m11*B.m11 + A.m12*B.m21 + A.m13*B.m31,
							A.m21*B.m11 + A.m22*B.m21 + A.m23*B.m31,
							A.m31*B.m11 + A.m32*B.m21 + A.m33*B.m31,
						 A.m11*B.m12 + A.m12*B.m22 + A.m13*B.m32,
							A.m21*B.m12 + A.m22*B.m22 + A.m23*B.m32,
							A.m31*B.m12 + A.m32*B.m22 + A.m33*B.m32,
						 A.m11*B.m13 + A.m12*B.m23 + A.m13*B.m33,
							A.m21*B.m13 + A.m22*B.m23 + A.m23*B.m33,
							A.m31*B.m13 + A.m32*B.m23 + A.m33*B.m33 ) );
}

RotationMapR3& RotationMapR3::Set( const VectorR3& u, double theta )
{
	assert ( fabs(u.NormSq()-1.0)<2.0e-6 );
	register double c = cos(theta);
	register double s = sin(theta);
	register double mc = 1.0-c;
	double xmc = u.x*mc;
	double xymc = xmc*u.y;
	double xzmc = xmc*u.z;
	double yzmc = u.y*u.z*mc;
	double xs = u.x*s;
	double ys = u.y*s;
	double zs = u.z*s;
	Matrix3x3::Set( u.x*u.x*mc+c, xymc+zs,      xzmc-ys,
					xymc-zs,	  u.y*u.y*mc+c, yzmc+xs,
					xzmc+ys,	  yzmc-xs,	    u.z*u.z*mc+c );
	return *this;
}

// The rotation axis vector u MUST be a UNIT vector!!! 
RotationMapR3& RotationMapR3::Set( const VectorR3& u, double s, double c )
{
	assert ( fabs(u.NormSq()-1.0)<2.0e-6 );
	double mc = 1.0-c;
	double xmc = u.x*mc;
	double xymc = xmc*u.y;
	double xzmc = xmc*u.z;
	double yzmc = u.y*u.z*mc;
	double xs = u.x*s;
	double ys = u.y*s;
	double zs = u.z*s;
	Matrix3x3::Set( u.x*u.x*mc+c, xymc+zs,      xzmc-ys,
					xymc-zs,	  u.y*u.y*mc+c, yzmc+xs,
					xzmc+ys,	  yzmc-xs,	    u.z*u.z*mc+c );
	return *this;
}

// ToAngle - find the angle theta of rotation.  Returns true if the rotation angle is non-zero,
//  and false if it is zero. 
double RotationMapR3::ToAngle() const
{
	double alpha = m11+m22+m33-1.0;
	double beta = sqrt(Square(m32-m23)+Square(m13-m31)+Square(m21-m12));
	return atan2( beta, alpha );
}

// ToAxisAndAngle - find a unit vector in the direction of the rotation axis,
//	and the angle theta of rotation.  Returns true if the rotation angle is non-zero,
//  and false if it is zero.
bool RotationMapR3::ToAxisAndAngle( VectorR3* u, double *theta ) const
{
	double alpha = m11+m22+m33-1.0;
	double beta = sqrt(Square(m32-m23)+Square(m13-m31)+Square(m21-m12));
	if ( beta==0.0 ) {
		u->SetUnitY();
		*theta = 0.0;
		return false;
	}
	else {
		u->Set(m32-m23, m13-m31, m21-m12);
		*u /= beta;
		*theta = atan2( beta, alpha );
		return true;
	}
}

// VrRotate is similar to glRotate.  Returns a matrix (LinearMapR3)
//		that will perform the rotation when multiplied on the left.
// u is supposed to be a *unit* vector.  Otherwise, the LinearMapR3
//		returned will be garbage!
RotationMapR3 VrRotate( double theta, const VectorR3& u )
{
	RotationMapR3 ret;
	ret.Set( u, theta );
	return ret;
}

// This version of rotate takes the cosine and sine of theta
//    instead of theta.  u must still be a unit vector.

RotationMapR3 VrRotate( double c, double s, const VectorR3& u )
{
	RotationMapR3 ret;
	ret.Set( u, s, c );
	return ret;
}

// Returns a RotationMapR3 which rotates the fromVec to be colinear
//		with toVec.

RotationMapR3 VrRotateAlign( const VectorR3& fromVec, const VectorR3& toVec)
{
	VectorR3 crossVec = fromVec;
	crossVec *= toVec;
	double sinetheta = crossVec.Norm();	// Not yet normalized!
	if ( sinetheta < 1.0e-40 ) {
		return ( RotationMapR3( 
					1.0, 0.0, 0.0, 
					0.0, 1.0, 0.0, 
					0.0, 0.0, 1.0) );
	}
	crossVec /= sinetheta;				// Normalize the vector
	double scale = 1.0/sqrt(fromVec.NormSq()*toVec.NormSq());
	sinetheta *= scale;
	double cosinetheta = (fromVec^toVec)*scale;
	return ( VrRotate( cosinetheta, sinetheta, crossVec) );
}

// RotateToMap returns a rotation map which rotates fromVec to have the
//		same direction as toVec.
// fromVec and toVec should be unit vectors
RotationMapR3 RotateToMap( const VectorR3& fromVec, const VectorR3& toVec)
{
	VectorR3 crossVec = fromVec;
	crossVec *= toVec;
	double sintheta = crossVec.Norm();
	double costheta = fromVec^toVec;
	if ( sintheta <= 1.0e-40 ) {
		if ( costheta>0.0 ) {
			return ( RotationMapR3( 
						1.0, 0.0, 0.0, 
						0.0, 1.0, 0.0, 
						0.0, 0.0, 1.0) );
		}
		else {
			GetOrtho ( toVec, crossVec );		// Get arbitrary orthonormal vector
			return( VrRotate(costheta, sintheta, crossVec ) );
		}
	}	
	else {
		crossVec /= sintheta;				// Normalize the vector
		return ( VrRotate( costheta, sintheta, crossVec) );
	}
}

// **************************************************************
// * RigidMapR3 class - math library functions					*
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **

// The rotation axis vector u MUST be a UNIT vector!!! 
RigidMapR3& RigidMapR3::SetRotationPart( const VectorR3& u, double theta )
{
	assert ( fabs(u.NormSq()-1.0)<2.0e-6 );
	register double c = cos(theta);
	register double s = sin(theta);
	register double mc = 1.0-c;
	double xmc = u.x*mc;
	double xymc = xmc*u.y;
	double xzmc = xmc*u.z;
	double yzmc = u.y*u.z*mc;
	double xs = u.x*s;
	double ys = u.y*s;
	double zs = u.z*s;
	Matrix3x4::Set3x3( u.x*u.x*mc+c, xymc+zs,      xzmc-ys,
					xymc-zs,	  u.y*u.y*mc+c, yzmc+xs,
					xzmc+ys,	  yzmc-xs,	    u.z*u.z*mc+c );
	return *this;
}

// The rotation axis vector u MUST be a UNIT vector!!! 
RigidMapR3& RigidMapR3::SetRotationPart( const VectorR3& u, double s, double c )
{
	assert ( fabs(u.NormSq()-1.0)<2.0e-6 );
	double mc = 1.0-c;
	double xmc = u.x*mc;
	double xymc = xmc*u.y;
	double xzmc = xmc*u.z;
	double yzmc = u.y*u.z*mc;
	double xs = u.x*s;
	double ys = u.y*s;
	double zs = u.z*s;
	Matrix3x4::Set3x3( u.x*u.x*mc+c, xymc+zs,      xzmc-ys,
					xymc-zs,	  u.y*u.y*mc+c, yzmc+xs,
					xzmc+ys,	  yzmc-xs,	    u.z*u.z*mc+c );
	return *this;
}


// CalcGlideRotation - converts a rigid map into an equivalent
//	glide rotation (screw motion).  It returns the rotation axis
//  as base point u, and a rotation axis v.  The vectors u and v are
//  always orthonormal.  v will be a unit vector.  
//  It also returns the glide distance, which is the translation parallel
//  to v.  Further, it returns the signed rotation angle theta (right hand rule 
//  specifies the direction.
// The glide rotation means a rotation around the point u with axis direction v.
// Return code "true" if the rotation amount is non-zero.  "false" if pure translation.
bool RigidMapR3::CalcGlideRotation( VectorR3* u, VectorR3* v, 
								    double *glideDist, double *rotation ) const
{
	// Compare to the code for ToAxisAndAngle.
	double alpha = m11+m22+m33-1.0;
	double beta = sqrt(Square(m32-m23)+Square(m13-m31)+Square(m21-m12));
	if ( beta==0.0 ) {
		double vN = m14*m14 + m24*m24 + m34*m34;
		if ( vN>0.0 ) {
			vN = sqrt(vN);
			v->Set( m14, m24, m34 );
			*v /= vN;
			*glideDist = vN;
		}
		else {
			v->SetUnitX();
			*glideDist = 0.0;
		}
		u->SetZero();
		*rotation = 0;
		return false;
	}
	else {
		v->Set(m32-m23, m13-m31, m21-m12);
		*v /= beta;				// v - unit vector, rotation axis
		*rotation = atan2( beta, alpha );
		u->Set( m14, m24, m34 );
		*glideDist = ((*u)^(*v));
		VectorR3 temp = *v;
		temp *= *glideDist;
		*u -= temp;					// Subtract component in direction of rot. axis v
		temp = *v;
		temp *= *u;
		temp /= tan((*rotation)/2);	// temp = (v \times u) / tan(rotation/2)
		*u += temp;
		*u *= 0.5;
		return true;
	}

}

// ***************************************************************
//  Linear Algebra Utilities									 *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

// Returns a righthanded orthonormal basis to complement vector u
void GetOrtho( const VectorR3& u,  VectorR3& v, VectorR3& w)
{
	if ( u.x > 0.5 || u.x<-0.5 || u.y > 0.5 || u.y<-0.5 ) {
		v.Set ( u.y, -u.x, 0.0 );
	}
	else {
		v.Set ( 0.0, u.z, -u.y);
	}
	v.Normalize();
	w = u;
	w *= v;
	w.Normalize();			// This is unnecessary if u is a unit vector
	// w.NormalizeFast();
	return;
}

// Returns a vector v orthonormal to unit vector u
void GetOrtho( const VectorR3& u,  VectorR3& v )
{
	if ( u.x > 0.5 || u.x<-0.5 || u.y > 0.5 || u.y<-0.5 ) {
		v.Set ( u.y, -u.x, 0.0 );
	}
	else {
		v.Set ( 0.0, u.z, -u.y);
	}
	v.Normalize();
	return;
}

// ***************************************************************
//  Stream Output Routines										 *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ostream& operator<< ( ostream& os, const VectorR3& u )
{
	return (os << "<" << u.x << "," << u.y << "," << u.z << ">");
}

ostream& operator<< ( ostream& os, const Matrix3x3& A )
{
	os << " <" << A.m11 << ", " << A.m12 << ", " << A.m13  << ">\n"
	   << " <" << A.m21 << ", " << A.m22 << ", " << A.m23  << ">\n"
	   << " <" << A.m31 << ", " << A.m32 << ", " << A.m33  << ">\n" ;
	return (os);
}

ostream& operator<< ( ostream& os, const Matrix3x4& A )
{
	os << " <" << A.m11 << ", " << A.m12 << ", " << A.m13 
			<< "; " << A.m14 << ">\n"
	   << " <" << A.m21 << ", " << A.m22 << ", " << A.m23 
			<< "; " << A.m24 << ">\n"
	   << " <" << A.m31 << ", " << A.m32 << ", " << A.m33 
			<< "; " << A.m34 << ">\n" ;
	return (os);
}

