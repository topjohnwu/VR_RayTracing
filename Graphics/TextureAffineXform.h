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

#ifndef TEXTUREAFFINEXFORM_H
#define TEXTUREAFFINEXFORM_H

#include "TextureMapBase.h"

// TextureAffineXform - Warps texture map coordinates by an affine transformation.

class TextureAffineXform : public TextureMapBase {

public:
	TextureAffineXform();

	// Make the affine transform be a scaling transformation.
	void SetScaling( double uFactor, double vFactor );

	// Set an arbitrary linear transform - Give entries in COLUMN order
	void SetLinearMatrix( double a11, double a21, double a12, double a22 );
	// Set an arbitrary affine transform - Give entries in COLUMN order
	void SetAffineMatrix( double a11, double a21, double a12, double a22,
						  double a13=0.0, double a23=0.0 );

	// Set texture coordinates for triangular points
	void SetTriangleCoords( double Ax, double Ay, double Bx, double By, 
							double Cx, double Cy);

	// More set routines to be written.

	void ApplyTexture( VisiblePoint& visPoint ) const;

private:
	double m11, m21, m12, m22, m13, m23;
};

inline TextureAffineXform::TextureAffineXform() 
{
	m11 = m22 = 1;
	m12 = m21 = m13 = m23 = 0.0;
}

inline void TextureAffineXform::SetScaling( double uFactor, double vFactor ) 
{
	m11 = uFactor;
	m22 = vFactor;
	m12 = m21 = m13 = m23 = 0.0;
}

// Set an arbitrary linear transform - set matrix in COLUMN order
inline void TextureAffineXform::SetLinearMatrix( double a11, double a21, 
												 double a12, double a22 )
{
	SetAffineMatrix( a11, a21, a12, a22, 0.0, 0.0);
}

// Set an arbitrary affine transform - set matrix in COLUMN order
inline void TextureAffineXform::SetAffineMatrix( double a11, double a21, 
												 double a12, double a22, 
												 double a13, double a23 )
{
	m11 = a11; 
	m21 = a21;
	m12 = a12;
	m22 = a22;
	m13 = a13;
	m23 = a23;
}

// Set texture coordinates for triangular points
//  A convenience function that maps the default texture coordinates for the
//  A-B-C vertices of a triangle (or the A-B-D vertices of a rectangle) to
//  three specified texture coordinate values.
inline void TextureAffineXform::SetTriangleCoords( 
							double Ax, double Ay,	// Image of <0,0>
							double Bx, double By,	// Image of <1,0>
							double Cx, double Cy) 	// Image of <0,1>
{
	m13 = Ax;			// Displacement
	m23 = Ay;
	m11 = Bx-Ax;		// Column 1
	m21 = By-Ay;
	m12 = Cx-Ax;		// Column 2
	m22 = Cy-Ay;
}

#endif //TEXTUREAFFINEXFORM_H
