/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006.
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

#ifndef PIXELARRAY_H
#define PIXELARRAY_H

// If you do not have GLUT installed, you can use the basic GL routines instead.
//   For this, include windows.h and GL/gl.h, instead of GL/glut.h
//#include <windows.h>
//#include <GL/gl.h>	// Basic OpenGL includes
// Including stdlib.h and disabling the atexit_hack seem to work everywhere.
//	Eventually there should be a new version of glut.h that doesn't need this.
#include <stdlib.h>
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>	// GLUT OpenGL includes


#include "../VrMath/LinearR3.h"
#include "../VrMath/LinearR4.h"
#include "../VrMath/MathMisc.h"
class RgbImage;

class PixelArray {

public:
	PixelArray();
	PixelArray( int width, int height );
	~PixelArray();

	void ResetSize();
	bool SetSize( int width, int height );

	// Set a single pixel color  -- i indexes left to right, j top to bottom
	void SetPixel( int i, int j, const float* color );
	void SetPixel( int i, int j, const double* color );
	void SetPixel( int i, int j, const VectorR4 color );
	void SetPixel( int i, int j, const VectorR3 color );

	// Draw into the OpenGL draw buffer
	// Any of the three methods could be used, but ClampAndDrawFloats
	//	works best at circumventing bugs in graphics board drivers.
	void Draw() { ClampAndDrawFloats(); }
	void ClampAndDrawFloats();
	void DrawFloats() const;
	void DrawViaRgbImage() const;

	// Write out to a RgbImage  or to a BITMAP (.bmp) file.
	void Dump( RgbImage& image ) const;
	void DumpBmp( const char* filename ) const;

	long GetWidth() const { return Width; }
	long GetHeight() const { return Height; }

	void GetPixel( int i, int j, double* rgb ) const;
	void GetPixel( int i, int j, float* rgb ) const;
	const float* GetPixel( int i, int j ) const;

	// Clamps all values to [0,1]
	void ClampAllValues();

protected:
	long Allocated;
	long Width, Height;
	long WidthAlloc;
	float *ColorValues;

};

inline PixelArray::PixelArray() 
{ 
	ColorValues = 0;
	Allocated = 0;
	ResetSize(); 
}
inline PixelArray::PixelArray( int width, int height )
{
	ColorValues = 0;
	Allocated = 0; 
	SetSize(width,height); 
}

inline PixelArray::~PixelArray() 
{
	delete[] ColorValues;
}

inline void PixelArray::SetPixel( int i, int j, const float* color )
{
	float* cptr = const_cast<float*>(GetPixel(i,j));
	*(cptr++) = *(color++);
	*(cptr++) = *(color++);
	*(cptr) = *(color);
}

inline void PixelArray::SetPixel( int i, int j, const double* color )
{
	float* cptr = const_cast<float*>(GetPixel(i,j));
	*(cptr++) = (float)(*(color++));
	*(cptr++) = (float)(*(color++));
	*(cptr) = (float)(*(color));
}

inline void PixelArray::SetPixel( int i, int j, const VectorR4 color )
{
	double t[3]={color.x,color.y,color.z};
	SetPixel(i,j,t);
}

inline void PixelArray::SetPixel( int i, int j, const VectorR3 color )
{
	double t[3]={color.x,color.y,color.z};
	SetPixel(i,j,t);
}

inline const float* PixelArray::GetPixel ( int i, int j ) const {
	return ColorValues + (((long) j)*WidthAlloc + ((long) i))*3;
}

inline void PixelArray::GetPixel( int i, int j, double* rgb ) const {
	const float* cptr = GetPixel(i,j);
	*(rgb++) = *(cptr++);
	*(rgb++) = *(cptr++);
	*(rgb) = *(cptr);
}

inline void PixelArray::GetPixel( int i, int j, float* rgb ) const {
	const float* cptr = GetPixel(i,j);
	*(rgb++) = (float)(*(cptr++));
	*(rgb++) = (float)(*(cptr++));
	*(rgb) = (float)(*(cptr));
}

#endif // PIXELARRAY_H
