/*
 *
 * RayTrace Software Package, release 3.2.  May 3, 2007.
 *		Graphics subpackage.
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
 * Thanks to Joey Hammer for a bug report that affected wrapping coordinates.
 *
 */


#include "TextureRgbImage.h"
#include "VisiblePoint.h"
#include "MaterialBase.h"
#include "../VrMath/LinearR3.h"

// TextureRgbImage makes a texture map out of an RGB image.  
// Uses bilinear interpolation to set colors (by default)
// Wraps around by default.

void TextureRgbImage::ApplyTexture( VisiblePoint& visPoint ) const
{
	assert( TextureImage );			// If this assert happens, there was probably a file open error.

	if ( TextureImage->ImageLoaded() ) {
		const VectorR2& uv = visPoint.GetUV();
		VectorR3 color;
		GetTextureColor(uv.x, uv.y, &color);
		visPoint.MakeMaterialMutable();
		visPoint.GetMaterialMutable().SetColorAmbientDiffuse(color);
	}
}

void TextureRgbImage::GetTextureColor( double u, double v, VectorR3 *retColor ) const
{
	switch( WrapMode ) {
	case WrapUV:
		if ( u<0.0 || u>1.0 ) {
			u = u-floor(u);
		}
		if ( v<0.0 || v>1.0 ) {
			v = v-floor(v);
		}
		break;
	case ClampUV:
		ClampRange( &u, 0.0, 1.0 );
		ClampRange( &v, 0.0, 1.0 );
		break;
	case BackgroundColorMode:
		if ( u<0.0 || u>1.0 || v<0.0 || v>1.0 ) {
			*retColor = BackgroundColor;
			return;
		}
		break;
	}

	double s = TextureImage->GetNumRows();
	double r = TextureImage->GetNumCols();

	if ( UseBilinearFlag ) {
		long iLo, iHi;
		double alpha;
		if ( r==1.0 ) {
			iLo = iHi = 0;
			alpha = 0.0;
		}
		else {
			r -= 1.0;
			double temp = floor(u*r);
			iLo = (long)temp;
			ClampMax<long>( &iLo, TextureImage->GetNumCols()-2 );
			iHi = iLo + 1;
			alpha = u*r - temp;
		}
		long jLo, jHi;
		double beta;
		if ( s==1.0 ) {
			jLo = jHi = 0;
			beta = 0.0;
		}
		else {
			s -= 1.0;
			double temp = floor(v*s);
			jLo = (long)temp;
			ClampMax<long>( &jLo, TextureImage->GetNumRows()-2 );
			jHi = jLo + 1;
			beta = v*s - temp;
		}

		VectorR3 wk;
		TextureImage->GetRgbPixel( jLo, iLo, &(wk.x), &(wk.y), &(wk.z) );
		wk *= (1.0-alpha)*(1.0-beta);
		*retColor = wk;
		TextureImage->GetRgbPixel( jHi, iLo, &(wk.x), &(wk.y), &(wk.z) );
		wk *= (1.0-alpha)*beta;
		*retColor += wk;
		TextureImage->GetRgbPixel( jHi, iHi, &(wk.x), &(wk.y), &(wk.z) );
		wk *= alpha*beta;
		*retColor += wk;
		TextureImage->GetRgbPixel( jLo, iHi, &(wk.x), &(wk.y), &(wk.z) );
		wk *= alpha*(1.0-beta);
		*retColor += wk;
	}
	else {
		// Just get closest pixel
		double temp = floor(u*r);
		long i = (long)temp;
		temp = floor(v*s);
		long j = (long)temp;
		ClampRange<long>( &i, 0, TextureImage->GetNumCols()-1 );	// Just in case (e.g. u=1)
		ClampRange<long>( &j, 0, TextureImage->GetNumRows()-1 );
		TextureImage->GetRgbPixel(j,i, &(retColor->x), &(retColor->y), &(retColor->z) );
	}
	return;
}
