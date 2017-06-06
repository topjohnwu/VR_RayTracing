/*
 *
 * RayTrace Software Package, release 3.2.  May 3, 2007.
 *		Graphics subpackage
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

#include "assert.h"
#include "PixelArray.h"
#include "RgbImage.h"

// SetSize(width, height) resizes the pixel data info.
// If necessary, it allocates new block of memory.
// Returns true if new memory has been allocated.
// Returns false if the memory location (and content) is unchanged
bool PixelArray::SetSize( int width, int height ) {
	bool retValue = false;
	long widthAlloc =  width;
	long newAlloc = ((long)widthAlloc)*((long)height)*3;
	if ( newAlloc>Allocated ) {
		delete[] ColorValues;
		Allocated = newAlloc;
		ColorValues = new float[Allocated];
		assert(ColorValues != 0);	
		retValue = true;
	}
	WidthAlloc = widthAlloc;
	Width = width;
	Height = height;
	return retValue;
}

// Set the size to the size of the viewport.
void PixelArray::ResetSize() {
	GLint got[4];		// i,j, width, height
	glGetIntegerv( GL_VIEWPORT, got );
	SetSize(got[2],got[3]);
}

// DrawFloats() writes the contents of the pixel array into
//	OpenGL's buffer.  The floating point numbers are written
//  directly in. 
// Please note that same ATI graphics board drivers have software
//  bugs in that they do not clamp color values to the range [0,1],
//  and this will cause bad results on those graphics boards.

void PixelArray::DrawFloats() const
{
	glPixelStorei( GL_UNPACK_ROW_LENGTH, Width );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
	glRasterPos2i(0,0);		// Position at base of window
	glDrawPixels( Width, Height, GL_RGB, GL_FLOAT, ColorValues );
}

// DrawViaRgbImage() writes the contents of the pixel array into
//	OpenGL's buffer.  The floating point numbers are first converted
//  to unsigned integers (via an RgbImage).
// This avoids the graphics board software bugs mentioned
//  in the comments for DrawFloats();  However, some ATI
//  graphics boards have another software bug (concerning
//  row length) that makes this method give a skewed image.

void PixelArray::DrawViaRgbImage() const
{
	glRasterPos2i(0,0);		// Position at base of window
	RgbImage image( GetHeight(), GetWidth() );
	Dump( image );
	image.DrawToOpenglBuffer();
}

// ClampAndDrawFloats() first clamps all values to the 
//	interval [0,1] and then draws them.  The whole point
//  is to avoid some software issues for some graphics
//  boards (particularly, ATI boards).

void PixelArray::ClampAndDrawFloats()
{
	ClampAllValues();							// Clamp values to range [0,1]
	DrawFloats();
}

// Dumps the PixelArray data into an RgbImage object.
//   The RgbImage data (for now at least) must match the
//	 size of the PixelArray dimensions.

void PixelArray::Dump( RgbImage& image ) const
{
	assert ( image.GetNumCols()==GetWidth() && image.GetNumRows()==GetHeight() );
	for ( long i=0; i<GetHeight(); i++ ) {
		for ( long j=0; j<GetWidth(); j++ ) {
			const float *color = GetPixel(j,i);
			image.SetRgbPixelf( i, j, *color, *(color+1), *(color+2) );
		}
	}
}

// Usually, the filename ends with the suffix ".bmp" (but it is not provided)
void PixelArray::DumpBmp( const char* filename ) const
{
	RgbImage image( GetHeight(), GetWidth() );
	Dump( image );
	image.WriteBmpFile( filename );
}


// Clamp all color components to be in the range [0,1]

void PixelArray::ClampAllValues()
{
	float* pixelPtr = ColorValues;
	long iterCount = 3*GetHeight()*GetWidth();
	for ( long i=0; i<iterCount; i++ ) {
		ClampRange( pixelPtr++, (float)0.0f, (float)1.0f );		// Clamp value to range [0,1]
	}
}
