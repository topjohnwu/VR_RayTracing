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

class VectorR3;
class Light;
class Material;
class ViewableBase;
class CameraView;

// Camera and camera information

extern CameraView* MainView;
extern double Cpos[3];		// Position of camera
extern double Cdir[3];		// Direction of camera
extern double Cdist;		// Distance to "screen"
extern double Cdims[2];		// Width & height of "screen"

// Here are the arrays that hold information about the scene

extern int NumLights;			// Number of lights
extern Light* LightArray[];		// Array of light pointers

extern int NumMaterials;		// Number of materials
extern Material* MatArray[];	// Array of material pointers.

extern int NumObjects;			// Number of objects
extern ViewableBase* ViewObj[];	// Array of viewable object pointers

extern double GlobalAmbientLight[3];
extern double* BackgroundColor;
extern VectorR3 GlobalAmbientR3;
extern VectorR3 BackgroundColorR3;

// Routines that load the data into the above arrays:
void SetUpMainView();
void SetUpMaterials();
void SetUpLights();
void SetUpViewableObjects();
