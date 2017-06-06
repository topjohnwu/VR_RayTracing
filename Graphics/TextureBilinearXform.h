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

#ifndef TEXTUREBILINEARXFORM_H
#define TEXTUREBILINEARXFORM_H

#include "TextureMapBase.h"
#include "../VrMath/LinearR2.h"

// TextureBilinearXform - Warps texture map coordinates bilinearly.

class TextureBilinearXform : public TextureMapBase {

public:
	TextureBilinearXform();

	void SetTextureCoords( double* );	// 4 pairs of doubles, giving texture coordinates
	void SetTextureCoords( float* );	// 4 pairs of float, giving texture coordinates
	
	void SetTextureCoordA ( double u, double v ) { TextureCoordA.Set(u,v); }
	void SetTextureCoordB ( double u, double v ) { TextureCoordB.Set(u,v); }
	void SetTextureCoordC ( double u, double v ) { TextureCoordC.Set(u,v); }
	void SetTextureCoordD ( double u, double v ) { TextureCoordD.Set(u,v); }

	void ApplyTexture( VisiblePoint& visPoint ) const;

private:
	VectorR2 TextureCoordA;		// Tex. coordinate for vertex A
	VectorR2 TextureCoordB;		// Tex. coordinate for vertex B
	VectorR2 TextureCoordC;		// Tex. coordinate for vertex C
	VectorR2 TextureCoordD;		// Tex. coordinate for vertex D
};

inline TextureBilinearXform::TextureBilinearXform() 
{
	TextureCoordA.Set( 0.0, 0.0 );
	TextureCoordB.Set( 1.0, 0.0 );
	TextureCoordC.Set( 1.0, 1.0 );
	TextureCoordD.Set( 0.0, 1.0 );
}

inline void TextureBilinearXform::SetTextureCoords( double* tc )	// 4 pairs of doubles, giving texture coordinates
{
	TextureCoordA.Load( tc );
	TextureCoordB.Load( tc+2 );
	TextureCoordC.Load( tc+4 );
	TextureCoordD.Load( tc+6 );
}

inline void TextureBilinearXform::SetTextureCoords( float* tc )	// 4 pairs of doubles, giving texture coordinates
{
	TextureCoordA.Load( tc );
	TextureCoordB.Load( tc+2 );
	TextureCoordC.Load( tc+4 );
	TextureCoordD.Load( tc+6 );
}



#endif //TEXTUREBILINEARXFORM_H
