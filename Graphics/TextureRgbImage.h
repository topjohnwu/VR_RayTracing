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

#ifndef TEXTURERGBIMAGE_H
#define TEXTURERGBIMAGE_H

#include "TextureMapBase.h"
#include "RgbImage.h"
#include "../VrMath/LinearR2.h"
#include "../VrMath/LinearR3.h"

// TextureRgbImage makes a texture map out of an RGB image.  
// Uses bilinear interpolation to set colors (by default)
// Wraps around by default.

class TextureRgbImage : public TextureMapBase {

public:
	TextureRgbImage();
	TextureRgbImage( const RgbImage& );
	TextureRgbImage( const char* filename );
	virtual ~TextureRgbImage();

	const RgbImage& GetRgbImage() const { return *TextureImage; }
	bool TextureMapLoaded() const { return RgbImageLoadedFromFile; }
	void FreeRgbImage() { RgbImageLoadedFromFile = false; delete TextureImage; }

	void UseBilinearInterp( bool status );		// controls whether bilinear interpolate
	void SetWrapMode( int mode );				// Mode should be WrapUV or ClampUV
	void SetWrapMode( const VectorR3& color );	// Sets BackgroundColorMode
	void SetWrapMode( double* );						// Ditto
	void SetWrapMode( float* );							// Ditto
	void SetWrapMode( double r, double g, double b );	// Ditto

	enum {
		WrapUV = 0,
		ClampUV = 1,
		BackgroundColorMode = 2
	};

	// ApplyTexture( visPoint ) merely "paints" in the color
	//    The color becomes the ambient/diffuse color and is applied before
	//	  lighting calculuations.
	void ApplyTexture( VisiblePoint& visPoint ) const;

	void GetTextureColor( const VectorR2& uvCoords, VectorR3* retColor ) const;  
	void GetTextureColor( double u, double v, VectorR3 *retColor ) const;

private:
	const RgbImage* TextureImage;	// Pointer to the RgbImage
	bool RgbImageLoadedFromFile;	// true if loaded from a file.

	bool UseBilinearFlag;			// if false, then just use closest pixel

	int WrapMode;
	VectorR3 BackgroundColor;		// Color used in BackgroundColorMode
};

inline TextureRgbImage::TextureRgbImage()
{
	TextureImage = 0;
	WrapMode = WrapUV;
	UseBilinearFlag = true;
	RgbImageLoadedFromFile = false;
}

inline TextureRgbImage::TextureRgbImage( const RgbImage& img ) 
{
	TextureImage = &img;
	WrapMode = WrapUV;
	UseBilinearFlag = true;
	RgbImageLoadedFromFile = false;
}

inline TextureRgbImage::TextureRgbImage( const char* filename ) 
{
	WrapMode = WrapUV;
	UseBilinearFlag = true;
	RgbImageLoadedFromFile = true;
	TextureImage = new RgbImage( filename );
	if ( TextureImage->GetErrorCode() ) {
		// Failed to open file!
		TextureImage = 0;
		RgbImageLoadedFromFile = false;
	}
}

inline TextureRgbImage::~TextureRgbImage()
{
	if ( RgbImageLoadedFromFile ) {
		delete TextureImage;
	}
}

inline void TextureRgbImage::UseBilinearInterp( bool status )
{
	UseBilinearFlag = status;
}

inline void TextureRgbImage::SetWrapMode( int mode ) 
{
	assert ( mode==WrapUV || mode==ClampUV );
	WrapMode = mode;
}

inline void TextureRgbImage::SetWrapMode( const VectorR3& color )
{
	WrapMode = BackgroundColorMode;
	BackgroundColor = color;
}

inline void TextureRgbImage::SetWrapMode( double* color )
{ 
	WrapMode = BackgroundColorMode;
	BackgroundColor.Load( color );
}

inline void TextureRgbImage::SetWrapMode( float* color )
{ 
	WrapMode = BackgroundColorMode;
	BackgroundColor.Load( color );
}

inline void TextureRgbImage::SetWrapMode( double r, double g, double b )
{ 
	WrapMode = BackgroundColorMode;
	BackgroundColor.Set( r, g, b );
}

inline void TextureRgbImage::GetTextureColor( const VectorR2& uvCoords, VectorR3* retColor ) const
{
	GetTextureColor( uvCoords.x, uvCoords.y, retColor );
}

#endif // TEXTURERGBIMAGE_H
