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

#include "PolynomialRC.h"
#include "MathMisc.h"
#include <math.h>
#include <assert.h>

// QuadraticSolveReal: solves degree to polynomial 
//			a*x^2 + b*x + c  = 0
//		with real coefficients over the reals.  Returns the
//		number of real roots as 0, 1, 2, 3.
//		The return code 3 indicates that a=b=c=0 so all reals
//			are roots.
//		In the case of one root, it is returned as root1 (only).
//		In the case of two roots, root1 will be the smaller root.
//			and root2 is the larger root.

int QuadraticSolveReal( double a, double b, double c,
					    double *root1, double *root2)
{
	if ( a==0.0 ) {
		if ( b!=0.0 ) {
			*root1 = c/b;
			return 1;
		}
		else if ( c==0.0 ) {
			return 3;
		}
		else {
			return 0;	// Constant (deg. zero) and non-zero.
		}
	}

	if ( b==0.0 ) {
		if ( c==0.0 ) {
			*root1 = 0.0;
			return 1;
		}
		else { 
			*root1 = sqrt(c/b);
			if ( *root1 != 0.0 ) {
				*root2 = -(*root1);
				return 2;
			}
			else {
				return 1;
			}
		}
	}

	// Now we know a and b are both nonzero.
	double dscrim = b*b - 4.0*a*c;
	if ( dscrim<0.0) {
		return 0;
	}
	return QuadraticSolveRealDescrimPos( a, b, c, dscrim, root1, root2 );
}

int QuadraticSolveRealDescrimPos( double a, double b, double c, 
						double dscrim, double *root1, double *root2)
{
	dscrim = sqrt(dscrim);		// Now the square root of descriminant
	
	if ( b<0 ) {
		dscrim = -dscrim;
	}
	double D = b+dscrim;
	*root1 = -D/(2.0*a);
	*root2 = -(2.0*c)/D;

	if ( *root1==*root2 ) {
		return 1;
	}
	
	if ( *root1 > *root2 ) {
		double temp = *root1;
		*root1 = *root2;
		*root2 = temp;
	}
	return 2;
}

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
								   double* soln2X, double* soln2Y )
{
	double descrim = b*b - a*c;		// Descriminant (or negative of determinant).
	if ( descrim<0.0 ) {
		*soln1X = -(b+c);			// Use the place where the poly achieves its maximum.
		*soln1Y = (a+b);			// These values can be used if descrim is close to zero.
		return 0;
	}
	else if ( descrim==0.0 ) {
		if ( a!=0.0 ) {
			*soln1X = -2.0*b;
			*soln1Y = a;
			return 1;
		}
		else if ( c!=0.0 ) {
			*soln1X = c;
			*soln1Y = -2.0*b;			// b should always equal zero here (within limits of precision)
			return 1;
		}
		else {
			return 3;					// In this case a==b==c==0.0;
		}
	}
	else {							// if descrim>0.0
		descrim = sqrt(descrim);		// Now square root of descriminant
		*soln1Y = a;
		*soln2X = c;
		if ( b<0.0 ) {
			*soln1X = *soln2Y = -b + descrim;
		}
		else {
			*soln1X = *soln2Y = -b - descrim;
		}
		return 2;
	}
}

int CubicSolveReal( double a, double b, double c, double d,
				   double* rootlist )
{
	double coefs[4]={a,b,c,d};
	return PolySolveReal(3,coefs,rootlist);
}


// Finds all the real roots of a polynomial of given degree.
int PolySolveReal( int degree, double *coefs, double *roots )
{
	// Find out true degree (skip leading zeros)
	int d;
	for (d=degree; d>=0; d--) {
		if ( *coefs != 0.0 ) {
			break;
		}
		coefs++;
	}

	// Now d is the degree and *coefs is the leading coefficient

	if ( d==-1 ) {
		return -1;		// constant zero polynomial
	}
	else if ( d==0 ) {
		return 0;		// constant non-zero polynomial
	}
	else if ( d==1 ) {
		*roots = (*(coefs+1))/(*coefs);
		return 1;
	}
	else if ( d==2 ) {
		int i;
		i = QuadraticSolveReal( *coefs, *(coefs+1), *(coefs+2), roots);
		if ( i==1 ) {
			*(roots+1) = *roots;		
			return 2;				// Double root
		}
		else {
			return i;
		}
	}
	else {
		double* work = new double[(d+2)*(d+1)];
		double* rootswork = work + (d+1)*(d+2)/2;
		// The array work has space (d+2)*d+1)/2 reserved for coefficients
		// The rest, of size (d=1)*2/ reserved for roots.

		// Load the first part of the work array with the coef's
		double* tptr = work;
		double* fptr = coefs;
		int i;
		for (i=degree; i>=0; i--) {
			(*(tptr++)) = *(fptr++);
		}

		// Call the recursive solver.
		int numRoots = PolySolveRealAll( d, work, rootswork );

		// Copy the answer out of rootswork
		tptr = roots;
		fptr = rootswork;
		for (i=0; i<numRoots; i++) {
			(*(tptr++)) = *(fptr++);
		}

		delete work;
		return numRoots;
	}
}

double EvalPoly_POLYRC( int degree, double* coefs, double x )
{
	double result;
	if ( x == DBL_MAX ) {
		result = ( (!(degree&1)) || *coefs>0.0) ? DBL_MAX : -DBL_MAX;
		return result;
	}
	else if ( x == -DBL_MAX ) {
		result = ( (!(degree&1)) || *coefs<0.0) ? DBL_MAX : -DBL_MAX;
		return result;
	}

	int i;
	result = *coefs;
	for (i=0; i<degree; i++ ) {
		result *= x;
		coefs++;
		result += *coefs;
	}
	return result;
}

//  This version returns a "scale" value which corresponds to the size of
//		intermediate results
double EvalPolyScale_POLYRC( int degree, double* coefs, double x, double *scale )
{
	double result;
	if ( x == DBL_MAX ) {
		result = ( (!(degree&1)) || *coefs>0.0) ? DBL_MAX : -DBL_MAX;
		return result;
	}
	else if ( x == -DBL_MAX ) {
		result = ( (!(degree&1)) || *coefs<0.0) ? DBL_MAX : -DBL_MAX;
		return result;
	}

	int i;
	result = *coefs;
	*scale = 0.0;
	for (i=0; i<degree; i++ ) {
		result *= x;
		coefs++;
		UpdateMaxAbs( scale, result );
		result += *coefs;
	}
	UpdateMaxAbs( scale, result );
	
	return result;
}


// Solve via Newton-Raphaelson iteration
//    This routine will assume that the iteration is being done
//		*safely* on a convex or concave interval.
double NewtonSolve_POLYRC( int degree, double* coefs, double startpos, double startval )
{
	double curEst;
	double lastEst = startpos;
	double lastVal = startval;
	double scale = 1.0;
	int lastSign;
	
	int i = 0;
	bool closeFlag = false;
	double Fprime = EvalPolyScale_POLYRC(degree-1,coefs+(degree+1),lastEst, &scale);
	while ( ++i ) {
		if ( Fprime==0.0 ) {
			assert ( fabs(lastVal)<1.0e-14*scale );
			curEst = lastEst;
			break;
		}
		double delta = lastVal/Fprime;
		curEst = lastEst-delta;
		if ( fabs(delta)<1.0e-13*fabs(lastEst) 
				|| ( fabs(curEst)<1.0e-80 && fabs(lastEst)<1.0e-80 )
				|| ( fabs(lastVal)<1.0e-13*scale ) )   {
			if (closeFlag) {
				break;
			}
			else {
				closeFlag = true;
			}
		}
		int curSign = Sign(delta);
		if ( i>3 && curSign!=lastSign ) {
			break;
		}
		lastSign = curSign;
		lastEst = curEst;
		lastVal = EvalPolyScale_POLYRC(degree,coefs,lastEst,&scale);
		if ( lastVal==0.0 ) {
			break;
		}
		Fprime = EvalPoly_POLYRC(degree-1,coefs+(degree+1),lastEst);
		assert ( i<40 );
	}
	return curEst;

}


// The recursive algorithm for finding all roots of a polynomial p(x).
//	It works by forming its first derivative p'(x), and recursively
//	finding its real roots.  Any real root of the first polynomial p(x)
//  will be bracketed by real roots of p'(x).  We then use Newton iteration
//  to find a root.  The newton iteration is started from a root of p''(x)
//  so as to guarantee convergence to a desired root.
// Returns the number of roots found, including repeated roots in the count.
//	Repeated roots are listed multiple times in the root array.
// This routine is called *only* with the leading coefficient non-zero.

int PolySolveRealAll( int degree, double* coefsarray, double* rootsarray) 
{
	int i;

	assert( degree>=1 );			// Degree must be at least one
	assert( 0.0 != *coefsarray);	// Leading coef should not be zero

	// Special case for degree 1
	if (degree==1) {
		*rootsarray = -(*(coefsarray+1))/(*coefsarray);
		*(rootsarray+1) = DBL_MAX;		// Terminates root list
		return 1;
	}

	// Calculate the coefs of the derivative p'(x)
	double* fromptr= coefsarray;
	double* toptr = coefsarray+(degree+1);
	for ( i=degree; i>0; i--) {
		*(toptr++) = i*(*(fromptr++));
	}
	int numRootsDeriv;
	numRootsDeriv = PolySolveRealAll( degree-1, 
										coefsarray+(degree+1), 
										rootsarray+(degree+1) );

	// Special case for degree 2
	//	The results of the p'(x) recursive call are still needed however.
	if (degree==2) {
		i = QuadraticSolveReal( *coefsarray,*(coefsarray+1), *(coefsarray+2),
							rootsarray, (rootsarray+1) );
		if (i==0) {
			*rootsarray = DBL_MAX;		// Terminates root list
			return 0;
		}
		else {
			if ( i==1 ) {
				*(rootsarray+1) = *rootsarray;	// Double root
			}
			*(rootsarray+2) = DBL_MAX;		// Terminates root list
			assert( LessOrEqualFuzzy(*rootsarray,*(rootsarray+3)) );
			assert( LessOrEqualFuzzy(*(rootsarray+3),*(rootsarray+1)) );
			return 2;
		}
	}

	// Handle the general case, degree > 2.
	int numRootsFound = 0;
	double* pPtr = rootsarray;
	double* p1Ptr = rootsarray+(degree+1);		// List of roots of p'(x)
	double* p2Ptr = p1Ptr + degree;				// List of roots of p''(x)
	int lastSign, curSign;
	double curVal;
	double last1Root = -DBL_MAX;
	double last2Root = -DBL_MAX;
	double cur1Root;
	int p1Counter = 0;		// Number of roots of p'(x) processed

	if ( degree&1 ) {	// If odd degree
		lastSign = (*coefsarray)>0.0 ? -1 : 1;	// Sign at -infinity
	}
	else {
		lastSign = 1;					// Positive at -infinity
	}
	bool concaveUp = (lastSign==1);		// Concavity at -infinity
										// True when concave up before current p'' root

	// Loop thru all zeros of p'(x)
	p1Counter = 0;
	while ( p1Counter<=numRootsDeriv ) {
	
		// cur1Root is zero of p'(x).   curVal = p(cur1Root).
		cur1Root = *p1Ptr;		// Lower bound on sought for root
		curVal = EvalPoly_POLYRC(degree, coefsarray, cur1Root);
		curSign = Sign(curVal);

		if ( curSign==0 ) {			// Got a double root (at least)
			*pPtr = cur1Root;
			pPtr++;
		}
		else if ( lastSign==0 ) {	// Last occurence of a multiple root
			*pPtr = last1Root;
			pPtr++;
		}
		else if ( lastSign!=curSign ) {
			double minRoot = last1Root;
			int cur2Sign = lastSign;		// Start with sign at curVal
			double cur2Val, last2Val;
			bool last2ValValid = false;
			while ( (*p2Ptr)<(*p1Ptr) ) {
				assert(*p2Ptr != DBL_MAX );
				cur2Val = EvalPoly_POLYRC(degree, coefsarray, *p2Ptr );
				cur2Sign = Sign(cur2Val);
				if ( cur2Sign == curSign ) {
					break;
				}
				minRoot = *p2Ptr;	// Looking for a root at least this large
				last2Root = *p2Ptr;
				last2Val = cur2Val;
				last2ValValid = true;
				p2Ptr++;
				concaveUp = !concaveUp;
			}
			assert( cur2Sign == curSign || (*p2Ptr)>=(*p1Ptr) );
			double startX;		// Value to start Newton iteration
			double startF;		// Function value at startX
			if ( concaveUp==(curSign==1) ) {
				if ( (*p2Ptr)<(*p1Ptr) ) {
					startX = *p2Ptr;
					startF = cur2Val;
				}
				else {
					assert( *p2Ptr==DBL_MAX );
					if ( last2Root > last1Root ) {
						startX = last2Root+1.0;
					}
					else {
						startX = last1Root+1.0;
					}
					startF = EvalPoly_POLYRC(degree, coefsarray, startX );
				}
			}
			else if ( last2Root > -DBL_MAX ) { 
				startX = last2Root;
				if ( last2ValValid ) {
					startF = last2Val;
				}
				else {
					startF = EvalPoly_POLYRC(degree, coefsarray, last2Root );
				}
			}
			else {
				startX = Min(cur1Root,*p2Ptr)-1;	// Before first root of p'(x) too
				startF = EvalPoly_POLYRC(degree, coefsarray, startX);
				assert( -DBL_MAX<startX && startX<DBL_MAX );
			}
			*pPtr = NewtonSolve_POLYRC(degree, coefsarray, startX, startF);
			// NEXT LINES FOR DEBUG PURPOSES ONLY
			//double scale;
			//double test=EvalPolyScale_POLYRC(degree,coefsarray,*pPtr,&scale);
			//if ( !(fabs(test)<1.0e-10*scale) ) {
			//	NewtonSolve_POLYRC(degree, coefsarray, startX, startF);
			//}
			//assert( fabs(test)<1.0e-10*scale );
			//assert( GreaterOrEqualFuzzy(*pPtr, minRoot) );
			//assert( LessOrEqualFuzzy(*pPtr, Min(cur1Root,*p2Ptr) ) );
			if ( *pPtr < minRoot ) {
				*pPtr = minRoot;		// Should happen only from roundoff error
			}
			pPtr++;
			numRootsFound++;
		}

		// Bump up roots in p''(x)
		while ( *p2Ptr < *p1Ptr ) {
			last2Root = *p2Ptr;
			p2Ptr++;
			concaveUp = !concaveUp;
		}
		// Set the next "last" values
		last1Root = cur1Root;
		lastSign = curSign;
		p1Ptr++;
		p1Counter++;
	}
	
	*pPtr = DBL_MAX;			// Terminate the list of roots
	return numRootsFound;
}
