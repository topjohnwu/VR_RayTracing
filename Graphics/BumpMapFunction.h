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

#ifndef BUMPMAPFUNCTION_H
#define BUMPMAPFUNCTION_H

#include "TextureMapBase.h"
#include "ViewableBase.h"

class VectorR2;

// Implements bump mapping with user-supplied functions which compute
//   either the height field or the directional derivatives.

class BumpMapFunction : public TextureMapBase {

public:
	BumpMapFunction();
	BumpMapFunction( double f(double u, double v) );		// Height (displacement) function
	BumpMapFunction( VectorR2 f(double u, double v) );	// Derivatives function

	// You should set only one of these.  Or you can use one of the constuctors.
	void SetHeightFunction ( double f(double u, double v) );
	void SetDerivativesFunction( VectorR2 f(double u, double v) );

	// The following are somewhat specialized: they set options on the way
	//	height function is sampled to estimate the partial derivatives.
	// The height function always uses the interval [0,1].
	// epsilon and "border type" apply ONLY to the height function case!
	void SetEpsilon( double epsilon );		// Step size (epsilon) to use for differencing
	void SetBorderType( int borderType );	// Clamp, extend or wrap.

	enum {
		BORDER_EXTEND = 0,			// Extend height function arguments past border (Default)
		BORDER_CLAMP = 1,			// Clamp height function arguments at border
		BORDER_WRAP = 2 			// Wrap height functions around.
	};
	static const double DefaultEpsilon() { return 0.001; }

	void ApplyTexture( VisiblePoint& visPoint, const VectorR3& viewDir ) const;

private:
	double (*HeightFunc)(double u, double v);		// Displacement (height) function
	VectorR2 (*DerivFunc)(double u, double v);	// Partial derivative (gradient) function
	int BorderType;
	double Epsilon;

	void ZeroBorderCheck( double *m ) const;
	void OneBorderCheck( double *p ) const;
};

inline BumpMapFunction::BumpMapFunction() {
	HeightFunc = 0;
	DerivFunc = 0;
	BorderType = BORDER_EXTEND;
	Epsilon = DefaultEpsilon();
}

inline BumpMapFunction::BumpMapFunction( double f(double u, double v) ) {
	HeightFunc = f;
	DerivFunc = 0;
	BorderType = BORDER_EXTEND;
	Epsilon = DefaultEpsilon();
}

inline BumpMapFunction::BumpMapFunction( VectorR2 f(double u, double v) ) {
	HeightFunc = 0;
	DerivFunc = f;
	BorderType = BORDER_EXTEND;
	Epsilon = DefaultEpsilon();
}

inline void BumpMapFunction::SetHeightFunction( double f(double u, double v) ) {
	HeightFunc = f;
	BorderType = BORDER_EXTEND;
	Epsilon = DefaultEpsilon();
}

inline void BumpMapFunction::SetDerivativesFunction( VectorR2 f(double u, double v) ) {
	DerivFunc = f;
	BorderType = BORDER_EXTEND;
	Epsilon = DefaultEpsilon();
}

inline void BumpMapFunction::SetEpsilon( double epsilon )
{
	Epsilon = epsilon;
}

inline void BumpMapFunction::SetBorderType( int borderType )
{
	BorderType = borderType;
}

#endif // BUMPMAPFUNCTION_H
