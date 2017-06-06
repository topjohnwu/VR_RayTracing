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

#include "RayTraceData.h"

#include "../Graphics/Material.h"
#include "../Graphics/ViewableSphere.h"
#include "../Graphics/ViewableTriangle.h"
#include "../Graphics/ViewableParallelogram.h"
#include "../Graphics/TextureCheckered.h"
#include "../Graphics/Light.h"
#include "../Graphics/CameraView.h"

// Data for views and intersection management

double RedColor[3] = {1.0, 0.0, 0.0};
double BlueColor[3] = {0.0, 0.0, 1.0};
double BlackColor[3] = {0.0, 0.0, 0.0};

// Camera Information   *****************************************

CameraView* MainView;
double Cpos[3] = {0.0,0.0,25.0};	// Position of camera
double Cdir[3] = {0.0,0.0,-1.0};	// Direction of camera
double Cdist = 25.0;				// Distance to "screen"
double Cdims[2] = {18.0, 18.0};		// Width & height of "screen"

// Data for lights  **********************************************

// Global lighting data
double GlobalAmbientLight[3] = { 0.0, 0.0, 0.0 };
double* BackgroundColor = &BlackColor[0];
VectorR3 GlobalAmbientR3(GlobalAmbientLight[0],GlobalAmbientLight[1],GlobalAmbientLight[2]);
VectorR3 BackgroundColorR3(BackgroundColor[0],BackgroundColor[1],BackgroundColor[2]);

// Array of (pointers to) lights:
const int MAX_LIGHTS = 3;
int NumLights = 0;
Light* LightArray[MAX_LIGHTS];

// Lighting values
float Lt0amb[3] = {0.0f, 0.0f, 0.0f};
float Lt0diff[3] = {1.0f, 1.0f, 1.0f};
float Lt0spec[3] = {1.0f, 1.0f, 1.0f};
float Lt0pos[3] = {5.0, 0.0f, 20.0f};

float Lt1amb[3] = {0.0f, 0.0f, 0.0f};
float Lt1diff[3] = {1.0f, 1.0f, 1.0f};
float Lt1spec[3] = {1.0f, 1.0f, 1.0f};
float Lt1pos[3] = {-20.0f, -5.0, 20.0f};

// Data for materials *******************************************

const int MAX_MATERIALS = 5;
int NumMaterials = 0;
Material* MatArray[MAX_MATERIALS];	// Array of material pointers.

// Material values
float Noemit[3] = {0.0f, 0.0f, 0.0f};
float Mat0spec[3] = {1.0f, 1.0f, 1.0f};

#if 0 
// Set up the sphere as reflective, but not transmissive
float Mat0nonspec[3] = {0.6f, 0.0f, 0.1f};
float Mat0reflect[3] = {0.8f, 0.8f, 0.8f};
float Mat0trans[3] = {0.0f, 0.0f, 0.0f};		// Default values
#else
// Set up the sphere as reflective and transmissive
float Mat0nonspec[3] = {0.1f, 0.0f, 0.0f};
float Mat0reflect[3] = {0.4f, 0.4f, 0.4f};
float Mat0trans[3] = {0.8f, 0.8f, 0.8f};
#endif

float Mat0shiny = 512.0f;
float Mat0refractIndex = 1.33f;
float Mat1spec[3] = {0.7f, 0.7f, 0.7f};
float Mat1nonspec[3] = {0.0f, 0.2f, 0.8f};
float Mat1shiny = 160.0f;
float Mat2emit[3] = {0.0f,0.0f,0.0f};
float Mat2spec[3] = {0.7f, 0.7f, 0.7f};
float Mat2nonspec[3] = {0.3f, 0.9f, 0.0f};
float Mat2shiny = 60.0f;
float Mat3spec[3] = {0.7f, 0.7f, 0.7f};
float Mat3nonspec[3] = {0.9f, 0.0f, 0.6f};
float Mat3shiny = 512.0f;
float Mat4spec[3] = {1.0f, 1.0f, 1.0f};
float Mat4nonspec[3] = {0.0f, 0.0f, 0.0f};
float Mat4shiny = 512.0f;


// Data for Viewable Objects  ************************************

const int MAX_OBJECTS = 8;
int NumObjects = 0;;		// Number of viewable objects
ViewableBase* ViewObj[MAX_OBJECTS];

// Vertices of triangle
float tri0verts[3][3] = { {3.0f,1.0f,7.5f}, {4.0f,2.0f,4.5f}, {2.5f,1.8f,7.5f} };
float tri1verts[3][3] = { {10.0f,10.0f,-7.0f}, {-10.0f,10.0f,-7.0f}, {10.0f,-9.0f,-7.0f} };
float tri2verts[3][3] = { {-10.0f,9.0f,-7.0f}, {-10.0f,-10.0f,-7.0f}, {10.0f,-10.0f,-7.0f} };
float tri3verts[3][3] = { {3.0f,3.0f,7.5f}, {4.0f,4.0f,4.5f}, {2.5f,3.8f,7.5f} };
float tri4verts[3][3] = { {2.0f,-4.0f,10.0f}, {5.5f,-2.0f,0.0f}, {1.0f,-1.2f,12.5f} };

// Vertices of parallelogram
// float par0verts[3][3] = { {11.0f,8.0f,-7.0f}, {11.0f,-8.0f,-7.0}, {18.0f, -7.0f ,5.0f} };
 
void SetUpMainView() {
	// Set Up Camera Information
	MainView = new CameraView();
	MainView->SetPosition( Cpos );
	MainView->SetDirection( Cdir );
	MainView->SetScreenDistance( Cdist );
	MainView->SetScreenDimensions( Cdims[0], Cdims[1] );
}


void SetUpMaterials() {
	// Initialize Array of Materials
	MatArray[0] = new Material;
	MatArray[0]->SetColorAmbientDiffuse( Mat0nonspec );
	MatArray[0]->SetColorSpecular( Mat0spec );
	MatArray[0]->SetShininess( Mat0shiny );
	MatArray[0]->SetColorReflective( Mat0reflect );
	MatArray[0]->SetColorTransmissive( Mat0trans );
	MatArray[0]->SetIndexOfRefraction( Mat0refractIndex );
	MatArray[1] = new Material;
	MatArray[1]->SetColorAmbientDiffuse( Mat1nonspec );
	MatArray[1]->SetColorSpecular( Mat1spec );
	MatArray[1]->SetShininess( Mat1shiny );
	MatArray[2] = new Material;
	MatArray[2]->SetColorAmbientDiffuse( Mat2nonspec );
	MatArray[2]->SetColorSpecular( Mat2spec );
	MatArray[2]->SetColorEmissive( Mat2emit );
	MatArray[2]->SetShininess( Mat2shiny );
	MatArray[3] = new Material;
	MatArray[3]->SetColorAmbientDiffuse( Mat3nonspec );
	MatArray[3]->SetColorSpecular( Mat3spec );
	MatArray[3]->SetShininess( Mat3shiny );
	MatArray[4] = new Material;
	MatArray[4]->SetColorAmbientDiffuse( Mat4nonspec );
	MatArray[4]->SetColorSpecular( Mat4spec );
	MatArray[4]->SetShininess( Mat4shiny );

	NumMaterials = 5;
	assert( NumMaterials<=MAX_MATERIALS );
}

void SetUpLights() {
	// Global ambient light and the background color are set above.
	
	// Initialize Array of Lights
	LightArray[0] = new Light();
	LightArray[0]->SetColorAmbient( Lt0amb );
	LightArray[0]->SetColorDiffuse( Lt0diff );
	LightArray[0]->SetColorSpecular( Lt0spec );
	LightArray[0]->SetPosition( Lt0pos );
	LightArray[1] = new Light();
	LightArray[1]->SetColorAmbient( Lt1amb );
	LightArray[1]->SetColorDiffuse( Lt1diff );
	LightArray[1]->SetColorSpecular( Lt1spec );
	LightArray[1]->SetPosition( Lt1pos );

	NumLights = 2;
	assert ( NumLights<=MAX_LIGHTS );
}

void SetUpViewableObjects() {

	// Initialize array of viewable objects

	ViewableSphere* vs = new ViewableSphere;
	vs->SetCenter(0.0,0.0,0.0);
	vs->SetRadius(5.0);
	vs->SetMaterial(MatArray[0]);
	ViewObj[0] = vs;			// Put in the array of visible objects

	ViewableSphere* vs2 = new ViewableSphere;
	vs2->SetCenter(-3.5,1.5,5.3);
	vs2->SetRadius(1.0);
	vs2->SetMaterial(MatArray[1]);
	ViewObj[1] = vs2;

	ViewableTriangle* vt = new ViewableTriangle;
	vt->Init(&tri0verts[0][0]);
	vt->SetMaterial(MatArray[2]);
	ViewObj[2] = vt;

	vt = new ViewableTriangle;
	vt->Init(&tri1verts[0][0]);
	vt->SetMaterial(MatArray[2]);
	ViewObj[3] = vt;
	TextureCheckered* txchecked = new TextureCheckered();
	txchecked->SetMaterial1(MatArray[3]);
	txchecked->SetWidths(0.125,0.125);
	vt->TextureMap(txchecked);

	vt = new ViewableTriangle;
	vt->Init(&tri2verts[0][0]);
	vt->SetMaterial(MatArray[2]);
	ViewObj[4] = vt;
	vt->TextureMap(txchecked);

	vt = new ViewableTriangle;
	vt->Init(&tri3verts[0][0]);
	vt->SetMaterial(MatArray[2]);
	ViewObj[5] = vt;

	vt = new ViewableTriangle;
	vt->Init(&tri4verts[0][0]);
	vt->SetMaterial(MatArray[2]);
	ViewObj[6] = vt;

	//ViewableParallelogram* vp = new ViewableParallelogram;
	//vp->Init(&par0verts[0][0]);
	//vp->SetMaterial(MatArray[4],MatArray[2]);
	//ViewObj[7] = vp;
	//vp->TextureMap(txchecked);

	NumObjects = 7;
	assert( NumObjects <= MAX_OBJECTS );
}

