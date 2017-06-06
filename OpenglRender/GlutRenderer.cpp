/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006.
 *		OpenglRender subpackage
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

#include "GlutRenderer.h"

#include "../Graphics/CameraView.h"
#include "../Graphics/ViewableBezierSet.h"
#include "../Graphics/ViewableCone.h"
#include "../Graphics/ViewableCylinder.h"
#include "../Graphics/ViewableEllipsoid.h"
#include "../Graphics/ViewableParallelepiped.h"
#include "../Graphics/ViewableParallelogram.h"
#include "../Graphics/ViewableSphere.h"
#include "../Graphics/ViewableTorus.h"
#include "../Graphics/ViewableTriangle.h"
#include "../RaytraceMgr/SceneDescription.h"

GlutRenderer::GlutRenderer()
{
	GlobalAmbientLight.Set( 0.1, 0.1, 0.1 );
	SetBrightnessFactor(1.0);
	MeshCount = 20;

	NumLightsUsed = 0;
	GLint numLightsAllowed;
	glGetIntegerv( GL_MAX_LIGHTS, &numLightsAllowed );
	NumLightsAllowed = (int)numLightsAllowed;

}

// Multiplies all material coefficients by this factor. Default = 1.
// Compensates for lack of bounces adding more brightness.
void GlutRenderer::SetBrightnessFactor( double factor ) {
	BrightnessMultiplier = factor;
} 

// Sets up orthographic or persepective transformations based on the
//		camera.  This sets the Projection matrix, and the ModelView matrix,
//		and leaves the ModelView matrix as the current matrix.
void GlutRenderer::SetupCameraView( const CameraView& theCameraView )
{
	SetupCameraView( theCameraView, theCameraView.GetNearClippingDist(), theCameraView.GetFarClippingDist() );
}

void GlutRenderer::SetupCameraView( const CameraView& theCameraView, double nearDistance )
{
	SetupCameraView( theCameraView, nearDistance, theCameraView.GetFarClippingDist() );
}

void GlutRenderer::SetupCameraView( const CameraView& theCameraView, double nearDistance, double farDistance )
{
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	if ( theCameraView.IsDirectional() ) {
		double right = 0.5*theCameraView.GetScreenWidth();
		double top = 0.5*theCameraView.GetScreenHeight();
		glOrtho( -right, right, -top, top, nearDistance, farDistance);
	}
	else {
		double aspectRatio = theCameraView.GetAspectRatio();
		double theta = 2.0*atan( 0.5*theCameraView.GetScreenHeight()/theCameraView.GetScreenDistance() );
		gluPerspective( RadiansToDegrees*theta, aspectRatio, nearDistance, farDistance );
	}

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	VectorR3 up = theCameraView.GetPixeldV();
	up.Normalize();
	const VectorR3& eye = theCameraView.GetPosition();
	const VectorR3& cntr = theCameraView.GetScreenCenter();
	gluLookAt( eye.x, eye.y, eye.z, cntr.x, cntr.y, cntr.z, up.x, up.y, up.z );

	ApplyGlobalAmbientLight();
	ApplyBackgroundColor();
	InitRendering();

	NumLightsUsed = 0;
}

void GlutRenderer::SetGlobalAmbientLight( const VectorR3& ambientLight )
{
	GlobalAmbientLight = ambientLight;
	ApplyGlobalAmbientLight();
}

void GlutRenderer::SetGlobalAmbientLight( double ambientRed, double ambientGreen, double ambientBlue )
{
	GlobalAmbientLight.Set( ambientRed, ambientGreen, ambientBlue );
	ApplyGlobalAmbientLight();
}

void GlutRenderer::ApplyGlobalAmbientLight() {
	GLfloat temp[4];							// Risky: GLfloat must be C++ double or float to work (because of Dump).
	GlobalAmbientLight.Dump(temp);
	temp[3] = 1.0;
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, temp );
}

void GlutRenderer::SetBackgroundColor( const VectorR3& backgroundColor )
{
	BackgroundColor = backgroundColor;
	ApplyBackgroundColor();
}

void GlutRenderer::SetBackgroundColor( double backRed, double backGreen, double backBlue )
{
	BackgroundColor.Set( backRed, backGreen, backBlue );
	ApplyBackgroundColor();
}

void GlutRenderer::ApplyBackgroundColor() {
	glClearColor( BackgroundColor.x, BackgroundColor.y, BackgroundColor.z, 1.0 );
}

bool GlutRenderer::AddLight( const Light& light )
{
	if ( NumLightsUsed>=NumLightsAllowed ) {
		return false;
	}
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);		// Phong light backsides


	GLfloat temp[4];								// Risky: "GLfloat" must be C++ "double" or "float" to work
													// If not, add new "Dump(temp)" to VectorR3 class.
	GLenum lightID = GL_LIGHT0 + NumLightsUsed;		// GL_LIGHTi, where i = NumLightsUsed
	glEnable( lightID );
	light.GetPosition().Dump(temp);
	temp[3] = 1.0;									// homogeneous coord!
	glLightfv( lightID, GL_POSITION, temp );
	light.GetColorAmbient().Dump(temp);
	glLightfv( lightID, GL_AMBIENT, temp );
	light.GetColorDiffuse().Dump(temp);
	glLightfv( lightID, GL_DIFFUSE, temp );
	light.GetColorSpecular().Dump(temp);
	glLightfv( lightID, GL_SPECULAR, temp );
	if ( light.SpotActive() ) {
		light.GetSpotDirection().Dump(temp);
		glLightfv( lightID, GL_SPOT_DIRECTION, temp );
		glLightf( lightID, GL_SPOT_EXPONENT, light.GetSpotExponent() );
		glLightf( lightID, GL_SPOT_CUTOFF, light.GetSpotCutoff() );
	}
	else {
		glLightf( lightID, GL_SPOT_EXPONENT, 0.0 );
		glLightf( lightID, GL_SPOT_CUTOFF, -180.0 );
	}
	glLightf( lightID, GL_CONSTANT_ATTENUATION, light.GetAttenuateConstant() );
	glLightf( lightID, GL_LINEAR_ATTENUATION, light.GetAttenuateLinear() );
	glLightf( lightID, GL_QUADRATIC_ATTENUATION, light.GetAttenuateQuadratic() );

	NumLightsUsed++;
	return true;
}

void GlutRenderer::RenderScene( const SceneDescription& scene ) {
	InitLightsAndView( scene );
	RenderViewables( scene );
	FinishRendering();
}

void GlutRenderer::InitLightsAndView( const SceneDescription& scene )
{
	// Set camera position
	SetBackgroundColor( scene.BackgroundColor() );
	SetupCameraView( scene.GetCameraView() );

	// Define all light sources
	int i;
	int numLights = scene.NumLights();
	for ( i=0; i<numLights; i++ ) {
		AddLight( scene.GetLight(i) );
	}
	SetGlobalAmbientLight( scene.GlobalAmbientLight() );
}

void GlutRenderer::InitRendering()
{
	glEnable( GL_LIGHTING );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_DEPTH_TEST );
}

void GlutRenderer::RenderViewables( const SceneDescription& scene ) {
	RenderViewables( scene.GetViewableArray() );
}

void GlutRenderer::RenderViewables( const Array<ViewableBase*>& viewables ) {
	// Render each ViewableObject
	int numObjects = viewables.SizeUsed();
	for ( long i=0; i<numObjects; i++ ) {
		RenderViewable( *(viewables[i]) );
	}
} 

void GlutRenderer::RenderViewable( const ViewableBase& object )
{
	switch ( object.GetViewableType() ) {
		case ViewableBase::Viewable_BezierSet:
			RenderViewableBezierSet( (const ViewableBezierSet&)object );
			break;
		case ViewableBase::Viewable_Cone:
			RenderViewableCone( (const ViewableCone&)object );
			break;
		case ViewableBase::Viewable_Cylinder:
			RenderViewableCylinder( (const ViewableCylinder&)object );
			break;
		case ViewableBase::Viewable_Ellipsoid:
			RenderViewableEllipsoid( (const ViewableEllipsoid&)object );
			break;
		case ViewableBase::Viewable_Parallelepiped:
			RenderViewableParallelepiped( (const ViewableParallelepiped&)object );
			break;
		case ViewableBase::Viewable_Parallelogram:
			RenderViewableParallelogram( (const ViewableParallelogram&)object );
			break;
		case ViewableBase::Viewable_Sphere:
			RenderViewableSphere( (const ViewableSphere&)object );
			break;
		case ViewableBase::Viewable_Torus:
			RenderViewableTorus( (const ViewableTorus&)object );
			break;
		case ViewableBase::Viewable_Triangle:
			RenderViewableTriangle( (const ViewableTriangle&)object );
			break;
		default:
			assert(0);
	}
}

void GlutRenderer::FinishRendering()
{
	glFlush();
	glutSwapBuffers();
}


// The routines below are usually not called directly: usually "RenderViewable" above
//		is easier to use.

void GlutRenderer::RenderViewableBezierSet( const ViewableBezierSet& object )
{
	glEnable( GL_NORMALIZE );
	glEnable( GL_AUTO_NORMAL );
	glEnable( GL_MAP2_VERTEX_4 );
	GLint theFrontFaceType;
	glGetIntegerv( GL_FRONT_FACE, &theFrontFaceType );
	glFrontFace( GL_CW );	// Make CW setting to avoid OpenGL bug on Bezier patches.

	// Set material properties
	SetFrontMaterial ( object.GetMaterialFront() );
	SetBackMaterial( object.GetMaterialBack() );
	
	// Loop over all Bezier patches in the BezierSet
	const BezierArray& thePatches = object.GetPatchList();
	long numPatches = thePatches.SizeUsed();
	GLdouble theControlPts[16][4];							// GLdouble must be double for this to work with Dump
	for ( long i=0; i<numPatches; i++ ) {
		const VectorR4* cntlPts = thePatches[i].GetControlPoints();
		for ( int j=0; j<16 ; j++ ) {
			(cntlPts+j)->Dump( &theControlPts[j][0] );
		}
		glMap2d(GL_MAP2_VERTEX_4, 0,1,4,4, 0,1,16,4, &theControlPts[0][0] );
		glMapGrid2d( MeshCount, 0,1, MeshCount, 0, 1);
		glEvalMesh2(GL_FILL, 0, MeshCount, 0, MeshCount);
	}

	glFrontFace( theFrontFaceType );	// Restore old Front Face setting (GL_CCW or GL_CW).
}

void GlutRenderer::RenderViewableCone( const ViewableCone& object )
{
	// Set material properties for sides of cone
	SetFrontMaterial ( object.GetMaterialSideOuter() );
	SetBackMaterial( object.GetMaterialSideInner() );

	// Get info about the cone
	const VectorR3& apex = object.GetApex();			// Apex of cone
	const VectorR3& axisC = object.GetCenterAxis();		// Unit vector pointing out of top code
	const VectorR3& axisA = object.GetInvScaledAxisA();	// First side axis (scaled by SlopeA)
	const VectorR3& axisB = object.GetInvScaledAxisB();	// Second side axis (scaled by SlopeB)
	VectorR3 baseN;										// Normal of base plane
	double baseD;										// Coef. constant for base plane
	object.GetBaseFace( &baseN, &baseD );

	// Draw the cone
	VectorR3 basePt1, basePt2;
	VectorR3 normal1, normal2;
	int i;
	CalcConeBasePt( 0.0, baseN, baseD, apex, axisC, axisA, axisB, &basePt1, &normal1 );
	for ( i=0; i<=MeshCount; i++ ) {
		double theta = (i<MeshCount) ? (PI2*i)/MeshCount : 0.0;
		CalcConeBasePt( theta, baseN, baseD, apex, axisC, axisA, axisB, &basePt2, &normal2 );
		DrawLadder( basePt1, basePt2, apex, apex, normal1, normal2 );
		basePt1 = basePt2;
		normal1 = normal2;
	}

	// Set material properties for base of cone
	SetFrontMaterial ( object.GetMaterialBaseOuter() );
	SetBackMaterial( object.GetMaterialBaseInner() );

	// Draw the base
	glBegin( GL_TRIANGLE_FAN );
	glNormal3d( baseN.x, baseN.y, baseN.z );
	// Draw center point
	basePt1 = apex - (((apex^baseN)-baseD)/(axisC^baseN))*axisC;	// Central point on base
	PutVertex( basePt1 );
	for (i=0; i<=MeshCount; i++ ) {
		double theta = (i<MeshCount) ? (PI2*i)/MeshCount : 0.0;
		CalcConeBasePt( theta, baseN, baseD, apex, axisC, axisA, axisB, &basePt1, &normal1 );
		PutVertex( basePt1 );
	}
	glEnd();

}

// Compute point on base of the cone at the angle theta.
void GlutRenderer::CalcConeBasePt( double theta, const VectorR3& baseN, double baseD, 
						 const VectorR3& apex, const VectorR3& axisC, 
						 const VectorR3& axisA, const VectorR3& axisB, 
						 VectorR3* basePt1, VectorR3* normal1 )
{
	// Compute base boundary pt
	VectorR3 u = (cos(theta)/axisA.NormSq())*axisA + (sin(theta)/axisB.NormSq())*axisB;
	double slopeZ = -(u^baseN)/(axisC^baseN);
	u += slopeZ*axisC;					// Add on central axis component
	double centerDepth = ((apex^baseN)-baseD)/(axisC^baseN);
	double beta = centerDepth/(1+slopeZ);
	*basePt1 = beta*u;
	*basePt1 += apex - centerDepth*axisC;

	// Compute normal
	*normal1 = (cos(theta))*axisA;
	*normal1 += (sin(theta))*axisB;
	*normal1 += axisC;
}


void GlutRenderer::RenderViewableCylinder( const ViewableCylinder& object )
{
	glEnable( GL_NORMALIZE );

	// Get info about the cylinder
	const VectorR3& center = object.GetCenter();		// A point on the center axis
	const VectorR3& axisC = object.GetCenterAxis();		// Central axis (unit vector)
	const VectorR3& axisA = object.GetInvScaledAxisA();	// First side axis (scaled by 1/RadiusA)
	const VectorR3& axisB = object.GetInvScaledAxisB();	// Second side axis (scaled by 1/RadiusB)
	VectorR3 baseN;										// Normal of base plane
	double baseD;										// Coef. constant for base plane
	VectorR3 topN;										// Normal of top plane
	double topD;										// Coef. constant for top plane
	object.GetBottomFace( &baseN, &baseD );
	object.GetTopFace( &topN, &topD );

	// Set material properties for side of cylinder
	SetFrontMaterial ( object.GetMaterialSideOuter() );
	SetBackMaterial( object.GetMaterialSideInner() );

	int i;
	VectorR3 basePt1, basePt2, topPt1, topPt2;
	VectorR3 normal1, normal2;
	CalcCylBasePt( 0.0, baseN, baseD, center, axisC, axisA, axisB, topN, topD, &basePt1, &normal1 );
	CalcCylTopPt( 0.0, baseN, baseD, center, axisC, axisA, axisB, topN, topD, &topPt1 );
	for ( i=0; i<=MeshCount; i++ ) {
		double theta = (i<MeshCount) ? (PI2*i)/MeshCount : 0.0;
		CalcCylBasePt( theta, baseN, baseD, center, axisC, axisA, axisB, topN, topD, &basePt2, &normal2 );
		CalcCylTopPt( theta, baseN, baseD, center, axisC, axisA, axisB, topN, topD, &topPt2 );
		DrawLadder( basePt1, basePt2, topPt1, topPt2, normal1, normal2 );
		basePt1 = basePt2;
		topPt1 = topPt2;
		normal1 = normal2;
	}

	// Set material properties for base of cylinder
	SetFrontMaterial ( object.GetMaterialBottomOuter() );
	SetBackMaterial( object.GetMaterialBottomInner() );

	// Draw the base
	glBegin( GL_TRIANGLE_FAN );
	glNormal3d( baseN.x, baseN.y, baseN.z );
	// Draw center point
	basePt1 = center - (((center^baseN)-baseD)/(axisC^baseN))*axisC;	// Central point on base
	if ( (basePt1^topN)<topD ) {
		PutVertex( basePt1 );			// If inside the cylinder, use as center of triangle fan
	}
	for (i=0; i<=MeshCount; i++ ) {
		double theta = (i<MeshCount) ? (PI2*i)/MeshCount : 0.0;
		CalcCylBasePt( theta, baseN, baseD, center, axisC, axisA, axisB, topN, topD, &basePt1, &normal1 );
		PutVertex( basePt1 );
	}
	glEnd();

	// Set material properties for top of cylinder
	SetFrontMaterial ( object.GetMaterialTopOuter() );
	SetBackMaterial( object.GetMaterialTopInner() );

	// Draw the top
	glBegin( GL_TRIANGLE_FAN );
	glNormal3d( topN.x, topN.y, topN.z );
	// Draw center point
	basePt1 = center - (((center^topN)-topD)/(axisC^topN))*axisC;	// Central point on base
	if ( (basePt1^baseN)<baseD ) {
		PutVertex( basePt1 );
	}
	for (i=0; i<=MeshCount; i++ ) {
		double theta = (i<MeshCount) ? (PI2*i)/MeshCount : 0.0;
		CalcCylBasePt( theta, baseN, baseD, center, axisC, axisA, axisB, topN, topD, &basePt1, &normal1 ); // Get the normal!
		CalcCylTopPt( theta, baseN, baseD, center, axisC, axisA, axisB, topN, topD, &basePt1 );
		PutVertex( basePt1 );
	}
	glEnd();
}

void GlutRenderer::CalcCylBasePt( double theta, const VectorR3& baseN, double baseD, 
						const VectorR3& center, const VectorR3& axisC, 
						const VectorR3& axisA, const VectorR3& axisB, 
						const VectorR3& topN, double topD, 
						VectorR3* basePt1, VectorR3* normal1 )
{
	VectorR3 baseCenter = center - (((center^baseN)-baseD)/(axisC^baseN))*axisC;  // center of base
	VectorR3 u = (cos(theta)/axisA.NormSq())*axisA + (sin(theta)/axisB.NormSq())*axisB;
	double slopeZ = -(u^baseN)/(axisC^baseN);
	u += slopeZ*axisC;					// Add on central axis component
	u += baseCenter;
	if ( (u^topN)>topD ) {
		double a = (u^topN)-topD;
		double b = (baseCenter^topN)-topD;
		u = Lerp( u, baseCenter, a/(a-b) );
	}
	*basePt1 = u;

	// Calculate normal
	// Compute normal
	*normal1 = cos(theta)*axisA;
	*normal1 += sin(theta)*axisB;
}

void GlutRenderer::CalcCylTopPt ( double theta, const VectorR3& baseN, double baseD, 
						const VectorR3& center, const VectorR3& axisC, 
						const VectorR3& axisA, const VectorR3& axisB, 
						const VectorR3& topN, double topD, 
						VectorR3* basePt1 )
{
	VectorR3 topCenter = center - (((center^topN)-topD)/(axisC^topN))*axisC;  // center of base
	VectorR3 u = (cos(theta)/axisA.NormSq())*axisA + (sin(theta)/axisB.NormSq())*axisB;
	double slopeZ = -(u^topN)/(axisC^topN);
	u += slopeZ*axisC;					// Add on central axis component
	u += topCenter;
	if ( (u^baseN)>baseD ) {
		double a = (u^baseN)-baseD;
		double b = (topCenter^baseN)-baseD;
		u = Lerp( u, topCenter, a/(a-b) );
	}
	*basePt1 = u;
}

void GlutRenderer::RenderViewableEllipsoid( const ViewableEllipsoid& object )
{
	glEnable( GL_NORMALIZE );
	
	// Set material properties
	SetFrontMaterial ( object.GetMaterialOuter() );
	SetBackMaterial( object.GetMaterialInner() );
	
	const VectorR3& axisA = object.GetScaledInvCentralAxis();	
	const VectorR3& axisB = object.GetScaledInvAxisB();
	const VectorR3& axisC = object.GetScaledInvAxisC();
	const VectorR3& cPos = object.GetCenter();

	glPushMatrix();
	double temp[16];
	axisB.Dump( temp );		// First column of matrix
	temp[3] = 0.0;
	axisC.Dump( temp+4 );	// Second column of matrix
	temp[7] = 0.0;
	axisA.Dump( temp+8 );	// Third column of matrix
	temp[11] = 0.0;
	cPos.Dump( temp+12 );	// Fourth column of matrix (with translation)
	temp[15] = 1.0;
	glMultMatrixd( temp );
	glScaled( Square(object.GetRadiusB()), Square(object.GetRadiusC()), Square(object.GetRadiusA()) );
	glutSolidSphere( 1.0, MeshCount, MeshCount );
	glPopMatrix();

}

void GlutRenderer::RenderViewableParallelepiped( const ViewableParallelepiped& object )
{
	// Set material properties
	SetFrontMaterial ( &(object.GetMaterialOuter()) );
	SetBackMaterial( &(object.GetMaterialInner()) );

	// Get vertices
	const VectorR3& vA = object.GetVertexA();
	const VectorR3& vB = object.GetVertexB();
	const VectorR3& vC = object.GetVertexC();
	const VectorR3& vD = object.GetVertexD();
	VectorR3 sideB = vB-vA;
	VectorR3 sideC = vC-vA;
	VectorR3 sideD = vD-vA;
	VectorR3 oppositeB = vC + sideD;
	VectorR3 oppositeC = vB + sideD;
	VectorR3 oppositeD = vB + sideC;
	VectorR3 oppositeA = oppositeB + sideB;

	// Draw faces
	glBegin ( GL_QUADS );
	// Front face
	SetNormal( object.GetNormalABC() );
	PutVertex( vA );
	PutVertex( vB );
	PutVertex( oppositeD );
	PutVertex( vC );
	// Back face
	SetNormal ( -object.GetNormalABC() );	
	PutVertex( oppositeC );
	PutVertex( vD );
	PutVertex( oppositeB );
	PutVertex( oppositeA );
	// Left face
	SetNormal( -object.GetNormalACD() );	// Normal points rightward (so negate)
	PutVertex( vA );
	PutVertex( vC );
	PutVertex( oppositeB );
	PutVertex( vD );
	// Right face
	SetNormal( object.GetNormalACD() );	// Normal points rightward 
	PutVertex( oppositeD );
	PutVertex( vB );
	PutVertex( oppositeC );
	PutVertex( oppositeA );
	// Top face
	SetNormal( object.GetNormalABD() );
	PutVertex( oppositeA );
	PutVertex( oppositeB );
	PutVertex( vC );
	PutVertex( oppositeD );
	// Bottom face
	SetNormal( -object.GetNormalABD() );
	PutVertex( vD );
	PutVertex( oppositeC );
	PutVertex( vB );
	PutVertex( vA );
	glEnd ();

}

void GlutRenderer::RenderViewableParallelogram( const ViewableParallelogram& object )
{
	// Set material properties
	SetFrontMaterial ( object.GetMaterialFront() );
	SetBackMaterial( object.GetMaterialBack() );

	// Set normal
	SetNormal( object.GetNormal() );

	// Draw triangle
	double temp[12];
	object.GetVertices( temp );
	glBegin( GL_QUADS );
	glVertex3dv( temp+0 );
	glVertex3dv( temp+3 );
	glVertex3dv( temp+6 );
	glVertex3dv( temp+9 );
	glEnd();
}

void GlutRenderer::RenderViewableSphere( const ViewableSphere& object )
{
	// Set material properties
	SetFrontMaterial ( object.GetMaterialOuter() );
	SetBackMaterial( object.GetMaterialInner() );
	const VectorR3& center = object.GetCenter();
	glPushMatrix();
	glTranslatef( center.x, center.y, center.z );
	// No need to rotate, since not applying texture coordinates
	glutSolidSphere( object.GetRadius(), MeshCount, MeshCount );
	glPopMatrix();
}


void GlutRenderer::RenderViewableTorus( const ViewableTorus& object )
{
	// Set material properties
	SetFrontMaterial ( object.GetMaterialOuter() );
	SetBackMaterial ( object.GetMaterialInner() );
	const VectorR3& center = object.GetCenter();
	glPushMatrix();
	glTranslatef( center.x, center.y, center.z );
	// Rotate central axis correctly (no texture coordinates yet, so no need yet to fully rotate)
	VectorR3 work( 0.0, 0.0, 1.0 );			// Glut renders "up" direction in z-axis direction
	work *= object.GetAxisC();				// Crossproduct
	double workNm = work.NormSq();
	// WARNING: workNm could also be zero for 180 degree rotation needed (no problem for now).
	if ( workNm!=0.0 ) {
		workNm = sqrt(workNm);
		work /= workNm;
		double theta = atan2( workNm, object.GetAxisC().z );
		glRotatef( RadiansToDegrees*theta, work.x, work.y, work.z );
	}
	double majorRadius = object.GetMajorRadius();
	double minorRadius = object.GetMinorRadius();
	glutSolidTorus( minorRadius, majorRadius, MeshCount, MeshCount );
	glPopMatrix();

}


void GlutRenderer::RenderViewableTriangle( const ViewableTriangle& object )
{
	// Set material properties
	SetFrontMaterial ( object.GetMaterialFront() );
	SetBackMaterial( object.GetMaterialBack() );

	// Set normal
	SetNormal( object.GetNormal() );

	// Draw triangle
	double temp[9];
	object.GetVertices( temp );
	glBegin( GL_TRIANGLES );
	glVertex3dv( temp+0 );
	glVertex3dv( temp+3 );
	glVertex3dv( temp+6 );
	glEnd();
}

void GlutRenderer::SetFrontMaterial( const MaterialBase* mat )
{
	if ( mat==0 ) {				// If no front material
		mat = &(Material::Default);
	}
	SetFaceMaterial( GL_FRONT, mat );
}

void GlutRenderer::SetBackMaterial( const MaterialBase* mat )
{
	if ( mat==0 ) {				// If no back material
		mat = &(Material::Default);
	}
	SetFaceMaterial( GL_BACK, mat );
}
	
void GlutRenderer::SetFaceMaterial( GLenum faceID, const MaterialBase* mat )
{
	GLfloat temp[4];							// Risky: must be C++ "float" or "double" 
												// If fails, write new LinearR4::Dump() for new type
	mat->GetColorAmbient().Dump(temp);
	glMaterialfv ( faceID, GL_AMBIENT, temp );
	mat->GetColorDiffuse().Dump(temp);
	glMaterialfv ( faceID, GL_DIFFUSE, temp );
	mat->GetColorSpecular().Dump(temp);
	glMaterialfv ( faceID, GL_SPECULAR, temp );
	mat->GetColorEmissive().Dump(temp);
	glMaterialfv ( faceID, GL_EMISSION, temp );

	glMaterialf( faceID, GL_SHININESS, Min( mat->GetPhongShininess(), 127.00 ) );   // Clamp to < 128.0 for safety.
}

void GlutRenderer::SetNormal( const VectorR3& normal )
{
	glNormal3d( normal.x, normal.y, normal.z );
}

void GlutRenderer::PutVertex( const VectorR3& vertPos )
{
	glVertex3d ( vertPos.x, vertPos.y, vertPos.z );
}

void GlutRenderer::DrawLadder( const VectorR3& basePt1, const VectorR3& basePt2, 
					 const VectorR3& topPt1, const VectorR3& topPt2, 
					 const VectorR3& normal1, const VectorR3& normal2 )
{
	glBegin( GL_QUAD_STRIP );
	int i;
	glNormal3d( normal1.x, normal1.y, normal1.z );
	PutVertex( basePt1 );
	glNormal3d( normal2.x, normal2.y, normal2.z );
	PutVertex( basePt2 );
	for ( i=1; i<MeshCount; i++ ) {
		double alpha = ((double)i)/((double)MeshCount);
		glNormal3d( normal1.x, normal1.y, normal1.z );
		PutVertex( Lerp( basePt1, topPt1, alpha ) );
		glNormal3d( normal2.x, normal2.y, normal2.z );
		PutVertex( Lerp( basePt2, topPt2, alpha ) );
	}
	glNormal3d( normal1.x, normal1.y, normal1.z );
	PutVertex( topPt1 );
	glNormal3d( normal2.x, normal2.y, normal2.z );
	PutVertex( topPt2 );
	glEnd();
}
