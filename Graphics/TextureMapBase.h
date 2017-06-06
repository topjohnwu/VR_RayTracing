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

#ifndef TEXTUREMAPBASE_H
#define TEXTUREMAPBASE_H

#include <assert.h>
class VisiblePoint;
class VectorR2;
class VectorR3;

// TextureMapBase is purely virtual.  

class TextureMapBase {

public:

	// The section ApplyTexture routine must be written for every
	//  texture map class.  It must modify the VisiblePoint according
	//	to the texture map.  At least one of the following two routines
	//  *must* be written;


	virtual void ApplyTexture( VisiblePoint& visPoint ) const { assert(0); }
	virtual void ApplyTexture( VisiblePoint& visPoint, const VectorR3& viewDir ) const;

	// Convert a unit vector reflection direction to cube map coordinates
	static void TextureMapBase::ReflectDirToCubeMap( const VectorR3& reflectDir, 
										  VectorR2* cubeMapCoords );

protected:

};

inline void TextureMapBase::ApplyTexture( VisiblePoint& visPoint, const VectorR3& viewDir ) const
{
	ApplyTexture( visPoint );	// Default implementation
} 

#endif // TEXTUREMAPBASE_H
