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

#include <math.h>
#include <assert.h>
#include "DoubleRecurse.h"


//  Consider a function which satisfies the following properties:
//    1.   f(N) = 1 + A*f(alpha*N) + B*f(beta*N)
//           where A+B > 1 and alpha, beta in (0,1].
//    2    f(1) = 1.
//
//  These conditions are satisfied by setting f(x) = C N^exponent + D,
//    where C, exponent, D are the values returned by FindDoubleRecurseSoln

void FindDoubleRecurseSoln ( double A, double B, double alpha, double beta,
							 double* C, double* exponent, double* D )
{
	// Pre-conditions
	assert ( A+B>1.0 );
	assert ( 0.0<alpha && alpha<=1.0 );
	assert ( 0.0<beta && beta<=1.0 );

	double denom = A+B-1.0;
	double ZdenomInv = 1.0/denom;
	double AlogAlpha = A*log(alpha);
	double BlogBeta = B*log(beta);

	*D = -ZdenomInv;
	*C = 1.0+ZdenomInv;			// Same as (A+B)/(A+B-1) = (A+B-1+1)/(A+B-1).

	// Use Newton's method to find exponent
	double X = 0;				// Initial choice for exponent
	double alphaExpX;			// ==1 for X==0.
	double betaExpX;			// ==1 for X==0.
	double funcValue = A+B;		// Since alphaExpX and betaExpX equal 1
	double deltaX = (1.0-funcValue)/(AlogAlpha+BlogBeta);
	while ( true ) {
		X += deltaX;
		if ( deltaX<1.0e-12 ) {
			break;
		}
		alphaExpX = pow(alpha, X);
		betaExpX = pow(beta, X);
		funcValue = A*alphaExpX + B*betaExpX;
		deltaX = (1.0-funcValue)/(AlogAlpha*alphaExpX+BlogBeta*betaExpX);
	}
	*exponent = X;
}

//  Consider a function which satisfies the following properties:
//    1.   f(N) = 1 + A*f(alpha*N) + B*f(beta*N)
//           where A+B > 1 and alpha, beta in (0,1].
//    2    f(1) = 1.
//
//  These conditions are satisfied by setting f(x) = C N^exponent + D,
//    where C, exponent, D are the values returned by FindDoubleRecurseSoln
//  This version takes an "exponentToBeat".   If the exponent cannot be
//	  beaten (i.e., if exponent would be greater than exponentToBeat, it
//	  returns false without computing C, D and exponent.  Otherwise, it
//	  sets values of C, D and exponent with exponent<exponentToBeat and
//	  returns true.

bool FindDoubleRecurseSoln ( double A, double B, double alpha, double beta,
							 double* C, double* exponent, double* D,
							 double exponentToBeat )
{
	// Pre-conditions
	assert ( A+B>1.0 );
	assert ( 0.0<alpha && alpha<=1.0 );
	assert ( 0.0<beta && beta<=1.0 );

	// First check on exponentToBeat
	//	Also, set some of the values for the first Newton iteration.
	double X = exponentToBeat;			// Initial choice for exponent
	double alphaExpX = pow(alpha, X);
	double betaExpX = pow(beta, X);
	double funcValue = A*alphaExpX + B*betaExpX;
	if ( funcValue>=1.0 ) {
		return false;
	}

	// We are going to beat the exponentToBeat.  
	double denom = A+B-1.0;
	double ZdenomInv = 1.0/denom;
	double AlogAlpha = A*log(alpha);
	double BlogBeta = B*log(beta);

	*D = -ZdenomInv;
	*C = 1.0+ZdenomInv;			// Same as (A+B)/(A+B-1) = (A+B-1+1)/(A+B-1).

	double deltaX = (1.0-funcValue)/(AlogAlpha*alphaExpX+BlogBeta*betaExpX);
	X += deltaX;
	if ( X<0.0 ) {
		// If undershot zero, restart Newton iteration at zero
		funcValue = A+B;		// Since alphaExpX and betaExpX equal 1
		deltaX = (1.0-funcValue)/(AlogAlpha+BlogBeta);
		X = deltaX;
	}
	else {
		deltaX = -deltaX;
	}
	while ( true ) {
		if ( deltaX<1.0e-12 ) {
			break;
		}
		alphaExpX = pow(alpha, X);
		betaExpX = pow(beta, X);
		funcValue = A*alphaExpX + B*betaExpX;
		deltaX = (1.0-funcValue)/(AlogAlpha*alphaExpX+BlogBeta*betaExpX);
		X += deltaX;
	}
	*exponent = X;

	return true;
}


//  Consider a function which satisfies the following properties:
//    1.   f(N) = 1 + A*f(alpha*N) + B*f(beta*N)
//           where A+B > 1 and alpha, beta in (0,1].
//    2    f(1) = 1.
//
//  These conditions are satisfied by setting f(x) = C N^exponent + D,
//    where C, exponent, D are the values returned by FindDoubleRecurseSoln

void FindDoubleRecurseSolnOld ( double A, double B, double alpha, double beta,
							 double* C, double* exponent, double* D )
{
	// Pre-conditions
	assert ( A+B>1.0 );
	assert ( 0.0<alpha && alpha<=1.0 );
	assert ( 0.0<beta && beta<=1.0 );

	double denom = A+B-1.0;
	double ZdenomInv = 1.0/denom;
	double AlogAlpha = A*log(alpha);
	double BlogBeta = B*log(beta);

	*D = -ZdenomInv;
	*C = 1.0+ZdenomInv;			// Same as (A+B)/(A+B-1) = (A+B-1+1)/(A+B-1).

	// Use Newton's method to find exponent
	double X = 0;				// Initial choice for exponent
	while ( true ) {
		double alphaExpX = pow(alpha, X);
		double betaExpX = pow(beta, X);
		double funcValue = A*alphaExpX + B*betaExpX;
		double deltaX = (1.0-funcValue)/(AlogAlpha*alphaExpX+BlogBeta*betaExpX);
		X += deltaX;
		if ( deltaX<1.0e-12 ) {
			break;
		}
	}
	*exponent = X;
}


