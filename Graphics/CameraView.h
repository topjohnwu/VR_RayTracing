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

#ifndef CAMERA_VIEW_H
#define CAMERA_VIEW_H

class VectorR3;
#include "DirectLight.h"	// Has the class definition of View.
#include "PixelArray.h"

// A camera view is a view plus information about the directions
// to the pixels.

class CameraView : public View {

public:
	CameraView();

	// Use these for perspective camera - 
	// Calculates the unit vector from view point to pixel(i,j).
	void CalcPixelDirection( double i, double j, double* dir) const;
	void CalcPixelDirection( double i, double j, float* dir) const;
	void CalcPixelDirection( double i, double j, VectorR3* dir ) const;

	// Calculate Pixel View source -- for orthographic camera
	//   Calculates the view position to look orthographically at pixel (i,j)
	void CalcViewPosition( double i, double j, double* pos) const;
	void CalcViewPosition( double i, double j, float* pos) const;
	void CalcViewPosition( double i, double j, VectorR3* pos ) const;

	// Calculate Pixel Position
	void CalcPixelPosition( double i, double j, double* pos) const;
	void CalcPixelPosition( double i, double j, float* pos) const;
	void CalcPixelPosition( double i, double j, VectorR3* pos ) const;

	void SetScreenPixelSize( int width, int height );	// Width and height in pixels
	void SetScreenPixelSize( const PixelArray& pixelarray );
	void SetScreenDistance( double dist );	// Distance to the projection place
	void SetScreenDimensions( double width, double height );

	// Set the position of the camera
	virtual void SetPosition( double x, double y, double z );
	virtual void SetPosition( const double* );
	virtual void SetPosition( const float* );
	virtual void SetPosition( const VectorR3& pos );

	// Set the direction of the camera
	virtual void SetDirection( double x, double y, double z );
	virtual void SetDirection( const double* );
	virtual void SetDirection( const float* );
	virtual void SetDirection( const VectorR3& pos );

	// Set information in "look at" format
	void SetLookAt ( const VectorR3& eyePos, const VectorR3& lookAt, const VectorR3& upDir );
	// Set ViewFrustum must be called *after* either SetDistance() or SetLookAt();
	void SetViewFrustum ( double aspectRatio, double fovy );

	// Set type of viewer (These commands are inherited from View class).
	// void SetLocalViewer();
	// void SetNonLocalViewer();

	// Rotate the view around the screen center
	void RotateViewUp( double theta );
	void RotateViewRight( double theta );
	void RescaleDistanceOfViewer( double factor );		// Must be positive: 1.0 for no change

public:
	bool IsPositional() const { return IsLocalViewer(); }
	bool IsDirectional() const { return !IsLocalViewer(); }
	double GetScreenWidth() const { return ScreenWidth; }
	double GetScreenHeight() const { return ScreenHeight; }
	double GetScreenDistance() const { return ScreenDistance; }
	double GetAspectRatio() const { return ScreenWidth/ScreenHeight; }

	const VectorR3& GetCameraPosition() const { return GetPosition(); }
	const VectorR3& GetCameraDirection() const { return GetDirection(); }

	const VectorR3& GetScreenCenter() const { return ScreenCenter; }

	// The "PixeldU" is the vector from a pixel to it right neighboring pixel
	const VectorR3& GetPixeldU() const { return pixeldU; }
	// The "PixeldV" is the vector from a pixel up to the pixel above it.
	const VectorR3& GetPixeldV() const { return pixeldV; }

	// Clipping distances used only for OpenGL rendering, not for ray tracing.
	void SetClippingDistances( double near, double far );
	void SetDefaultClippingDistances() { SetClippingDistances(0.1*ScreenDistance, 10.0*ScreenDistance); }
	double GetNearClippingDist() const { return NearClippingDist; }
	double GetFarClippingDist() const { return FarClippingDist; }

private:

	int WidthPixels, HeightPixels;		// Width and height in pixels
	double ScreenWidth, ScreenHeight;	// Width and height of screen
	double ScreenDistance;				// Distance from camera to screen
	VectorR3 ScreenCenter;				// Position of the screen's center
	VectorR3 pixeldU, pixeldV;			// Vector displacement between pixels in u- and v-directions
	bool DistanceHasBeenSet;

	void CalcScreenCenter();	// Compute screen center from position and direction
	void PixelDirPreCalc();		// Precalculation of directions for screen (dU and dV)
	void RecalcPixeldUdV();		// Rescale dU and dV for changed screen size or screen resolution

	// These values are not used by the ray tracing code in most cases.
	double NearClippingDist;				
	double FarClippingDist;				
};

inline CameraView::CameraView()
{
	WidthPixels = HeightPixels = 2;
	ScreenWidth = ScreenHeight = 1.0;
	ScreenDistance = 10.0;
	DistanceHasBeenSet = false;
	SetDefaultClippingDistances();

	CalcScreenCenter();
	PixelDirPreCalc();
}

inline void CameraView::SetScreenPixelSize( int width, int height )		// Width and height in pixels
{
	WidthPixels = width;
	HeightPixels = height;

	RecalcPixeldUdV(); 
}

inline void CameraView::SetScreenPixelSize( const PixelArray& pixels )
{
	SetScreenPixelSize( pixels.GetWidth(), pixels.GetHeight() );
}

inline void CameraView::SetScreenDistance( double dist )
{	// Distance to the projection screen
	ScreenDistance = dist;
	DistanceHasBeenSet = true;
	SetDefaultClippingDistances();
	CalcScreenCenter();
}


inline void CameraView::SetScreenDimensions( double width, double height )
{
	ScreenWidth = width;
	ScreenHeight = height;
	RecalcPixeldUdV();
}

inline void CameraView::CalcPixelDirection( double i, double j, double* dir ) const 
{
	VectorR3 temp;
	CalcPixelDirection ( i, j, &temp );
	*dir = temp.x;
	*(dir+1) = temp.y;
	*(dir+2) = temp.z;
}

inline void CameraView::CalcPixelDirection( double i, double j, float* dir ) const 
{
	VectorR3 temp;
	CalcPixelDirection ( i, j, &temp );
	*dir = (float)temp.x;
	*(dir+1) = (float)temp.y;
	*(dir+2) = (float)temp.z;
}

inline void CameraView::CalcPixelDirection( double i, double j, VectorR3* dir ) const
{
	assert( IsLocalViewer() );

	CalcPixelPosition(i,j,dir);
	(*dir) -= Position;
	dir->Normalize();
}

inline void CameraView::CalcPixelPosition( double i, double j, double* dir ) const 
{
	VectorR3 temp;
	CalcPixelPosition ( i, j, &temp );
	*dir = temp.x;
	*(dir+1) = temp.y;
	*(dir+2) = temp.z;
}

inline void CameraView::CalcPixelPosition( double i, double j, float* dir ) const 
{
	VectorR3 temp;
	CalcPixelPosition ( i, j, &temp );
	*dir = (float)temp.x;
	*(dir+1) = (float)temp.y;
	*(dir+2) = (float)temp.z;
}

inline void CameraView::CalcPixelPosition( double i, double j, VectorR3* dir ) const
{
	double iOffset = i - (WidthPixels-1.0)/2.0;
	double jOffset = j - (HeightPixels-1.0)/2.0;

	*dir = ScreenCenter + iOffset*pixeldU + jOffset*pixeldV;
}

inline void CameraView::CalcViewPosition( double i, double j, double* pos) const
{
	VectorR3 temp;
	CalcViewPosition ( i, j, &temp );
	*pos = temp.x;
	*(pos+1) = temp.y;
	*(pos+2) = temp.z;
}

inline void CameraView::CalcViewPosition( double i, double j, float* pos) const
{
	VectorR3 temp;
	CalcViewPosition ( i, j, &temp );
	*pos = (float)temp.x;
	*(pos+1) = (float)temp.y;
	*(pos+2) = (float)temp.z;
}

inline void CameraView::CalcViewPosition( double i, double j, VectorR3* pos ) const
{
	double iOffset = i - (WidthPixels-1.0)/2.0;
	double jOffset = j - (HeightPixels-1.0)/2.0;

	*pos = Position + iOffset*pixeldU + jOffset*pixeldV;
}

inline void CameraView::SetPosition( double x, double y, double z ) {
	View::SetPosition(x,y,z);
	CalcScreenCenter();
}

inline void CameraView::SetPosition( const double* pos ) {
	View::SetPosition(pos);
	CalcScreenCenter();
}

inline void CameraView::SetPosition( const float* pos ) {
	View::SetPosition(pos);
	CalcScreenCenter();
}

inline void CameraView::SetPosition( const VectorR3& pos ) {
	View::SetPosition(pos);
	CalcScreenCenter();
}

inline void CameraView::SetDirection( double x, double y, double z ) {
	View::SetDirection(x,y,z);
	CalcScreenCenter();
	PixelDirPreCalc();
}

inline void CameraView::SetDirection( const double* dir) {
	View::SetDirection(dir);
	CalcScreenCenter();
	PixelDirPreCalc();
}

inline void CameraView::SetDirection( const float* dir) {
	View::SetDirection(dir);
	CalcScreenCenter();
	PixelDirPreCalc();
}

inline void CameraView::SetDirection( const VectorR3& dir ) {
	View::SetDirection(dir);
	CalcScreenCenter();
	PixelDirPreCalc();
}

inline void CameraView::SetClippingDistances( double near, double far )
{
	NearClippingDist = near; 
	FarClippingDist = far;
}


#endif  // CAMERA_VIEW_H
