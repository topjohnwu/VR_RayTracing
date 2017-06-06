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

#ifndef GLUT_RENDERER
#define GLUT_RENDERER

// Including stdlib.h and disabling the atexit_hack seem to work everywhere.
//	Eventually there should be a new version of glut.h that doesn't need this.
#include <stdlib.h>
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>

#include "../VrMath/LinearR3.h"
#include "../DataStructs/Array.h"
class CameraView;
class Light;
class ViewableBase;
class ViewableBezierSet;
class ViewableCone;
class ViewableCylinder;
class ViewableEllipsoid;
class ViewableParallelepiped;
class ViewableParallelogram;
class ViewableSphere;
class ViewableTorus;
class ViewableTriangle;
class MaterialBase;
class SceneDescription;

// The GlutRenderer class controls rendering Graphics items from the RayTrace
//	software package using GLUT.   This allows real time modification
//  and viewing of the scene.  Texture mapping is not supported!
//
// It is used as follows:
//		0. Set up the window, initialize GLUT.  Do this yourself.
//			GlutRenderer's should be constructed *after* GLUT is initialized.
//		1. Call SetupCameraView()
//		2. Call AddLight() repeatedly and SetGlobalAmbientLight() once
//		3. Call RenderViewable() repeatedly.
//		4. Call FinishRendering().
//	Repeat steps 1-4 for each frame to be rendered.
//
// Alternate usage with SceneDescription class:
//		0. Set up the window, initialize GLUT.  Do this yourself.
//			GlutRenderer's should be constructed *after* GLUT is initialized.
//		1. Call RenderScene() with the Scene Description
//	Repeat step 1 for each frame to be rendered.

class GlutRenderer {

public:
	GlutRenderer();

	// Set values for GLUT's "stacks" and "slices".
	void SetMeshCounts( int meshCounts ) { MeshCount = meshCounts; }	

	// Multiplies all material coefficients by this factor. Default = 1.
	// Compensates for lack of bounces adding more brightness.
	void SetBrightnessFactor( double factor );

	void SetupCameraView( const CameraView& theCameraView );
	void SetupCameraView( const CameraView& theCameraView, double nearDistance );
	void SetupCameraView( const CameraView& theCameraView, double nearDistance, double farDistance );

	void SetGlobalAmbientLight( const VectorR3& ambientLight );
	void SetGlobalAmbientLight( double ambientRed, double ambientGreen, double ambientBlue );
	void SetBackgroundColor( const VectorR3& backgroundColor );
	void SetBackgroundColor( double backRed, double backGreen, double backBlue );
	
	bool AddLight( const Light& light );			// Returns false if quota of lights exceeded

	void GlutRenderer::InitLightsAndView( const SceneDescription& scene );

	void RenderViewable( const ViewableBase& object );
	void RenderViewables( const SceneDescription& scene );
	void RenderViewables( const Array<ViewableBase*>& viewables );
	void FinishRendering();

	void RenderScene( const SceneDescription& scene );
	
	void InitRendering();	// Called by SetupLightsAndView, so usually you do not need to call this.

public:
	// These routines are usually not called directly: usually "RenderViewable" above
	//		is easier to use.
	void RenderViewableBezierSet( const ViewableBezierSet& bezierSet );
	void RenderViewableCone( const ViewableCone& object );
	void RenderViewableCylinder( const ViewableCylinder& object );
	void RenderViewableEllipsoid( const ViewableEllipsoid& object );
	void RenderViewableParallelepiped( const ViewableParallelepiped& object );
	void RenderViewableParallelogram( const ViewableParallelogram& object );
	void RenderViewableSphere( const ViewableSphere& object );
	void RenderViewableTorus( const ViewableTorus& object );
	void RenderViewableTriangle( const ViewableTriangle& object );
	
private:
	int MeshCount;
	double BrightnessMultiplier;
	VectorR3 GlobalAmbientLight;

	VectorR3 BackgroundColor;

	int NumLightsUsed;
	int NumLightsAllowed;

	void ApplyGlobalAmbientLight();
	void ApplyBackgroundColor();

	void SetFrontMaterial( const MaterialBase* mat );
	void SetBackMaterial( const MaterialBase* mat );
	void SetFaceMaterial( GLenum faceID, const MaterialBase* mat );
	void SetNormal( const VectorR3& normal );
	void PutVertex( const VectorR3& vertPos );

private:
	void CalcConeBasePt( double theta, const VectorR3& baseN, double baseD, 
						 const VectorR3& apex, const VectorR3& axisC, 
						 const VectorR3& axisA, const VectorR3& axisB, 
						 VectorR3* basePt1, VectorR3* normal1 );
	void CalcCylBasePt( double theta, const VectorR3& baseN, double baseD, 
						const VectorR3& center, const VectorR3& axisC, 
						const VectorR3& axisA, const VectorR3& axisB, 
						const VectorR3& topN, double topD, 
						VectorR3* basePt1, VectorR3* normal1 );
	void CalcCylTopPt ( double theta, const VectorR3& baseN, double baseD, 
						const VectorR3& center, const VectorR3& axisC, 
						const VectorR3& axisA, const VectorR3& axisB, 
						const VectorR3& topN, double topD, 
						VectorR3* basePt1 );

	void DrawLadder( const VectorR3& basePt1, const VectorR3& basePt2, 
					 const VectorR3& topPt1, const VectorR3& topPt2, 
					 const VectorR3& normal1, const VectorR3& normal2 );
};

#endif	// GLUT_RENDERER