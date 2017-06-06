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

#include "TextureCheckered.h"
#include "ViewableBase.h"

bool TextureCheckered::InOddSquare(double u, double v ) const	// Compute even/odd ness
{
	bool ans = false;
	if ( u<0.0 ) {
		ans = true;
		u = -u;
	}
	ans ^= (((long)(u/uWidth))&0x01);
	if ( v<0.0 ) {
		ans = !ans;
		v = -v;
	}
	ans ^= (((long)(v/vWidth))&0x01);
	return ans;
}

void TextureCheckered::ApplyTexture( 
					VisiblePoint& visPoint ) const
{
	if ( InOddSquare( visPoint.GetU(), visPoint.GetV() ) ) {
		if ( Material1 ) {
			visPoint.SetMaterial(*Material1);
		}
	}
	else if ( Material2 ) {
		visPoint.SetMaterial( *Material2 );
	}
}
