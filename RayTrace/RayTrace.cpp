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

//#include <windows.h>
//#include <GL/gl.h>	// Basic OpenGL includes
// Including stdlib.h and disabling the atexit_hack seem to work everywhere.
//	Eventually there should be a new version of glut.h that doesn't need this.
#include <stdlib.h>
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>	// GLUT OpenGL includes 

#include "RayTraceData.h"
#include "../Graphics/PixelArray.h"
#include "../Graphics/ViewableBase.h"
#include "../Graphics/DirectLight.h"
#include "../Graphics/CameraView.h"
#include "../VrMath/LinearR3.h"
#include "../VrMath/LinearR4.h"
#include "../VrMath/MathMisc.h"
#include "../OpenglRender/GlutRenderer.h"

void RenderWithGlut(void);

void RayTraceView(void);
long SeekIntersection(const VectorR3& startPos, const VectorR3& direction,
										double *hitDist, VisiblePoint& returnedPoint,
										long avoidK = -1);
void RayTrace( int TraceDepth, const VectorR3& pos, const VectorR3 dir, 
			  VectorR3& returnedColor, long avoidK = -1);
bool ShadowFeeler(const VectorR3& pos, const Light& light, long intersectNum=-1 );
void CalcAllDirectIllum( const VectorR3& viewPos, const VisiblePoint& visPoint, 
						VectorR3& returnedColor, long avoidK = -1);

static void ResizeWindow(int w, int h);

GlutRenderer* glutDraw = 0;

// Window size and pixel array variables
bool WindowMinimized = false;
long WindowWidth;	// Width in pixels
long WindowHeight;	// Height in pixels
PixelArray* pixels;		// Array of pixels

// Ray Trace mode parameters:
bool RayTraceMode = false;		// Set true for RayTraciing,  false for rendering with OpenGL
								// Rendering with OpenGL does not support all features, esp., texture mapping
// Next two variables can be used to keep from re-raytracing a window.
long NumScanLinesRayTraced = -1;
long WidthRayTraced = -1;

const double MAX_DIST = 50.0;		// Max. view distance

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
		RenderWithGlut();
	}
}

// Renders the scene with OpenGL/GLUT -- gives low quality rendering without ray-tracing
void RenderWithGlut(void)
{
	if ( glutDraw==0 ) {
		glutDraw = new GlutRenderer();		// Needs to be constructed *after* glut initialized.
	}

	// Set camera position
	glutDraw->SetupCameraView( *MainView, 1.0, MAX_DIST );
	// Define all light sources
	int i;
	for ( i=0; i<NumLights; i++ ) {
		glutDraw->AddLight( *(LightArray[i]) );
	}
	glutDraw->SetGlobalAmbientLight( GlobalAmbientR3 );
	glutDraw->SetGlobalAmbientLight( 0.5, 0.5, 0.5 );

	// Render each ViewableObject
	for ( i=0; i<NumObjects; i++ ) {
		glutDraw->RenderViewable( *(ViewObj[i]) );
	}
	glutDraw->FinishRendering();
} 

// RayTraceView() is the top level routine that starts the ray tracing.
//	Current implementation: casts a ray to the center of each pixel.
//	Calls RayTrace() for each one.
void RayTraceView(void)
{
	int i,j;
	VectorR3 PixelDir;
	VisiblePoint visPoint;
	VectorR3 curPixelColor;		// Accumulator for Pixel Color

	if ( WidthRayTraced!=WindowWidth || NumScanLinesRayTraced!=WindowHeight ) {
		// Do the rendering here
		int TraceDepth = 3;
		for ( i=0; i<WindowWidth; i++) {
			for ( j=0; j<WindowHeight; j++ ) {
				MainView->CalcPixelDirection(i,j,&PixelDir);
				RayTrace( TraceDepth, MainView->GetPosition(), PixelDir, curPixelColor );
				pixels->SetPixel(i,j,curPixelColor);
			}
		}
		WidthRayTraced = WindowWidth;				// Set these values to show scene is computed
		NumScanLinesRayTraced = WindowHeight;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WindowWidth, 0, WindowHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	pixels->Draw();

	// flush the pipeline, swap the buffers
	glFlush();
	glutSwapBuffers();

}

// SeekIntersection seeks for an intersection with all viewable objects
// If it finds one, it returns the index of the viewable object,
//   and sets the value of hitDist and fills in the returnedPoint values.
long SeekIntersection(const VectorR3& pos, const VectorR3& direction,
										double *hitDist, VisiblePoint& returnedPoint,
										long avoidK)
{
	double BestDist = MAX_DIST;
	long hitK = -1;

	VectorR3 startPos = direction;
	startPos *= 0.0001;
	startPos += pos;

	for ( long k=0; k<NumObjects; k++ ) {
		if (ViewObj[k]->FindIntersection(startPos,direction,
											BestDist, hitDist, returnedPoint) )
		{
			if ( k!=avoidK || (*hitDist)>1.0e-5 ) { // Ignore if same as start point
				assert( BestDist>=(*hitDist) );
				BestDist = *hitDist;
				hitK = k;
			}
		}
	}
	return hitK;
}

// ShadowFeeler - returns whether the light is visible from the position pos.
//		intersectNum is the index of the visible object being (possibly)
//		illuminated at pos.

bool ShadowFeeler(const VectorR3& pos, const Light& light, long intersectNum ) {
	VectorR3 dir = light.GetPosition();
	dir -= pos;
	double dist = dir.Norm();
	if ( dist<1.0e-7 ) {
		return true;
	}
	dir /= dist;

	// Avoid self-shadowing
	VectorR3 startpos(dir);
	startpos *= 0.0001;		
	startpos += pos;

	double hitDist;
	VisiblePoint visPoint;
	for ( long k=0; k<NumObjects; k++ ) {
		if (ViewObj[k]->FindIntersection(startpos, dir,
											dist, &hitDist, visPoint) )
		{
			return false;
		}
	}
	return true;	// Did not find any shadowing objects
}

// **********************************************************************
//  RayTrace is the main recursive routine for performing basic 
//		recursive ray tracing.
// **********************************************************************

void RayTrace( int TraceDepth, const VectorR3& pos, const VectorR3 dir, 
			  VectorR3& returnedColor, long avoidK ) 
{
	double hitDist;
	VisiblePoint visPoint;

	int intersectNum = SeekIntersection(pos,dir,
								&hitDist,visPoint, avoidK );
	if ( intersectNum<0 ) {
		returnedColor = BackgroundColorR3;
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
				RayTrace( TraceDepth-1, visPoint.GetPosition(), nextDir, moreColor, intersectNum);
				VectorR3 c = thisMat->GetReflectionColor(visPoint,-dir,nextDir);
				moreColor.x *= c.x;
				moreColor.y *= c.y;
				moreColor.z *= c.z;
				returnedColor += moreColor;
			}

			// Ray Trace Transmission
			if ( thisMat->IsTransmissive() ) {
				if ( thisMat->CalcRefractDir(visPoint.GetNormal(),dir, nextDir) ) {
					RayTrace( TraceDepth-1, visPoint.GetPosition(), nextDir, moreColor, intersectNum);
					VectorR3 c = thisMat->GetTransmissionColor(visPoint,-dir,nextDir);
					moreColor.x *= c.x;
					moreColor.y *= c.y;
					moreColor.z *= c.z;
					returnedColor += moreColor;
				}
			}
		}
	}
}

//************************************************************
// CalcAllDirectIllum - calculates the direct illumination of
//		a point from all light sources.	
//			Only handles point light source.
// ***********************************************************

void CalcAllDirectIllum( const VectorR3& viewPos,
						 const VisiblePoint& visPoint, 
						 VectorR3& returnedColor, long avoidK )
{
	const MaterialBase* thisMat = &(visPoint.GetMaterial());
	returnedColor = ArrayProd(thisMat->GetColorAmbient(),GlobalAmbientR3);
	returnedColor += thisMat->GetColorEmissive();

	VectorR3 thisColor;
	VectorR3 percentLit;
	bool clearpath;

	for ( int k=0; k<NumLights; k++ ) {
		clearpath = ShadowFeeler(visPoint.GetPosition(), *(LightArray[k]), avoidK );
		if ( clearpath ) {
			percentLit.Set(1.0,1.0,1.0);	// Directly lit, with no shadowing
		}
		else {
			percentLit.SetZero();	// Blocked by shadows (still do ambient lighting)
		}
		DirectIlluminateViewPos (visPoint, viewPos,  
						 *(LightArray[k]), thisColor, percentLit); 
		returnedColor += thisColor;
	}
}

//********************************************************************
// Setup all the RayTrace geometries, lights, materials.			 *
//   -- Called to initialize the world.								 *
//********************************************************************
void SetupRayTraceWorld()
{
	// Initialize Pixel Array and main viewpoint
	SetUpMainView();
	pixels = new PixelArray(640,480);		// Array of pixels
	MainView->SetScreenPixelSize( *pixels );

	// Initialize Array of Materials
	SetUpMaterials();

	// Initialize Array of Lights
	SetUpLights();

	// Initialize array of viewable objects
	SetUpViewableObjects();
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
	double sRatio = Max(Cdims[0]/w, Cdims[1]/h);
	MainView->SetScreenDimensions( sRatio*w, sRatio*h );
	MainView->SetScreenPixelSize( *pixels );
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
		MainView->RotateViewUp( 0.1 );
		RayTraceMode = false;
		NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed		
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:	
		MainView->RotateViewUp( -0.1 );
		RayTraceMode = false;
		NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed		
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:	
		MainView->RotateViewRight( 0.1 );
		RayTraceMode = false;
		NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed		
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:	
		MainView->RotateViewRight( -0.1 );
		RayTraceMode = false;
		NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed		
		glutPostRedisplay();
		break;
	case GLUT_KEY_HOME:	
		MainView->RescaleDistanceOfViewer( 1.1 );
		RayTraceMode = false;
		NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed		
		glutPostRedisplay();
		break;
	case GLUT_KEY_END:	
		MainView->RescaleDistanceOfViewer( 0.9 );
		RayTraceMode = false;
		NumScanLinesRayTraced = WidthRayTraced = -1;	// Signal view has changed		
		glutPostRedisplay();
		break;
	}
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
	glutInitWindowSize(250, 120);
	glutInitWindowPosition(0, 0);
	glutCreateWindow( "Ray Tracing" );

	// set up callback functions
	glutKeyboardFunc( myKeyboardFunc );
	glutSpecialFunc( mySpecialFunc );
	glutReshapeFunc( ResizeWindow );

	// Set up information about the raytraced objects, lights, materials
	SetupRayTraceWorld();

	// call this in main loop
	glutDisplayFunc(RenderScene);
	glutMainLoop();

	return(0);
}
