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


#include "BumpMapFunction.h"
#include "../VrMath/LinearR2.h"
#include "../Graphics/VisiblePoint.h"

// viewDir - unit vector giving direction from viewpoint

void BumpMapFunction::ApplyTexture(VisiblePoint& visPoint,
								   const VectorR3& viewDir ) const {

	VectorR2 deriv;				// Partial derivative function
	if ( DerivFunc != 0 ) {
		deriv = DerivFunc(visPoint.GetU(), visPoint.GetV());
	}
	else {
		assert( HeightFunc != 0 );		// At least one function must be given
		double u = visPoint.GetU();
		double v = visPoint.GetV();
		if ( u<0.0 || u>1.0 ) {
			switch (BorderType) {
			case BORDER_CLAMP:
				return;
			case BORDER_WRAP:
				u = u - floor(u);
				break;
			case BORDER_EXTEND:
				break;
			}
		}
		if ( v<0.0 || v>1.0 ) {
			switch (BorderType) {
			case BORDER_CLAMP:
				return;
			case BORDER_WRAP:
				v = v - floor(v);
				break;
			case BORDER_EXTEND:
				break;
			}
		}
		double uMinus = u-Epsilon;
		double uPlus = u+Epsilon;
		double vMinus = v-Epsilon;
		double vPlus = v+Epsilon;
		if ( BorderType!=BORDER_EXTEND) {
			ZeroBorderCheck( &uMinus );
			OneBorderCheck( &uPlus );
			ZeroBorderCheck( &vMinus );
			OneBorderCheck( &vPlus );
		}
		deriv.x = (HeightFunc(uPlus,v)-HeightFunc(uMinus,v))/(uPlus-uMinus);		
		deriv.y = (HeightFunc(u,vPlus)-HeightFunc(u,vMinus))/(vPlus-vMinus);		
	}

	VectorR3 retDerivU, retDerivV;
	bool partialsOK;
	partialsOK = visPoint.GetObject().CalcPartials( visPoint, retDerivU, retDerivV );
	if ( !partialsOK ) {	// If at a singularity
		return;			// Cannot do anything with information about the partials. 
	}

	const VectorR3& oldNormal = visPoint.GetNormal();
	retDerivU.AddScaled(oldNormal,deriv.x);
	retDerivV.AddScaled(oldNormal,deriv.y);
	retDerivU *= retDerivV;				// Cross product

	double newDotViewDir = (retDerivU^viewDir);		// Positive if facing away from viewer (new value)
	double oldDotViewDir = (oldNormal^viewDir);		// Positive if facing away from viewer (old value)
	if ( newDotViewDir*oldDotViewDir<0.0 ) {	
		// If switched direction relative to viewDir.
		retDerivU.AddScaled(viewDir,-newDotViewDir);	// Component perpindicular to viewDir
	}
	double norm = retDerivU.Norm();
	if ( norm!=0.0 ) {
		retDerivU /= norm;
		visPoint.SetNormal( retDerivU );
	}

	return;
}

void BumpMapFunction::ZeroBorderCheck( double *m ) const {
	if ( (*m)<0.0 ) {
		switch ( BorderType ) {
		case BORDER_CLAMP:
			*m = 0.0;
			break;
		case BORDER_WRAP:
			(*m) += 1.0;
			break;
		case BORDER_EXTEND:
			break;
		}
	}
}

void BumpMapFunction::OneBorderCheck( double *m ) const {
	if ( (*m) > 1.0 ) {
		switch ( BorderType ) {
		case BORDER_CLAMP:
			*m = 1.0;
			break;
		case BORDER_WRAP:
			(*m) -= 1.0;
			break;
		case BORDER_EXTEND:
			break;
		}
	}
}
