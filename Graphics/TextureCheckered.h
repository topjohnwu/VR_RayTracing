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

#ifndef TEXTURECHECKERED_H
#define TEXTURECHECKERED_H

#include "TextureMapBase.h"

class MaterialBase;

// TextureCheckered makes a checkerboard style pattern.
//		It takes has one or two materials which replace
//		the material in the positions of subsquares.
// The texture pattern repeats and tiles the whole uv-plane.

class TextureCheckered : public TextureMapBase 
{
public:
	TextureCheckered() : Material1(0),Material2(0) { SetWidths(0.5,0.5); }

	void SetWidths(double uwidth, double vwidth) 
					{ uWidth=uwidth; vWidth=vwidth; }
	void SetMaterials( const MaterialBase* mat1, const MaterialBase* mat2 )
					{ SetMaterial1(mat1); SetMaterial2(mat2); }
	void SetMaterial1( const MaterialBase* mat1 ) { Material1 = mat1; }
	void SetMaterial2( const MaterialBase* mat2 ) { Material2 = mat2; }

	const MaterialBase* GetMaterial1() const { return Material1; }
	const MaterialBase* GetMaterial2() const { return Material2; }

	bool InOddSquare(double u, double v ) const;	// Compute even/odd ness
	virtual void ApplyTexture( VisiblePoint& visPoint ) const;

protected:
	double uWidth, vWidth;		// Width of squares in u and v directions
	const MaterialBase* Material1;	// Null if material not applied
	const MaterialBase* Material2;	// Null if material not applied
};

#endif //TEXTURECHECKERED_H
