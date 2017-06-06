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

#include "RayTraceSetup2.h"

#include "../Graphics/Material.h"
#include "../Graphics/ViewableSphere.h"
#include "../Graphics/ViewableEllipsoid.h"
#include "../Graphics/ViewableCone.h"
#include "../Graphics/ViewableTorus.h"
#include "../Graphics/ViewableTriangle.h"
#include "../Graphics/ViewableParallelogram.h"
#include "../Graphics/ViewableCylinder.h"
#include "../Graphics/ViewableParallelepiped.h"
#include "../Graphics/ViewableBezierSet.h"
#include "../Graphics/TextureCheckered.h"
#include "../Graphics/TextureBilinearXform.h"
#include "../Graphics/TextureSequence.h"
#include "../Graphics/Light.h"
#include "../Graphics/CameraView.h"
#include "../RaytraceMgr/SceneDescription.h"

// ******************************************

SceneDescription TheScene2;			// The scene as created by the routines below

// ******************************************


// Some simple colors

double RedColor[3] = {1.0, 0.0, 0.0};
double BlueColor[3] = {0.0, 0.0, 1.0};
double BlackColor[3] = {0.0, 0.0, 0.0};

// Camera Information Data  *****************************************

double Cpos[3] = {0.0,5.0,25.0};	// Position of camera
double Cdir[3] = {0.0,-0.1,-1.0};	// Direction of camera
double Cdist = 25.0;				// Distance to "screen"
double Cdims[2] = {18.0, 10.0};		// Width & height of "screen"
// Use next two to focus on the tori
//double Cdims[2] = {2.0,2.0};		// Width & height of "screen"
//double Cdir[3] = {0.0,-4.9,-20.0};	// Direction of camera

// Data for lights  **********************************************

// Global lighting data

// Lighting values
float Lt0amb[3] = {0.0f, 0.0f, 0.0f};
float Lt0diff[3] = {1.0f, 1.0f, 1.0f};
float Lt0spec[3] = {1.0f, 1.0f, 1.0f};
float Lt0pos[3] = {7.0, 15.0f, 12.0f};

float Lt1amb[3] = {0.0f, 0.0f, 0.0f};
float Lt1diff[3] = {1.0f, 1.0f, 1.0f};
float Lt1spec[3] = {1.0f, 1.0f, 1.0f};
float Lt1pos[3] = {-7.0f, 25.0, 12.0f};

// Data for materials *******************************************

// Material values

// First material for the floor
float Mat0spec[3] = {0.8f, 0.8f, 0.8f};
float Mat0nonspec[3] = {0.5f, 0.5f, 0.0f};
float Mat0reflect[3] = {0.6f, 0.6f, 0.4f};
float Mat0shiny = 512.0f;

// Second material for the floor
float Mat1spec[3] = {0.8f, 0.8f, 0.8f};
float Mat1nonspec[3] = {0.0f, 0.2f, 0.8f};
float Mat1reflect[3] = {0.6f, 0.6f, 0.4f};
float Mat1shiny = 512.0f;

// Material for the side walls
float Mat2spec[3] = {0.2f, 0.2f, 0.2f};
float Mat2nonspec[3] = {0.3f, 0.3f, 0.0f};
float Mat2reflect[3] = {0.8f, 0.8f, 0.4f};
float Mat2shiny = 160.0f;

// Red, mixed with some blue, (i.e., magenta) slightly reflective
float Mat3spec[3] = {0.7f, 0.7f, 0.7f};
float Mat3nonspec[3] = {0.9f, 0.0f, 0.6f};
float Mat3reflect[3] = {0.1f, 0.1f, 0.1f};
float Mat3shiny = 512.0f;

float Mat4spec[3] = {1.0f, 1.0f, 1.0f};
float Mat4nonspec[3] = {0.0f, 0.0f, 0.0f};
float Mat4reflect[3] = {0.1f, 0.1f, 0.1f};
float Mat4shiny = 512.0f;

float Mat5spec[3] = {0.6f, 0.6f, 0.6f};
float Mat5nonspec[3] = {0.0f, 0.0f, 0.0f};
float Mat5reflect[3] = {0.3f, 0.3f, 0.3f};
float Mat5trans[3] = {0.8f, 0.8f, 0.8f};
float Mat5shiny = 512.0f;

float Mat6spec[3] = {0.2f, 0.2f, 0.2f};
float Mat6nonspec[3] = {0.0f, 0.2f, 0.8f};
float Mat6reflect[3] = {0.3f, 0.3f, 0.3f};
float Mat6shiny = 160.0f;

// Black!
float Mat7spec[3] = {0.6f, 0.6f, 0.6f};
float Mat7nonspec[3] = {0.0f, 0.0f, 0.0f};
float Mat7reflect[3] = {0.0f, 0.0f, 0.0f};
float Mat7shiny = 160.0f;

// Completely invisible!
float Mat8spec[3] = {0.0f, 0.0f, 0.0f};
float Mat8nonspec[3] = {0.0f, 0.0f, 0.0f};
float Mat8reflect[3] = {0.0f, 0.0f, 0.0f};
float Mat8trans[3] = {1.0f, 1.0f, 1.0f};

// A near perfect mirror
float Mat9spec[3] = {0.95f, 0.95f, 0.95f};
float Mat9nonspec[3] = {0.05f, 0.05f, 0.05f};
float Mat9reflect[3] = {0.95f, 0.95f, 0.95f};
float Mat9shiny = 160.0f;


// Data for Viewable Objects  ************************************

// Vertices of triangles & rectangles
float par0verts[3][3] = { {-8.0f,0.0f,10.0f}, {8.0f,0.0f,10.0f}, {8.0f,0.0f, -5.0f} };
float par1verts[3][3] = { {-8.0f,0.0f,-5.0f}, {8.0f,0.0f,-5.0f}, {8.0f,10.0f,-5.0f} };
float tri2verts[3][3] = { {-8.0f,0.0f,-5.0f}, {-8.0f,10.0f,-5.0f}, {-8.0f,0.0f,10.0f} };
float tri3verts[3][3] = { {8.0f,0.0f,-5.0f}, {8.0f,0.0f,10.0f}, {8.0f,10.0f,-5.0f} };

void SetUpScene2()
{
	SetUpMainView();
	SetUpMaterials();
	SetUpLights( TheScene2 );
	SetUpViewableObjects();
}
 
void SetUpMainView() {
	// Set Up Camera Information
	TheScene2.GetCameraView().SetPosition( Cpos );
	TheScene2.GetCameraView().SetDirection( Cdir );
	TheScene2.GetCameraView().SetScreenDistance( Cdist );
	TheScene2.GetCameraView().SetScreenDimensions( Cdims[0], Cdims[1] );
}

void SetUpMaterials() {
	// Initialize Array of Materials
	Material* mat0 = new Material;
	TheScene2.AddMaterial( mat0 );
	mat0->SetColorAmbientDiffuse( Mat0nonspec );
	mat0->SetColorSpecular( Mat0spec );
	mat0->SetShininess( Mat0shiny );
	mat0->SetColorReflective( Mat0reflect );

	Material* mat1 = new Material;
	TheScene2.AddMaterial(mat1);
	mat1->SetColorAmbientDiffuse( Mat1nonspec );
	mat1->SetColorSpecular( Mat1spec );
	mat1->SetShininess( Mat1shiny );

	Material* mat2 = new Material;
	TheScene2.AddMaterial(mat2);
	mat2->SetColorAmbientDiffuse( Mat2nonspec );
	mat2->SetColorSpecular( Mat2spec );
	mat2->SetColorReflective( Mat2reflect );
	mat2->SetShininess( Mat2shiny );

	Material* mat3 = new Material;
	TheScene2.AddMaterial(mat3);
	mat3->SetColorAmbientDiffuse( Mat3nonspec );
	mat3->SetColorSpecular( Mat3spec );
	mat3->SetColorReflective( Mat3reflect );
	mat3->SetShininess( Mat3shiny );

	Material* mat4 = new Material;
	TheScene2.AddMaterial(mat4);
	mat4->SetColorAmbientDiffuse( Mat4nonspec );
	mat4->SetColorSpecular( Mat4spec );
	mat4->SetColorReflective( Mat4reflect );
	mat4->SetShininess( Mat4shiny );

	Material* mat5 = new Material;
	TheScene2.AddMaterial(mat5);
	mat5->SetColorAmbientDiffuse( Mat5nonspec );
	mat5->SetColorSpecular( Mat5spec );
	mat5->SetColorReflective( Mat5reflect );
	mat5->SetColorTransmissive( Mat5trans );
	mat5->SetShininess( Mat5shiny );
	mat5->SetIndexOfRefraction(1.5);	// Glass!

	Material* mat6 = new Material;
	TheScene2.AddMaterial(mat6);
	mat6->SetColorAmbientDiffuse( Mat6nonspec );
	mat6->SetColorSpecular( Mat6spec );
	mat6->SetColorReflective( Mat6reflect );
	mat6->SetShininess( Mat6shiny );

	Material* mat7 = new Material;
	TheScene2.AddMaterial(mat7);
	mat7->SetColorAmbientDiffuse( Mat7nonspec );
	mat7->SetColorSpecular( Mat7spec );
	mat7->SetColorReflective( Mat7reflect );
	mat7->SetShininess( Mat7shiny );

	// Perfectly invisible with index of
	//		refraction = 0, reflection = 0, transmission = 1.
	//	Use for two facing pieces of glass.
	Material* mat8 = new Material;
	TheScene2.AddMaterial(mat8);
	mat8->SetColorAmbientDiffuse(Mat8nonspec);
	mat8->SetColorSpecular(Mat8spec);
	mat8->SetColorReflective( Mat8reflect );
	mat8->SetColorTransmissive( Mat8trans );

	// A near perfect mirror surface
	Material* mat9 = new Material;
	TheScene2.AddMaterial(mat9);
	mat9->SetColorAmbientDiffuse(Mat9nonspec);
	mat9->SetColorSpecular(Mat9spec);
	mat9->SetColorReflective( Mat9reflect );
	mat9->SetShininess( Mat9shiny );

}

void SetUpLights( SceneDescription& scene ) {
	// Global ambient light and the background color are set above.
	
	// Initialize Array of Lights
	Light* myLight0 = new Light();
	scene.AddLight( myLight0 );
	myLight0->SetColorAmbient( Lt0amb );
	myLight0->SetColorDiffuse( Lt0diff );
	myLight0->SetColorSpecular( Lt0spec );
	myLight0->SetPosition( Lt0pos );

	Light* myLight1 = new Light();
	scene.AddLight( myLight1 );
	myLight1->SetColorAmbient( Lt1amb );
	myLight1->SetColorDiffuse( Lt1diff );
	myLight1->SetColorSpecular( Lt1spec );
	myLight1->SetPosition( Lt1pos );

}

void SetUpViewableObjects() {

	// Initialize array of viewable objects

	// ViewableSphere* vs = new ViewableSphere;
	// vs->SetCenter(0.0,0.0,0.0);
	// vs->SetRadius(5.0);
	// vs->SetMaterial(MatArray[0]);
	// ViewObj[0] = vs;			// Put in the array of visible objects

	// Flat plane (the floor)
	ViewableParallelogram* vp;
	vp = new ViewableParallelogram();
	vp->Init(&par0verts[0][0]);
	vp->SetMaterial( &TheScene2.GetMaterial(0) );
	TheScene2.AddViewable( vp );

	TextureCheckered* txchecked = TheScene2.NewTextureCheckered();
	txchecked->SetMaterial1( &TheScene2.GetMaterial(1) );
	txchecked->SetWidths(1.0/15.0,0.0625);
	vp->TextureMap(txchecked);		

	// Back wall

	vp = new ViewableParallelogram();
	vp->Init(&par1verts[0][0]);
	vp->SetMaterial( &TheScene2.GetMaterial(7) );
	TheScene2.AddViewable( vp );

	TextureCheckered* txchecked2 = TheScene2.NewTextureCheckered();
	txchecked2->SetMaterial1( &TheScene2.GetMaterial(6) );
	txchecked2->SetWidths(0.5,1.0001);
	TextureBilinearXform* txWarp = TheScene2.NewTextureBilinearXform();
	txWarp->SetTextureCoordA(-8.0, 0.0);
	txWarp->SetTextureCoordB(8.0, 0.0);
	txWarp->SetTextureCoordD(-1.0, 1.0);
	txWarp->SetTextureCoordC(1.2, 1.0);
	// Combine the textures: first warp bilinearly and then apply checker pattern
	TextureSequence* txTwoTextures = TheScene2.NewTextureSequence( txWarp, txchecked2 );
	vp->TextureMap(txTwoTextures);		
	//TextureRgbImage* image = new TextureRgbImage("FireplaceBox.bmp");
    //vp->TextureMap(image); 

	// Left wall (triangular)

	ViewableTriangle* vt;
	vt = new ViewableTriangle();
	vt->Init(&tri2verts[0][0]);
	vt->SetMaterial( &TheScene2.GetMaterial(2) );
	TheScene2.AddViewable( vt );

	// Right wall (triangular)

	vt = new ViewableTriangle;
	vt->Init(&tri3verts[0][0]);
	vt->SetMaterial( &TheScene2.GetMaterial(2) );
	TheScene2.AddViewable( vt );

	// Left checkered sphere
	ViewableSphere* vs;
	vs = new ViewableSphere();
	vs->SetCenter(-7.0,0.5,-4.0);
	vs->SetRadius(0.5);
	vs->SetMaterial( &TheScene2.GetMaterial(3) );
	int smallLeftSphereIdx = TheScene2.AddViewable( vs );
	TextureCheckered* txc2 = TheScene2.NewTextureCheckered();
	txc2->SetMaterial1( &TheScene2.GetMaterial(4) );
	txc2->SetWidths(0.0625,0.125);
	vs->TextureMap(txc2);	
	// Right checkered sphere

	vs = new ViewableSphere();
	vs->SetCenter(7.0,0.5,-4.0);
	vs->SetRadius(0.5);
	vs->SetMaterial( &TheScene2.GetMaterial(3) );
	int smallRightSphereIdx = TheScene2.AddViewable( vs );
	vs->TextureMap(txc2);
	vs->SetuvCylindrical();		// SetuvSpherical is the default

	// Two transparent spheres

	double radS = 1.8;
	double zS = 2.0;
	ViewableSphere* vsX = new ViewableSphere();
	vsX->SetRadius( radS );
	vsX->SetMaterial( &TheScene2.GetMaterial(5) );

	// Left transparent sphere

	vs = new ViewableSphere();
	*vs = *vsX;
	vs->SetCenter( -radS, (sqrt(2.0)*radS), zS );
	TheScene2.AddViewable( vs );

	// Right transparent sphere

	vs = new ViewableSphere();
	*vs = *vsX;
	vs->SetCenter( radS, (sqrt(2.0)*radS), zS );
	TheScene2.AddViewable( vs );

	// Make lots of copies of the left small sphere.

	vsX = (ViewableSphere*)(&TheScene2.GetViewable(smallLeftSphereIdx));
	vs = new ViewableSphere;
	*vs = *vsX;
	vs->SetCenter( -5.0, 0.5, -4.0 );
	TheScene2.AddViewable( vs );

	vs = new ViewableSphere;
	*vs = *vsX;
	vs->SetCenter( -3.0, 0.5, -4.0 );
	TheScene2.AddViewable( vs );

	vs = new ViewableSphere;
	*vs = *vsX;
	vs->SetCenter( -1.0, 0.5, -4.0 );
	TheScene2.AddViewable( vs );

	// Make lots of copies of the right small sphere

	vsX = (ViewableSphere*)(&TheScene2.GetViewable(smallRightSphereIdx));;
	vs = new ViewableSphere;
	*vs = *vsX;
	vs->SetCenter( 5.0, 0.5, -4.0 );
	TheScene2.AddViewable( vs );

	vs = new ViewableSphere;
	*vs = *vsX;
	vs->SetCenter( 3.0, 0.5, -4.0 );
	TheScene2.AddViewable( vs );

	vs = new ViewableSphere;
	*vs = *vsX;
	vs->SetCenter( 1.0, 0.5, -4.0 );
	TheScene2.AddViewable( vs );

	// Try out two test tori

	ViewableTorus* vT = new ViewableTorus();
	vT->SetCenter( -0.3, 0.6, 7.0 );
	vT->SetMaterial( &TheScene2.GetMaterial(5) );
	vT->SetRadii(0.6,0.15);
	vT->SetAxis(VectorR3(0.0,1.0,1.0));
	TheScene2.AddViewable( vT );

	vT = new ViewableTorus();
	vT->SetCenter( 0.3, 0.6, 7.0 );
	vT->SetMaterial( &TheScene2.GetMaterial(0) );
	vT->SetRadii(0.6,0.15);
	vT->SetAxis(VectorR3(0.0,1.0,-1.0));
	TheScene2.AddViewable( vT );

	// Upright right cylinder
	VectorR3 firstCylCenter(-4.0,1.5,6.0);
	ViewableCylinder* vcyl = new ViewableCylinder();
	vcyl->SetCenter(firstCylCenter);
	vcyl->SetCenterAxis(0.0,1.0,0.0); // The default
	vcyl->SetHeight(3.0);
	vcyl->SetRadius(0.25);
	vcyl->SetMaterial( &TheScene2.GetMaterial(3) );
	TheScene2.AddViewable( vcyl );

	VectorR3 cylEndPt(-4.0,1.5,8.0);	// Common end pt. of diagonal cylinders
	VectorR3 cylnormalA(1.0,0.0,0.0);
	double coefA = cylnormalA^cylEndPt;
	VectorR3 cylnormalB(0.0,-1.0,0.0);
	vcyl = new ViewableCylinder();
	vcyl->SetCenter(cylEndPt);
	vcyl->SetCenterAxis(1.0,1.0,0.0);
	vcyl->SetRadialAxes(VectorR3(0.0,0.0,1.0),VectorR3(1.0,-1.0,0.0));
	vcyl->SetRadii(0.1,0.5);
	vcyl->SetTopFace( cylnormalA, coefA );
	vcyl->SetBottomFace( cylnormalB, 0.0 );
	vcyl->SetMaterial( &TheScene2.GetMaterial(5) );
	vcyl->SetMaterialTopInner( &TheScene2.GetMaterial(8) );
	vcyl->SetMaterialTopOuter( &TheScene2.GetMaterial(8) );
	TheScene2.AddViewable( vcyl );

	vcyl = new ViewableCylinder();
	vcyl->SetCenter(cylEndPt);
	vcyl->SetCenterAxis(1.0,-1.0,0.0);
	vcyl->SetRadialAxes(VectorR3(0.0,0.0,1.0),VectorR3(1.0,1.0,0.0));
	vcyl->SetRadii(0.1,0.5);
	vcyl->SetBottomFace( -cylnormalA, -coefA );
	vcyl->SetTopFace( cylnormalB, 0.0 );
	vcyl->SetMaterial( &TheScene2.GetMaterial(5) );
	vcyl->SetMaterialTopInner( &TheScene2.GetMaterial(8) );
	vcyl->SetMaterialTopOuter( &TheScene2.GetMaterial(8) );
	TheScene2.AddViewable( vcyl );

	vcyl = new ViewableCylinder();
	vcyl->SetCenter(-4.0,0.2,8.0);
	vcyl->SetCenterAxis(0.0,0.0,-1.0);
	vcyl->SetRadialAxes(VectorR3(0.0,1.0,0.0),VectorR3(1.0,0.0,0.0));
	vcyl->SetRadii(0.2,0.4);
	vcyl->SetHeight(1.0);
	vcyl->SetMaterial( &TheScene2.GetMaterial(3) );
	TheScene2.AddViewable( vcyl );
	TextureCheckered* txc3 = TheScene2.NewTextureCheckered();
	txc3->SetMaterial1( &TheScene2.GetMaterial(4) );
	txc3->SetWidths(0.125,0.25);
	vcyl->TextureMap(txc3);	

	// Horizontal yellow ellipsoid
	ViewableEllipsoid* ve = new ViewableEllipsoid();
	ve->SetCenter(4.0,2.0,8.0);
	ve->SetMaterial( &TheScene2.GetMaterial(0) );
	ve->SetRadii(0.3,0.6,1.0);		// radii along y axis, z axis & x axis
	TheScene2.AddViewable( ve );
	// Diagonal purple and black ellipsoid
	ve = new ViewableEllipsoid();
	ve->SetCenter(4.0,1.0,9.0);
	ve->SetAxes(VectorR3(1.0,1.0,0.0),VectorR3(0.0,0.0,1.0));
	ve->SetRadii(0.5,0.2,0.8);
	ve->SetMaterial( &TheScene2.GetMaterial(3) );
	TheScene2.AddViewable( ve );
	ve->TextureMap(txc2);
	// Vertical, glass ellipsoid
	ve = new ViewableEllipsoid();
	ve->SetCenter(6.5,2.5,6.0);
	ve->SetRadii(2.5,0.1,0.3);
	ve->SetMaterial( &TheScene2.GetMaterial(5) );
	TheScene2.AddViewable( ve );

	// Cone
	ViewableCone* vcone = new ViewableCone();
	vcone->SetApex(3.0,1.0,6.0);
	vcone->SetSlope(2.0);
	vcone->SetMaterial( &TheScene2.GetMaterial(0) );
	TheScene2.AddViewable( vcone );

	vcone = new ViewableCone();
	vcone->SetApex(3.0, 0.0, 9.0);
	vcone->SetCenterAxis(6.0,-1.0,0.0);
	vcone->SetRadialAxis(VectorR3(0.0,0.0,1.0));
	vcone->SetSlopes(1.0, 6.0);
	vcone->SetMaterial( &TheScene2.GetMaterial(3) );
	vcone->TextureMap(txc2);
	TheScene2.AddViewable( vcone );
	
	vcone = new ViewableCone();
	vcone->SetApex(5.0, 1.0, 9.2);
	vcone->SetCenterAxis(1.0,2.0,0.0);
	vcone->SetSlopes(4.0, 4.0);
	vcone->SetBaseFace(VectorR3(0.0,-1.0,0.0), -0.01);
	vcone->SetMaterial( &TheScene2.GetMaterial(5) );
	vcone->SetMaterialBaseInner( &TheScene2.GetMaterial(3) );
	TheScene2.AddViewable( vcone );

	vcone = new ViewableCone();
	vcone->SetApex(5.0, 1.0, 9.2);
	vcone->SetCenterAxis(-1.0,2.0,0.0);
	vcone->SetSlopes(4.0, 4.0);
	vcone->SetBaseFace(VectorR3(0.0,-1.0,0.0), -0.01);
	vcone->SetMaterial( &TheScene2.GetMaterial(5) );
	vcone->SetMaterialBaseInner( &TheScene2.GetMaterial(3) );
	TheScene2.AddViewable( vcone );

	// Yellow cube
	ViewableParallelepiped* vpiped = new ViewableParallelepiped();
	vpiped->SetVertices(VectorR3(-6.0,6.0,4.0), VectorR3(-5.0,6.0,4.0),
						VectorR3(-6.0,7.0,4.0), VectorR3(-6.0,6.0,3.0) );
	vpiped->SetMaterialOuter( &TheScene2.GetMaterial(0) );
	TheScene2.AddViewable( vpiped );

	// Red & Black checked parallelpiped
	vpiped = new ViewableParallelepiped();
	vpiped->SetVertices(VectorR3(-6.6,6.0,4.5), VectorR3(-6.6,1.0,5.0),
						VectorR3(-6.6,5.5,6.0), VectorR3(-6.3,5.5,4.5) );
	vpiped->SetMaterialOuter( &TheScene2.GetMaterial(3) );
	TheScene2.AddViewable( vpiped );

	// A black checker pattern (superimposed on whatever other material is used)
	TextureCheckered* txc4 = TheScene2.NewTextureCheckered();
	txc4->SetMaterial1( &TheScene2.GetMaterial(4) );	// black
	txc4->SetWidths(0.0625,0.0625);

	// Bezier patch for testing
	double cx=-1.5;
	double cy=1.0;
	double cr = 1.0;
	double cz=7.0;
	// Order 3 x 3 rational Bezier patch
	double cntlPtsA[3][3][4] = {
		{ {cx-cr,cy,cz,1}, {0, 0 ,cr,0}, {cx+cr,cy,cz,1} },
		{ {cx-cr*0.5,cy+0.5,cz,1}, {0, 0 ,cr*0.5,0}, {cx+cr*0.5,cy+0.5,cz,1} },
		{ {cx-cr,cy+1,cz,1}, {0, 0 ,cr,0}, {cx+cr,cy+1,cz,1} }
	};
	double cntlPtsB[3][3][4] = {
		{ {cx+cr,cy,cz,1}, {0, 0 ,-cr,0}, {cx-cr,cy,cz,1} },
		{ {cx+cr*0.5,cy+0.5,cz,1}, {0, 0 ,-cr*0.5,0}, {cx-cr*0.5,cy+0.5,cz,1} },
		{ {cx+cr,cy+1,cz,1}, {0, 0 ,-cr,0}, {cx-cr,cy+1,cz,1} }
	};
	ViewableBezierSet* vBezierSet = new ViewableBezierSet();
	vBezierSet->SetMaterialFront(  &TheScene2.GetMaterial(0) );	// Reflective greenish yellow
	vBezierSet->SetMaterialBack(  &TheScene2.GetMaterial(3) );		// Magenta-ish
	vBezierSet->TextureMapBack( txc4 );
	vBezierSet->AddRationalPatch(3,3,&cntlPtsA[0][0][0]);
	vBezierSet->AddRationalPatch(3,3,&cntlPtsB[0][0][0]);
	TheScene2.AddViewable( vBezierSet );

	// Curved circular bezier patch, mirror in corner
	cx = 8.0;
	cz = -5.0;
	cy = 5.0;
	cr = 1.0;
	double s2i = 1.0/sqrt(2.0);
	double cntlPtsC[4][3][4] = {
		{ {cx-cr, cy-1.0, cz, 1}, {s2i*(cx-cr), s2i*(cy-1.0), s2i*(cz+cr), s2i}, {cx, cy-1.0, cz+cr, 1} },
		{ {cx-1.5*cr, cy, cz, 1}, {s2i*(cx-1.5*cr), s2i*(cy), s2i*(cz+1.5*cr), s2i}, {cx,cy, cz+1.5*cr, 1} },
		{ {cx-1.6*cr, cy+0.5, cz, 1}, {s2i*(cx-1.6*cr), s2i*(cy+0.5), s2i*(cz+1.6*cr), s2i}, {cx,cy+0.5, cz+1.6*cr, 1} },
		{ {cx-cr, cy+1.0, cz, 1}, {s2i*(cx-cr), s2i*(cy+1.0), s2i*(cz+cr), s2i}, {cx,cy+1.0, cz+cr, 1} },
	};
	vBezierSet = new ViewableBezierSet();
	vBezierSet->SetMaterialFront( &TheScene2.GetMaterial(9) );	// near perfect mirror
	vBezierSet->SetMaterialBack(  &TheScene2.GetMaterial(4) );	// black
	vBezierSet->AddRationalPatch(3,4,&cntlPtsC[0][0][0]);
	TheScene2.AddViewable( vBezierSet );
}

