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

#ifndef POLYNOMIALRC_H
#define POLYNOMIALRC_H

#include <iostream>
#include "MathMisc.h"
using namespace std;

//
// Routines for polynomials over reals/complexes
//

// QuadraticSolveReal: solves degree to polynomial with
//		real coefficients over the reals.  Returns the
//		number of real roots (0,1,2; or 3 if a=b=c=0).
//		root1 will be the smaller root.
// The "Safe" versions should be used only if the leading coefficient, a,
//		is known to be non-zero.
int QuadraticSolveReal( double a, double b, double c,
					    double *root1, double *root2);
int QuadraticSolveReal( double a, double b, double c,
					    double* rootlist);
int QuadraticSolveRealSafe( double a, double b, double c,
					    double *root1, double *root2);
int QuadraticSolveRealSafe( double a, double b, double c,
					    double* rootlist);

int CubicSolveReal( double a, double b, double c, double d,
				    double* rootlist );

// Finds all the real roots of a polynomial of given degree.
int PolySolveReal( int degree, double *coefs, double *roots);

// Intended only for internal use:
int QuadraticSolveRealDescrimPos( double a, double b, double c, 
						double descrim, double *root1, double *root2);
int PolySolveRealAll( int degree, double *coefsarray, double* rootsarray );

//
// Routines for 2-variable homogeneous polynomials over the reals.
//
// Solves homogeneous equation.
// Solves an equation    a x^2 + (2b)xy + c y^2, returning two
//	homogenous vectors describing all solutions.  Also returns
//  the number of distinct solutions (0,1,2) or returns 3 if
//  all vectors are solutions (the latter case occurs only if
//	a, b, and c are all zero.
// Note the factor "2" is applied to the second coefficient!!
// This is the same as the matrix equation (x y) X (x y)^T,
//  where X is the matrix ( a b )
//	                      ( b c ).
// In the general case, solutions all have the form
//  (alpha)(soln1A, soln1Y) and (alpha)(soln2X, soln2Y),
//  where alpha can be any constant.
// Special case: if the determinant is non-zero, it still 
//	returns values for x and y.  Namely the values where the
//	value of the quadratic polynomial is maximized.
int QuadraticSolveHomogeneousReal( double a, double b, double c,
								   double* soln1X, double* soln1Y,
								   double* soln2X, double* soln2Y );

/*****************************************************/
/*    Inlined routines								 */
/*****************************************************/

inline int QuadraticSolveReal( double a, double b, double c,
							   double* rootlist)
{
	return QuadraticSolveReal(a, b, c, rootlist, rootlist+1);
}

inline int QuadraticSolveRealSafe( double a, double b, double c,
							   double* rootlist)
{
	return QuadraticSolveRealSafe(a, b, c, rootlist, rootlist+1);
}

inline int QuadraticSolveRealSafe( double a, double b, double c,
							   double* root1, double *root2)
{
	double descrim = b*b-4.0*a*c;
	if ( descrim<0.0 ) {
		return 0;
	}
	return QuadraticSolveRealDescrimPos( a, b, c, descrim, 
										 root1, root2 );
}

#endif //POLYNOMIALRC_H
