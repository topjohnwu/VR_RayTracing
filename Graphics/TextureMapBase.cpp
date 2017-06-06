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

#include "TextureMapBase.h"
#include "../VrMath/LinearR2.h"
#include "../VrMath/LinearR3.h"
#include "../VrMath/MathMisc.h"

// Convert a unit vector reflection direction to cube map coordinates
// The input is a unit vector: reflectDir
// The return value is a VectorR2, a value in [0,1]x[0,1]
// Layout of the cube map.
//				 Up
//		   Left Back Right Front
//				Down
void TextureMapBase::ReflectDirToCubeMap( const VectorR3& reflectDir, 
										  VectorR2* cubeMapCoords ) {

	// Array of center positions.
	double centers[12] = {
							7.0/8.0, 3.0/6.0,		// Front
							3.0/8.0, 3.0/6.0,		// Back
							5.0/8.0, 3.0/6.0,		// Right
							1.0/8.0, 3.0/6.0,		// Left
							3.0/8.0, 5.0/6.0,		// Up (Ceiling)
							3.0/8.0, 1.0/6.0,		// Down (Floor)
	};

	assert ( reflectDir.IsUnit() );

	double maxAbs = fabs(reflectDir.x);
	int maxDir = 0;					// Zero if max component is x.
	double t = fabs(reflectDir.y);
	if ( t > maxAbs ) {
		maxDir = 1;
		maxAbs = t;
	}
	t = fabs(reflectDir.z);
	if ( t > maxAbs ) {
		maxDir = 2;
		maxAbs = t;
	}

	double a, b;
	switch ( maxDir ) {
	case 0:
		if ( reflectDir.x>0.0 ) {			// If right wall
			cubeMapCoords->Load(centers+4);
		}
		else {
			cubeMapCoords->Load(centers+6);	// Left wall
		}
		a = reflectDir.z/reflectDir.x;
		b = reflectDir.y/maxAbs;
		break;
	case 1:
		if ( reflectDir.y>0.0 ) {				// If up (ceiling)
			cubeMapCoords->Load(centers+8);
		}
		else {
			cubeMapCoords->Load(centers+10);	// Else, it's down (floor)
		}
		a = reflectDir.x/maxAbs;
		b = reflectDir.z/reflectDir.y;
		break;
	case 2:
		if ( reflectDir.z>0.0 ) {			// If front wall (viewer position)
			cubeMapCoords->Load(centers);
		}
		else {
			cubeMapCoords->Load(centers+2);	// Back wall
		}
		a = -reflectDir.x/reflectDir.z;
		b = reflectDir.y/maxAbs;
		break;
	}

	cubeMapCoords->x += a*OneEighth;
	cubeMapCoords->y += b*OneSixth;

}
