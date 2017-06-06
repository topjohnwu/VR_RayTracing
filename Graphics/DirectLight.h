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

#ifndef DIRECTLIGHT_H
#define DIRECTLIGHT_H

#include "../VrMath/LinearR4.h"
#include "Material.h"	// header file
#include "Light.h"
#include "ViewableBase.h"

// These two classes are defined below
class View;				// A viewer (viewpoint position and direction)
class LightView;		// Combination of a light and a view


// For a view structure (may be a local viewer)
void DirectIlluminate( const VectorR3& position, const VectorR3& normal,
					  const View& view,
					  const Light& light,
					  const MaterialBase& material,
					  VectorR3& colorReturned,
					  const VectorR3& percentLit );

// For a LightView combined class
void DirectIlluminate( const VectorR3& position, const VectorR3& normal,
					  const LightView& lv,
					  const MaterialBase& material,
					  VectorR3& colorReturned,
					  const VectorR3& percentLit );

// For a viewpoint with explicit position
void DirectIlluminateViewPos( const VectorR3& position, const VectorR3& normal,
					  const VectorR3& ViewPos,
					  const Light& light,
					  const MaterialBase& material,
					  VectorR3& colorReturned,
					  const VectorR3& percentLit ); 
// For a viewpoint with explicit direction of view 
//    ViewDir must a normal vector from the direction of the viewer
void DirectIlluminateViewDir( const VectorR3& position, const VectorR3& normal,
					  const VectorR3& ViewDir,
					  const Light& light,
					  const MaterialBase& material,
					  VectorR3& colorReturned,
					  const VectorR3& percentLit );

// The routines below are identical except use the VisiblePoint class


// For a view structure (may be a local viewer)
inline void DirectIlluminate( const VisiblePoint& visPoint,
					  const View& view,
					  const Light& light,
					  VectorR3& colorReturned,
					  const VectorR3& percentLit ) 
{
	DirectIlluminate( visPoint.GetPosition(), visPoint.GetNormal(),
					  view, light, visPoint.GetMaterial(), colorReturned,
					  percentLit  );
}

// For a LightView combined class
inline void DirectIlluminate( const VisiblePoint& visPoint,
					  const LightView& lv,
					  VectorR3& colorReturned,
					  const VectorR3& percentLit )
{
	DirectIlluminate( visPoint.GetPosition(), visPoint.GetNormal(),
					  lv, visPoint.GetMaterial(), colorReturned,
					  percentLit  );
}

// For a viewpoint with explicit position
inline void DirectIlluminateViewPos( const VisiblePoint& visPoint,
					  const VectorR3& ViewPos,
					  const Light& light,
					  VectorR3& colorReturned,
					  const VectorR3& percentLit )
{
	DirectIlluminateViewPos( visPoint.GetPosition(), visPoint.GetNormal(),
					  ViewPos, light, visPoint.GetMaterial(), colorReturned,
					  percentLit  );
}
 
// For a viewpoint with explicit direction of view 
//    ViewDir must a normal vector from the direction of the viewer
inline void DirectIlluminateViewDir( const VisiblePoint& visPoint,
					  const VectorR3& ViewDir,
					  const Light& light,
					  VectorR3& colorReturned,
					  const VectorR3& percentLit )
{
	DirectIlluminateViewDir( visPoint.GetPosition(), visPoint.GetNormal(),
					  ViewDir, light, visPoint.GetMaterial(), colorReturned,
					  percentLit );
}


// N = surface normal
// L = Unit vector towards light
// V = Unit vector towards viewer
// H = H vector (or null pointer)
void DirectIlluminateBasic( VectorR3& colorReturned, const MaterialBase& material, 
						    const Light& light, 
							const VectorR3& percentLit, double lightAttenuation,
							const VectorR3& N, const VectorR3& V, 
							const VectorR3& L, const VectorR3* H );

void CalcAmbientOnly( const MaterialBase& mat, const Light& light, double lightAttenuation,
					  VectorR3& colorReturned );

bool CalcLightDirAndFactor(const Light& light, 
						   const VectorR3& position, 
						   VectorR3 *lightVector, 
						   double *lightAttenuate );


class View {
public:
	View() { Reset(); }

	void Reset(); 

	void SetLocalViewer() { LocalViewer = true; }
	void SetNonLocalViewer() { LocalViewer = false; }
	bool IsLocalViewer() const { return LocalViewer; }

	virtual void SetPosition( double x, double y, double z );
	virtual void SetPosition( const double* );
	virtual void SetPosition( const float* );
	virtual void SetPosition( const VectorR3& pos );
	void GetPosition( double* ) const;
	void GetPosition( float* ) const;
	const VectorR3& GetPosition() const { return Position; }

	virtual void SetDirection( double x, double y, double z );
	virtual void SetDirection( const double* );
	virtual void SetDirection( const float* );
	virtual void SetDirection( const VectorR3& dir );
	void GetDirection( double* ) const;
	void GetDirection( float* ) const;
	const VectorR3& GetDirection() const { return Direction; }

protected:
	VectorR3 Position;
	VectorR3 Direction;	// Viewer direction (unit vector).
	bool LocalViewer;	// True if is a local viewer
						// If local viewer, only Position is used
						// If non-local viewer, only the Direction is used.
};	

class LightView {
public:
	LightView() :light(0), view(0), UseH(false) {};
	LightView( Light& l, View& v, bool useH ) 
		: light(&l), view(&v), UseH(useH) {}

	void SetView( View& v ) { view = &v; }
	void SetLight( Light& l ) { light = &l; }

	View& GetView() const { return *view; }
	Light& GetLight() const { return *light; }

	void InitUseH() { CalcH(); UseH = true; }
	void ResetUseH() { UseH = false; }
	bool GetUseH() const { return UseH; }
	
	void GetH( double* h) const;
	void GetH( float* h) const;
	
	const VectorR3& GetH() const { return H; }
	
private:
	Light* light;
	View* view;
	bool UseH;		// Should we use the H vector?
	VectorR3 H;		// Precomputed H vector (non-local viewer).

	void CalcH();	// Calculate the H vector
};

inline void View::Reset() { 
	Position.SetZero();
	Direction.SetNegUnitZ(); 
	SetLocalViewer();
}

inline void View::SetPosition( double x, double y, double z )
{
	Position.Set(x, y, z);
}

inline void View::SetPosition( const double* pos) {
	SetPosition( *pos, *(pos+1), *(pos+2) );
}

inline void View::SetPosition( const float* pos ) {
	SetPosition( *pos, *(pos+1), *(pos+2) );
}

inline void View::SetPosition( const VectorR3& pos ) {
	SetPosition( pos.x, pos.y, pos.z );
}

inline void View::GetPosition( double* pos ) const {
	*pos = Position.x;
	*(pos+1) = Position.y;
	*(pos+2) = Position.z;
}

inline void View::GetPosition( float* pos ) const {
	*pos = (float)Position.x;
	*(pos+1) = (float)Position.y;
	*(pos+2) = (float)Position.z;
}

inline void View::SetDirection( double x, double y, double z )
{
	Direction.Set(x, y, z);
	Direction.Normalize();
}

inline void View::SetDirection( const double* dir) {
	SetDirection( *dir, *(dir+1), *(dir+2) );
}

inline void View::SetDirection( const float* dir ) {
	SetDirection( *dir, *(dir+1), *(dir+2) );
}

inline void View::SetDirection( const VectorR3& dir ) {
	SetDirection( dir.x, dir.y, dir.z );
}

inline void View::GetDirection( double* dir ) const {
	*dir = Direction.x;
	*(dir+1) = Direction.y;
	*(dir+2) = Direction.z;
}

inline void View::GetDirection( float* dir ) const {
	*dir = (float)Direction.x;
	*(dir+1) = (float)Direction.y;
	*(dir+2) = (float)Direction.z;
}

inline void LightView::GetH( double* h) const {
	*h = H.x;
	*(h+1) = H.y;
	*(h+2) = H.z;
}

inline void LightView::GetH( float* h) const {
	*h = (float)H.x;
	*(h+1) = (float)H.y;
	*(h+2) = (float)H.z;
}


#endif // DIRECTLIGHT_H

