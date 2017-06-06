/*
 *
 * RayTrace Software Package, release 3.2.  May 3, 2007.
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

#include <math.h>
#include <limits.h>
#include <stdio.h>

// If you do not have GLUT installed, you can use the basic GL routines instead.
//   For this, include windows.h and GL/gl.h, instead of GL/glut.h
//#include <windows.h>
//#include <GL/gl.h>	// Basic OpenGL includes
// Including stdlib.h and disabling the atexit_hack seem to work everywhere.
//	Eventually there should be a new version of glut.h that doesn't need this.
#include <stdlib.h>
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>	// GLUT OpenGL includes

#include "RayTraceStats.h"

#include "../Graphics/PixelArray.h"
#include "../Graphics/ViewableBase.h"
#include "../Graphics/DirectLight.h"
#include "../Graphics/CameraView.h"
#include "../VrMath/LinearR3.h"
#include "../VrMath/LinearR4.h"
#include "../VrMath/MathMisc.h"
#include "../OpenglRender/GlutRenderer.h"
#include "../DataStructs/KdTree.h"
#include "../RaytraceMgr/LoadNffFile.h"
#include "../RaytraceMgr/LoadObjFile.h"
#include "../RaytraceMgr/SceneDescription.h"
#include "RayTraceSetup2.h"

void RenderWithGlut(void);

void RayTraceView(void);
long SeekIntersection(const VectorR3& startPos, const VectorR3& direction,
										double *hitDist, VisiblePoint& returnedPoint,
										long avoidK = -1);
long SeekIntersectionKd(const VectorR3& startPos, const VectorR3& direction,
										double *hitDist, VisiblePoint& returnedPoint,
										long avoidK = -1);
void RayTrace( int TraceDepth, const VectorR3& pos, const VectorR3 dir, 
			  VectorR3& returnedColor, long avoidK = -1);
bool ShadowFeeler(const VectorR3& pos, const Light& light, long intersectNum=-1 );
void CalcAllDirectIllum( const VectorR3& viewPos, const VisiblePoint& visPoint, 
						VectorR3& returnedColor, long avoidK = -1);

static void ResizeWindow(int w, int h);

GlutRenderer* glutDraw = 0;

// ***********************Statistics************
RayTraceStats MyStats;
// **********************************************

// Window size and pixel array variables
bool WindowMinimized = false;
int WindowWidth;	// Width in pixels
int WindowHeight;	// Height in pixels
PixelArray* pixels;		// Array of pixels
	
bool RayTraceMode = false;		// Set true for RayTraciing,  false for rendering with OpenGL
								// Rendering with OpenGL does not support all features, esp., texture mapping
// Next two variables can be used to keep from re-raytracing a window.
long NumScanLinesRayTraced = -1;
long WidthRayTraced = -1;

const double MAX_DIST = 50;	// Max. view distance

SceneDescription* ActiveScene;

SceneDescription FileScene;			// Scene that is loaded from an .obj or .nff file.

// RenderScene() chooses between using OpenGL or  ray-tracing to render the scene
static void RenderScene(void)
{
	if ( WindowMinimized ) {
		return;
	}
	if ( RayTraceMode ) {
		RayTraceView();
	}
	else {
		GlutRenderer newGlutter;
		newGlutter.RenderScene(*ActiveScene);
	}
}

// ******************************************************
//   KdTree definitions and routines for creating the KdTree
// ******************************************************
KdTree ObjectKdTree;

void myExtentFunc( long objNum, AABB& retBox )
{
	return ActiveScene->GetViewable(objNum).CalcAABB( retBox );
}
bool myExtentsInBox( long objNum, const AABB& aabb, AABB& retBox)
{
	return ActiveScene->GetViewable(objNum).CalcExtentsInBox( aabb, retBox );
}

void myBuildKdTree()
{
	ObjectKdTree.SetDoubleRecurseSplitting( true );
	ObjectKdTree.SetObjectCost(8.0);
	ObjectKdTree.BuildTree( ActiveScene->NumViewables(), myExtentFunc, myExtentsInBox  );
	RayTraceStats::PrintKdStats( ObjectKdTree );
}

// *****************************************************************
// RayTraceView() is the top level routine that starts the ray tracing.
//	Current implementation: casts a ray to the center of each pixel.
//	Calls RayTrace() for each one.
// *****************************************************************

void RayTraceView(void)
{
	int i,j;
	VectorR3 PixelDir;
	VisiblePoint visPoint;
	VectorR3 curPixelColor;		// Accumulator for Pixel Color

	const CameraView& MainView = ActiveScene->GetCameraView();

	if ( WidthRayTraced!=WindowWidth || NumScanLinesRayTraced!=WindowHeight ) {  
		// Do the rendering here
		MyStats.Init();
		ObjectKdTree.ResetStats();
		int TraceDepth = 3;
		for ( i=0; i<WindowWidth; i++) {
			for ( j=0; j<WindowHeight; j++ ) {
				//if ( i==15 && (j==91 || j==169) ) {
				if ( i==16 && j==WindowHeight-106 ) {
					int iii = 0;
				}
				//i = 15;
				//j = 91;
				//j = 169;
				MainView.CalcPixelDirection(i,j,&PixelDir);
				RayTrace( TraceDepth, MainView.GetPosition(), PixelDir, curPixelColor );
				pixels->SetPixel(i,j,curPixelColor);
			}
		}
		WidthRayTraced = WindowWidth;			// Set these values to show scene has been computed.
		NumScanLinesRayTraced = WindowHeight;
		MyStats.GetKdRunData( ObjectKdTree );
		MyStats.PrintStats();
	}
			
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WindowWidth, 0, WindowHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	pixels->Draw();

	// flush the pipeline, swap the buffers
	glFlush();
	glutSwapBuffers();

}


// *********************************************************
// Data that supports the callback operation 
//		of the SeekIntersectionKd kd-Tree Traversal
//      and the ShadowFeelerKd kd-Tree Traversal
// *********************************************************
bool kdTraverseFeeler;
double isectEpsilon = 1.0e-6;
long bestObject;
long kdTraverseAvoid;
double bestHitDistance;
double kdShadowDist;
VisiblePoint tempPoint;
VisiblePoint* bestHitPoint;
VectorR3 kdStartPos;
VectorR3 kdStartPosAvoid;
VectorR3 kdTraverseDir;

// Call back function for KdTraversal of view ray or reflection ray
// It is of type PotentialObjectCallback.
bool potHitSeekIntersection( long objectNum, double* retStopDistance ) 
{
	double thisHitDistance;
	bool hitFlag;
	if ( objectNum == kdTraverseAvoid ) {
		hitFlag = ActiveScene->GetViewable(objectNum).FindIntersection(kdStartPosAvoid, kdTraverseDir,
											bestHitDistance, &thisHitDistance, tempPoint);
		if ( !hitFlag ) {
			return false;
		}
		thisHitDistance += isectEpsilon;		// Adjust back to real hit distance
	}
	else {
		hitFlag = ActiveScene->GetViewable(objectNum).FindIntersection(kdStartPos, kdTraverseDir,
											bestHitDistance, &thisHitDistance, tempPoint);
		if ( !hitFlag ) {
			return false;
		}
	}

	*bestHitPoint = tempPoint;		// The visible point that was hit
	bestObject = objectNum;				// The object that was hit
	bestHitDistance = thisHitDistance;
	*retStopDistance = bestHitDistance;	// No need to traverse search further than this distance
	return true;
}

// Call back function for KdTraversal of shadow feeler
// It is of type PotentialObjectCallback.
bool potHitShadowFeeler( long objectNum, double* retStopDistance ) 
{
	double thisHitDistance;
	bool hitFlag = ActiveScene->GetViewable(objectNum).FindIntersection(kdStartPos, kdTraverseDir,
											kdShadowDist, &thisHitDistance, tempPoint);
	if  ( hitFlag && !(/*objectNum==kdTraverseAvoid &&*/ thisHitDistance+isectEpsilon>=kdShadowDist) )
	{
		kdTraverseFeeler = false;
		*retStopDistance = -1.0;	// Negative value should abort process quickly
		return true;
	}
	else { 
		return false;
	}
}



// SeekIntersectionKd seeks for an intersection with all viewable objects
// If it finds one, it returns the index of the viewable object,
//   and sets the value of hitDist and fills in the returnedPoint values.
// This "Kd" version uses the Kd-Tree
long SeekIntersectionKd(const VectorR3& pos, const VectorR3& direction,
										double *hitDist, VisiblePoint& returnedPoint,
										long avoidK)
{
	MyStats.AddRayTraced();

	bestObject = -1;
	bestHitDistance = DBL_MAX;
	kdTraverseAvoid = avoidK;
	kdStartPos = pos;
	kdTraverseDir = direction;
	kdStartPosAvoid = pos;
	kdStartPosAvoid.AddScaled( direction, isectEpsilon );
	bestHitPoint = &returnedPoint;
	
	bestObject = -1;
	ObjectKdTree.Traverse( pos, direction, *potHitSeekIntersection );

	if ( bestObject>=0 ) {
		*hitDist = bestHitDistance;
	}
	return bestObject;
}	

// ShadowFeeler - returns whether the light is visible from the position pos.
//		Return value is "true" if no shadowing object found.
//		intersectNum is the index of the visible object being (possibly)
//		illuminated at pos.

bool ShadowFeelerKd(const VectorR3& pos, const Light& light, long intersectNum ) {
	MyStats.AddRayTraced();
	MyStats.AddShadowFeeler();

	kdTraverseDir = pos;
	kdTraverseDir -= light.GetPosition();
	double dist = kdTraverseDir.Norm();
	if ( dist<1.0e-7 ) {
		return true;		// Extremely close to the light!
	}
	kdTraverseDir /= dist;			// Direction from light position towards pos
	kdStartPos = light.GetPosition();
	kdTraverseFeeler = true;		// True indicates no shadowing objects
	kdTraverseAvoid = intersectNum;
	kdShadowDist = dist;
	ObjectKdTree.Traverse( light.GetPosition(), kdTraverseDir, potHitShadowFeeler, dist, true );

	return kdTraverseFeeler;	// Return whether ray is free of shadowing objects
}


void RayTrace( int TraceDepth, const VectorR3& pos, const VectorR3 dir, 
			  VectorR3& returnedColor, long avoidK ) 
{
	double hitDist;
	VisiblePoint visPoint;

	int intersectNum = SeekIntersectionKd(pos, dir,
								&hitDist, visPoint, avoidK );
	if ( intersectNum<0 ) {
		returnedColor = ActiveScene->BackgroundColor();
	}
	else {
		CalcAllDirectIllum( pos, visPoint, returnedColor, intersectNum );
		if ( TraceDepth > 1 ) {
			VectorR3 nextDir;
			VectorR3 moreColor;
			const MaterialBase* thisMat = &(visPoint.GetMaterial());

			// Ray trace reflection
			if ( thisMat->IsReflective() ) {
				nextDir = visPoint.GetNormal();
				nextDir *= -2.0*(dir^visPoint.GetNormal());
				nextDir += dir;
				nextDir.ReNormalize();	// Just in case...
				VectorR3 c = thisMat->GetReflectionColor(visPoint, -dir, nextDir);
				RayTrace( TraceDepth-1, visPoint.GetPosition(), nextDir, moreColor, intersectNum);
				moreColor.x *= c.x;
				moreColor.y *= c.y;
				moreColor.z *= c.z;
				returnedColor += moreColor;
			}

			// Ray Trace Transmission
			if ( thisMat->IsTransmissive() ) {
				if ( thisMat->CalcRefractDir(visPoint.GetNormal(),dir, nextDir) ) {
					VectorR3 c = thisMat->GetTransmissionColor(visPoint, -dir, nextDir);
					RayTrace( TraceDepth-1, visPoint.GetPosition(), nextDir, moreColor, intersectNum);
					moreColor.x *= c.x;
					moreColor.y *= c.y;
					moreColor.z *= c.z;
					returnedColor += moreColor;
				}
			}
		}
	}
}

void CalcAllDirectIllum( const VectorR3& viewPos,
						 const VisiblePoint& visPoint, 
						 VectorR3& returnedColor, long avoidK )
{
	const MaterialBase* thisMat = &(visPoint.GetMaterial());
	const VectorR3& ambientcolor = thisMat->GetColorAmbient();
	const VectorR3& ambientlight = ActiveScene->GlobalAmbientLight();
	const VectorR3& emitted = thisMat->GetColorEmissive();
	returnedColor.x = ambientcolor.x*ambientlight.x + emitted.x;
	returnedColor.y = ambientcolor.y*ambientlight.y + emitted.y;
	returnedColor.z = ambientcolor.z*ambientlight.z + emitted.z;

	VectorR3 thisColor;
	VectorR3 percentLit;
	VectorR3 toLight;
	bool checksides = visPoint.GetMaterial().IsTransmissive();
	double viewDot;
	if ( !checksides ) {						// If not transmissive
		toLight = viewPos;
		toLight -= visPoint.GetPosition();		// Direction to *viewer*
		viewDot = toLight^visPoint.GetNormal();
	}
	bool clearpath;

	int numLights = ActiveScene->NumLights();
	for ( int k=0; k<numLights; k++ ) {
		const Light& thisLight = ActiveScene->GetLight(k);
		clearpath = true;
		// Cast a shadow feeler if (a) transmissive or (b) light and view on the same side
		if ( !checksides ) {
			toLight = thisLight.GetPosition();
			toLight -= visPoint.GetPosition();		// Direction to light
			if ( !SameSignNonzero( viewDot, (toLight^visPoint.GetNormal()) ) ) {
				clearpath = false;
			}
		}
		if ( clearpath ) {
			clearpath = ShadowFeelerKd(visPoint.GetPosition(), thisLight, avoidK );
		}
		if ( clearpath ) {
			percentLit.Set(1.0,1.0,1.0);	// Directly lit, with no shadowing
		}
		else {
			percentLit.SetZero();	// Blocked by shadows (still do ambient lighting)
		}
		DirectIlluminateViewPos (visPoint, viewPos,  
						 thisLight, thisColor, percentLit); 
		returnedColor.x += thisColor.x;
		returnedColor.y += thisColor.y;
		returnedColor.z += thisColor.z;
	}
}

// called when the window is resized
static void ResizeWindow(int w, int h)
{
	WindowMinimized = (h==0 || w==0);
	h = (h==0) ? 1 : h;
	w = (w==0) ? 1 : w;

	if ( (NumScanLinesRayTraced!=h || WidthRayTraced!=w) && !WindowMinimized ) {
		RayTraceMode = false;							// Go back to OpenGL mode if size changes.
	}

	WindowHeight = h;
	WindowWidth = w;
	if ( pixels->SetSize( WindowWidth, WindowHeight ) ) {	// If pixel data reallocated,
		RayTraceMode = false;
		NumScanLinesRayTraced = WidthRayTraced = -1;		// signal pixel data no longer valid
	}

	glViewport(0, 0, w, h);

	// Resize the camera viewpoint
	ActiveScene->CalcNewScreenDims( (double)w / (double)h );
	ActiveScene->GetCameraView().SetScreenPixelSize( *pixels );
}

// *******************************************************************
// Handle all "normal" ascii key presses.
// The "g" or space bar commands have the same effect: switch between
//		rendering with OpenGL and rendering with Glut.
// *******************************************************************
void myKeyboardFunc( unsigned char key, int x, int y )
{
	switch ( key ) {

	case 'g':							// "g" command
	case ' ':							// Space bar
		// Set to be in Ray Trace mode
		if ( !RayTraceMode ) {
			RayTraceMode = true;
			glutPostRedisplay();
		}
		break;
	case 'G':							// 'G' command
		// Set to be rendering with OpenGL
		if ( RayTraceMode ) {
			RayTraceMode = false;
			glutPostRedisplay();
		}
		break;
	}
}

// *******************************************************************
// Handle all "special" key presses.
// *******************************************************************
void mySpecialFunc( int key, int x, int y )
{
	switch ( key ) {

	case GLUT_KEY_UP:	
		ActiveScene->GetCameraView().RotateViewUp( 0.1 );
		RayTraceMode = false;
		NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed		
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:	
		ActiveScene->GetCameraView().RotateViewUp( -0.1 );
		RayTraceMode = false;
		NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed		
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:	
		ActiveScene->GetCameraView().RotateViewRight( 0.1 );
		RayTraceMode = false;
		NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed		
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:	
		ActiveScene->GetCameraView().RotateViewRight( -0.1 );
		RayTraceMode = false;
		NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed		
		glutPostRedisplay();
		break;
	case GLUT_KEY_HOME:	
		ActiveScene->GetCameraView().RescaleDistanceOfViewer( 1.1 );
		RayTraceMode = false;
		NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed		
		glutPostRedisplay();
		break;
	case GLUT_KEY_END:	
		ActiveScene->GetCameraView().RescaleDistanceOfViewer( 0.9 );
		RayTraceMode = false;
		NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed		
		glutPostRedisplay();
		break;
	}
}

// **********************************************************
// Called for mouse clicks  (Debugging mostly)
//***********************************************************
void myMouseUpDownFunc( int button, int state, int x, int y )
{
	if ( button!=GLUT_LEFT_BUTTON ) {
		return;
	}
	if ( state==GLUT_DOWN ) {
		fprintf(stdout, "Mouse click at: %d, %d.\n", x, y );
	}
}

void InitializeSceneGeometry()
{
	// Define the lights, materials, textures and viewable objects.

// One of the following three lines should un-commented to select the way
//		the scene is loaded into the SceneDescription.
//#define MODE 1  /* Use this line to manually set the scene in RayTraceSetup2.cpp */
//#define MODE 2  /* Use this line to load the scene data from an .obj file. */
#define MODE 3  /* Use this line to load the scene data from a .nff file. */
#if MODE==1
	SetUpScene2();
	ActiveScene = &TheScene2;
#elif MODE==2
	LoadObjFile( "f15.obj", FileScene );
	ActiveScene = &FileScene;
	// The next lines specify scene attributes not given in the obj file.
	ActiveScene->SetBackGroundColor( 0.0, 0.0, 0.0 );
	ActiveScene->SetGlobalAmbientLight( 0.6, 0.6, 0.2 );
	CameraView& theCV = ActiveScene->GetCameraView();
	theCV.SetPosition( 0.0, 0.0, 40.0 );
	theCV.SetScreenDistance( 40.0 );
	theCV.SetScreenDimensions( 20.0, 20.0 );
	SetUpLights( *ActiveScene );
	// You may add more scene elements here if you wish
#else
	LoadNffFile( "jacks_5_1.nff", FileScene );
	ActiveScene = &FileScene;
	// You may add more scene elements here if you wish
#endif

	pixels = new PixelArray(10,10);		// Array of pixels
	ActiveScene->GetCameraView().SetScreenPixelSize( *pixels );
	ActiveScene->RegisterCameraView();

	// Build the kd-Tree.
	myBuildKdTree();
}


//**********************************************************
// Main Routine
// Set up OpenGL, hook up callbacks, define RayTrace world,
// and start the main loop
//**********************************************************
int main( int argc, char** argv )
{
	fprintf( stdout, "Press 'g' or space bar to start ray tracing. (And then wait!)\n" );
	fprintf( stdout, "Press 'G' to return to OpenGL render mode.\n" );
	fprintf( stdout, "Arrow keys change view direction (and use OpenGL).\n" );
	fprintf( stdout, "Home/End keys alter view distance --- resizing keeps it same view size.\n");

	glutInit(&argc, argv);
	// we're going to animate it, so double buffer 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize(301,246);
	glutInitWindowPosition(0, 0);
	glutCreateWindow( "Ray Tracing" );

	InitializeSceneGeometry();

	// set up callback functions
	glutKeyboardFunc( myKeyboardFunc );
	glutSpecialFunc( mySpecialFunc );
	glutMouseFunc( myMouseUpDownFunc );
	glutReshapeFunc( ResizeWindow );

	// call this in main loop
	glutDisplayFunc(RenderScene);
	glutMainLoop();

	return(0);
}
