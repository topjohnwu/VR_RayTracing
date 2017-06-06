/*
 *
 * RayTrace Software Package, release 3.1.  December 20, 2006.
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

#ifndef MATH_MISC_H
#define MATH_MISC_H

#include <math.h>
#include <float.h>

//
// Commonly used constants
//

const double DBL_NAN = sqrt(-1.0);	// Kludgy - ought to be an IEEE standard for this constant

const double PI = 3.1415926535897932384626433832795028841972;
const double PI2 = 2.0*PI;
const double PI4 = 4.0*PI;
const double PISq = PI*PI;
const double PIhalves = 0.5*PI;
const double PIthirds = PI/3.0;
const double PItwothirds = PI2/3.0;
const double PIfourths = 0.25*PI;
const double PIsixths = PI/6.0;
const double PIsixthsSq = PIsixths*PIsixths;
const double PItwelfths = PI/12.0;
const double PItwelfthsSq = PItwelfths*PItwelfths;
const double PIinv = 1.0/PI;
const double PI2inv = 0.5/PI;
const double PIhalfinv = 2.0/PI;
const double TwoPiSqrtInv = 1.0/sqrt(2.0*PI);
const double LogPI = log(PI);

const double RadiansToDegrees = 180.0/PI;
const double DegreesToRadians = PI/180;

const double OneThird = 1.0/3.0;
const double TwoThirds = 2.0/3.0;
const double OneSixth = 1.0/6.0;
const double OneEighth = 1.0/8.0;
const double OneTwelfth = 1.0/12.0;

const double Root2 = sqrt(2.0);
const double Root3 = sqrt(3.0);
const double Root2Inv = 1.0/Root2;	// sqrt(2)/2
const double HalfRoot3 = sqrt(3.0)/2.0;

const double E = exp(1.0);
const double LnTwo = log(2.0);
const double LnTwoInv = 1.0/log(2.0);

const double GoldenRatio = (sqrt(5.0)+1.0)*0.5;
const double GoldenRatioInv = (sqrt(5.0)-1.0)*0.5;  // 1.0/GoldenRatio

// Special purpose constants
const double OnePlusEpsilon15 = 1.0+1.0e-15;
const double OneMinusEpsilon15 = 1.0-1.0e-15;


inline double ZeroValue(const double& x)
{
	return 0.0;
}

// Inner product -- so can be used in templated situations

inline double InnerProduct( double x, double y ) 
{
	return x*y;
}

//
// Comparisons
//

template<class T> inline T Min ( const T& x, const T& y ) 
{
	return (x<y ? x : y);
}

template<class T> inline T Max ( const T& x, const T& y ) 
{
	return (y<x ? x : y);
}

template<class T> inline T ClampRange ( const T& x, const T& min, const T& max) 
{
	if ( x<min ) {
		return min;
	}
	if ( x>max ) {
		return max;
	}
	return x;
}

template<class T> inline bool ClampRange ( T *x, const T& min, const T& max) 
{
	if ( (*x)<min ) {
		(*x) = min;
		return false;
	}
	else if ( (*x)>max ) {
		(*x) = max;
		return false;
	}
	else {
		return true;
	}
}

#if 0
inline void ClampRangeSafe ( double* x, double min, double max) 
{
	if ( (!((*x)>=min)) || _isnan(*x) ) {	// Other compilers may use "isnan" instead of "_isnan"
		*x = min;
	}
	else if ( !((*x)<=max) ) {
		*x = max;
	}
}
#endif

template<class T> inline bool ClampMin ( T *x, const T& min) 
{
	if ( (*x)<min ) {
		(*x) = min;
		return false;
	}
	return true;
}


template<class T> inline bool ClampMax ( T *x, const T& max) 
{
	if ( (*x)>max ) {
		(*x) = max;
		return false;
	}
	return true;
}

template<class T> inline T& UpdateMin ( const T& x, T& y ) 
{
	if ( x<y ) {
		y = x;
	}
	return y;
}

template<class T> inline T& UpdateMax ( const T& x, T& y ) 
{
	if ( x>y ) {
		y = x;
	}
	return y;
}

template<class T> inline int UpdateMinMax ( const T& x, T& min, T& max ) 
{
	if ( x>max ) {
		max = x;
		return 1;
	}
	if ( x<min ) {
		min = x;
		return -1;
	}
	return 0;
}

template<class T> inline bool SameSignNonzero( const T& x, const T& y )
{
	if ( x<0 ) {
		return (y<0);
	}
	else if ( 0<x ) {
		return (0<y);
	}
	else {
		return false;
	}
}

// Template version of Sign function
template<class T> inline int Sign( T x) 
{
	if ( x<0 ) {
		return -1;
	}
	else if ( x==0 ) {
		return 0;
	}
	else {
		return 1;
	}
}

inline double Mag ( double x ) {
	return fabs(x);
}

inline double Dist ( double x, double y ) {
	return fabs(x-y);
}

template <class T>
inline bool NearEqual( const T& a, const T& b, double tolerance ) {
	T delta = a;
	delta -= b;
	return ( Mag(delta)<=tolerance );
}

inline bool EqualZeroFuzzy( double x ) {
	return ( fabs(x)<=1.0e-14 );
}

inline bool NearZero( double x, double tolerance ) {
	return ( fabs(x)<=tolerance );
}

inline bool LessOrEqualFuzzy( double x, double y )
{
	if ( x <= y ) {
		return true;
	}

	if ( y > 0.0 ) {
		if ( x>0.0 ) {
			return ( x*OneMinusEpsilon15 < y*OnePlusEpsilon15 );
		}
		else {
			return ( y<1.0e-15 );	// x==0 in this case
		}
	}
	else if ( y < 0.0 ) {
		if ( x<0.0 ) {
			return ( x*OnePlusEpsilon15 < y*OneMinusEpsilon15 );
		}
		else {
			return ( y>-1.0e-15 );	// x==0 in this case
		}
	}
	else {
		return ( -1.0e-15<x && x<1.0e-15 );
	}
}

inline bool GreaterOrEqualFuzzy ( double x, double y ) 
{
	return LessOrEqualFuzzy( y, x );
}

inline bool UpdateMaxAbs( double *maxabs, double updateval )
{
	if ( updateval > *maxabs ) {
		*maxabs = updateval;
		return true;
	}
	else if ( -updateval > *maxabs ) {
		*maxabs = -updateval;
		return true;
	}
	else {
		return false;
	}
}

// **********************************************************
// Combinations and averages.								*
// **********************************************************

template <class T>
inline void averageOf ( const T& a, const T &b, T&c ) {
	c = a;
	c += b;
	c *= 0.5;
}

template <class T>
inline void Lerp( const T& a, const T&b, double alpha, T&c ) {
	double beta = 1.0-alpha;
	if ( beta>alpha ) {
		c = b;
		c *= alpha/beta;
		c += a;
		c *= beta;
	}
	else {
		c = a;
		c *= beta/alpha;
		c += b;
		c *= alpha;
	}
}

template <class T>
inline T Lerp( const T& a, const T&b, double alpha ) {
	T ret;
	Lerp( a, b, alpha, ret );
	return ret;
}

// This version is a little better in that if a and b are equal,
//    the Lerp( a, b, alpha, c) will yield c = a = b.
template <class T>
inline void LerpDelta( const T& a, const T&b, double alpha, T& c ) {
	if ( alpha<=0.5 ) {
		c = b;
		c -= a;
		c *= alpha;
		c += a;
	}
	else {
		double beta = 1.0-alpha;
		c = a;
		c -= b;
		c *= beta;
		c += b;
	}
}

inline void LerpWith( float *a, float b, float alpha )
{
	(*a) *= (1.0f-alpha);
	(*a) += alpha*b;
}

inline void LerpWith( double *a, double b, double alpha )
{
	(*a) *= (1.0-alpha);
	(*a) += alpha*b;
}

template <class T>
inline void LerpWith( T* a, const T&b, double alpha ) {
	(*a) *= (1.0-alpha);
	a -> AddScaled( b, alpha );
}

// **********************************************************
// Trigonometry												*
// **********************************************************

// TimesCot(x) returns x*cot(x)
inline double TimesCot ( double x ) {	
	if ( -1.0e-5 < x && x < 1.0e-5 ) {
		return 1.0+x*OneThird;
	}
	else {
		return ( x*cos(x)/sin(x) );
	}
}

// SineOver(x) returns sin(x)/x.
inline double SineOver( double x ) {
	if ( -1.0e-5 < x && x < 1.0e-5 ) {
		return 1.0-x*x*OneSixth;
	}
	else {
		return sin(x)/x;
	}
}
// OverSine(x) returns x/sin(x).
inline double OverSine( double x ) {
	if ( -1.0e-5 < x && x < 1.0e-5 ) {
		return 1.0+x*x*OneSixth;
	}
	else {
		return x/sin(x);
	}
}

inline double SafeAsin( double x ) {
	if ( x <= -1.0 ) { 
		return -PIhalves;
	}
	else if ( x >= 1.0 ) {
		return PIhalves;
	}
	else {
		return asin(x);
	}
}

inline double SafeAcos( double x ) {
	if ( x <= -1.0 ) { 
		return PI;
	}
	else if ( x >= 1.0 ) {
		return 0.0;
	}
	else {
		return acos(x);
	}
}


// **********************************************************************
// Roots and powers														*
// **********************************************************************

// Square(x) returns x*x,  of course!

template<class T> inline T Square ( const T& x ) 
{
	return (x*x);
}

// Cube(x) returns x*x*x,  of course!

template<class T> inline T Cube ( const T& x ) 
{
	return (x*x*x);
}

template<class T> inline T FourthPower( const T& x )
{
	return Square(Square(x));
}

template<class T> inline T FifthPower( const T& x )
{
	return x*Square(Square(x));
}

template<class T> inline T SixthPower( const T& x )
{
	return Cube(Square(x));
}

// SafeSqrt(x) = returns sqrt(max(x, 0.0));

inline double SafeSqrt( double x ) {
	if ( x<=0.0 ) {
		return 0.0;
	}
	else {
		return sqrt(x);
	}
}


// SignedSqrt(a, s) returns (sign(s)*sqrt(a)).
inline double SignedSqrt( double a, double sgn )
{
	if ( sgn==0.0 ) {
		return 0.0;
	}
	else {
		return ( sgn>0.0 ? sqrt(a) : -sqrt(a) );
	}
}


#endif		// #ifndef MATH_MISC_H
