/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006
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

#include "TextureBilinearXform.h"
#include "../Graphics/VisiblePoint.h"

void TextureBilinearXform::ApplyTexture( VisiblePoint& visPoint ) const
{
	double uCoord = visPoint.GetU();
	double vCoord = visPoint.GetV();
	visPoint.SetUV( (1.0-uCoord)*((1.0-vCoord)*TextureCoordA
										+vCoord*TextureCoordD)
						+ uCoord*((1.0-vCoord)*TextureCoordB
									+vCoord*TextureCoordC));
	return;
}
