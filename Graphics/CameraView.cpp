/*
 *
 * RayTrace Software Package, release 3.1.2  February 12, 2007.
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

#include "CameraView.h"

void CameraView::CalcScreenCenter()
{
	ScreenCenter = Direction;
	ScreenCenter *= ScreenDistance;
	ScreenCenter += Position;
}

// Set the camera position according the "LookAt" data
void CameraView::SetLookAt( const VectorR3& eyePos, const VectorR3& lookAt, const VectorR3& upDir )
{
	SetLocalViewer();					// This is a "local" viewer

	View::SetPosition(eyePos);
	ScreenCenter = lookAt;
	VectorR3 viewDir = lookAt-eyePos;
	ScreenDistance = viewDir.Norm();
	assert ( ScreenDistance!=0.0 );
	DistanceHasBeenSet = true;
	SetDefaultClippingDistances();
	viewDir /= ScreenDistance;			// Normalize the viewDir
	View::SetDirection( viewDir );
	pixeldV = upDir;
	pixeldV.AddScaled( viewDir, -(upDir^viewDir) );
	pixeldU = viewDir;
	pixeldU *= upDir;		// Vector points rightward
	assert ( !viewDir.NearZero(1.0e-10) && !pixeldU.NearZero(1.0e-10) && !pixeldV.NearZero(1.0e-10) );
	RecalcPixeldUdV();
}

// This function must be called *AFTER* the distance to the screen has been
//	set (by either a call to SetDistance() or a call to SetLookAt().)
void CameraView::SetViewFrustum( double aspectRatio, double fovy )
{
	assert ( DistanceHasBeenSet );
	ScreenHeight = 2.0*ScreenDistance*tan(fovy*0.5);
	ScreenWidth = aspectRatio * ScreenHeight;
	RecalcPixeldUdV();
}

// This routine assumes the view position (Position), and the view direction (Direction)
//   and distance to the screen (ScreenDistance) are already calculated. It computes
//	 the u direction vector and v direction vector.
void CameraView::PixelDirPreCalc( ) {

	VectorR3 u(-Direction.z,0.0,Direction.x);		// Sideways direction (rightward)
	if ( u.IsZero() ) {
		u.Set(0.0,1.0,0.0);
	}
	VectorR3 v = u*Direction;		// Cross product - gives "up" direction
	pixeldU = u;
	pixeldV = v;
	RecalcPixeldUdV();
}

// Rescale dU and dV for changed screen size or screen resolution
void CameraView::RecalcPixeldUdV()
{
	pixeldU.Normalize();
	if ( WidthPixels>1 ) {
		pixeldU *= ScreenWidth/(WidthPixels-1);
	}
	pixeldV.Normalize();
	if ( HeightPixels>1 ) {
		pixeldV *= ScreenHeight/(HeightPixels-1);
	}
}

// Given a fully established camera and view, rotate the view upward around
//		screen's  center.
void CameraView::RotateViewUp( double theta )
{
	VectorR3 upDir = pixeldV;
	upDir.Normalize();
	Direction *= cos(theta);
	Direction.AddScaled(upDir, -sin(theta) );
	Direction.Normalize();						// Just in case....

	pixeldV = pixeldU*Direction;				// New up direction
	pixeldV.Normalize();
	if ( HeightPixels>1 ) {
		pixeldV *= ScreenHeight/(HeightPixels-1);
	}

	Position = ScreenCenter - ScreenDistance*Direction;
}

// Given a fully established camera and view, rotate the view rightward around
//		screen's  center.
void CameraView::RotateViewRight( double theta )
{
	VectorR3 rightDir = pixeldU;
	rightDir.Normalize();
	Direction *= cos(theta);
	Direction.AddScaled(rightDir, -sin(theta) );
	Direction.Normalize();						// Just in case....

	pixeldU = Direction*pixeldV;				// New "right" direction
	pixeldU.Normalize();
	if ( WidthPixels>1 ) {
		pixeldU *= ScreenWidth/(WidthPixels-1);
	}

	Position = ScreenCenter - ScreenDistance*Direction;
}

// Given a fully established camera and view, scale distance of viewer from
//		screen's  center.
void CameraView::RescaleDistanceOfViewer( double factor )
{
	assert ( factor>0.0 );
	if ( factor>0.0 ) {
		ScreenDistance *= factor;
	}

	Position = ScreenCenter - ScreenDistance*Direction;
}

