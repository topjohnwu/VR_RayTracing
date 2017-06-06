/*
 *
 * RayTrace Software Package, release 3.1.  December 20, 2006
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

// This tells the Visual C++ 2005 compiler to allow use of fopen, sscnaf, strcpy, etc.
// Undocumented: This must be included *before* the #include of stdio.h (!!)
#define _CRT_SECURE_NO_DEPRECATE 1

#include <stdio.h>
#include "LoadNffFile.h"
#include "LoadObjFile.h"

#include "../Graphics/CameraView.h"
#include "../Graphics/ViewableCone.h"
#include "../Graphics/ViewableCylinder.h"
#include "../Graphics/ViewableSphere.h"
#include "../Graphics/ViewableTriangle.h"

const int numCommands = 14;
char* nffCommandList[numCommands] = 
{ 
	"v",	// View position, direction and field
	"b",	// Background color
	"l",	// Positional light
	"f",	// Color and reflection and transmission
	"c",	// Cylinder and cone
	"s",	// Sphere
	"p",	// Polygon
	"pp",	// Polygon patch (i.e., polygon with vert normals)
			// Patch normals are not supported and are ignored
	"from",			// Parameter to "v" command: eye position
	"at",			// Parameter to "v" command: "look at" position
	"up",			// Parameter to "v" command: "up" direction
	"angle",		// Parameter to "v" command: fov angle
	"hither",		// Parameter to "v" command: near clipping distance
	"resolution"	// Parameter to "v" command: resolution
					// By default, "resolution" is ignored.
};


bool LoadNffFile( const char* filename, SceneDescription& theScene )
{
	NffFileLoader myLoader;
	return myLoader.Load( filename, theScene );
}

bool NffFileLoader::Load( const char* filename, SceneDescription& theScene )
{
	Reset();
	ScenePtr = &theScene;

	FILE* infile = fopen( filename, "r" );
	FileLineNumber = 0;

	if ( !infile ) {
		fprintf(stderr, "LoadNffFile: Unable to open file: %s\n", filename);
		return false;
	}

	const Material* curMaterial = &Material::Default;

	// Information for view ("v") command
	int viewCmdStatus = false;		// True if currently handling a "v" command
	VectorR3 viewPos;
	VectorR3 lookAtPos;
	VectorR3 upVector;
	double fovy;		// Field of view angle (in radians)
	int screenWidth, screenHeight;
	double hither;

	char inbuffer[1026];
	while ( true ) {
		if ( !fgets( inbuffer, 1026, infile ) ) {
			if ( viewCmdStatus ) {
				SetCameraViewInfo( theScene.GetCameraView(),
						viewPos, lookAtPos, upVector, fovy, 
						screenWidth, screenHeight, hither );
			}
			fclose( infile );
			PrintCmdNotSupportedErrors(stderr);
			return true;
		}
		FileLineNumber++;

		char *findStart = PreparseNff( inbuffer );
		if ( findStart==0 ) {
			// Ignore if a comment or a blank line
			if ( viewCmdStatus ) {
				SetCameraViewInfo( theScene.GetCameraView(),
						viewPos, lookAtPos, upVector, fovy, 
						screenWidth, screenHeight, hither );
				viewCmdStatus = false;
			}
			continue;				
		}

		bool parseErrorOccurred = false;		

		char theCommand[17];
		int scanCode = sscanf( inbuffer, "%16s", theCommand );
		if ( scanCode!=1 ) {
			parseErrorOccurred = true;
		}
		int cmdNum = GetCommandNumber( theCommand );		
		if ( cmdNum==-1 ) {
			AddUnsupportedCmd( theCommand );
			continue;
		}
		if ( viewCmdStatus && cmdNum<8 ) {
			SetCameraViewInfo( theScene.GetCameraView(),
						viewPos, lookAtPos, upVector, fovy, 
						screenWidth, screenHeight, hither );
			viewCmdStatus = false;
		}


		char* args = ObjFileLoader::ScanForSecondField( findStart );
		bool ok = true;
		switch ( cmdNum ) {
		case 0:   // 'v' command
			viewCmdStatus = true;
			break;
		case 1:   // 'b' command - background color
			{
				VectorR3 bgColor;
				scanCode = sscanf( args, "%lf %lf %lf", &(bgColor.x), &(bgColor.y), &(bgColor.z) );
				if ( scanCode!=3 ) {
					ok = false;
					break;
				}
				theScene.SetBackGroundColor( bgColor );
			}
			break;
		case 2:	// 'l' command - positional light
			{
				VectorR3 lightPos, lightColor;
				scanCode = sscanf( args, "%lf %lf %lf %lf %lf %lf", 
											&(lightPos.x), &(lightPos.y), &(lightPos.z),
											&(lightColor.x), &(lightColor.y), &(lightColor.z) );
				if ( scanCode==3 || scanCode==6 ) {
					Light* aLight = new Light();
					aLight->SetPosition( lightPos );
					if ( scanCode==6 ) {
						aLight->SetColor( lightColor );
					}
					theScene.AddLight( aLight );
				}
				else {
					ok = false;
				}
			}
			break;
		case 3:		// 'f' command - material properties
			{
				VectorR3 color;			// Material color
				double Kd, Ks;			// Diffuse and specular components
				double shininess;
				double transmission;	// Transmission coefficient
				double indexOfRefraction;
				scanCode = sscanf( args, "%lf %lf %lf %lf %lf %lf %lf %lf", 
									&color.x, &color.y, &color.z, &Kd, &Ks,
									&shininess, &transmission, &indexOfRefraction );
				if ( scanCode==8 ) {
					Material* mat = new Material();
					theScene.AddMaterial( mat );				// theScene can take of deleting this material
					mat->SetColorAmbientDiffuse( Kd*color );
					mat->SetColorSpecular( Ks*color );
					mat->SetShininess( shininess );
					if ( transmission>0.0 ) {
						mat->SetColorTransmissive( transmission, transmission, transmission );
						mat->SetIndexOfRefraction( indexOfRefraction );
					}
					curMaterial = mat;
				}
				else {
					ok = false;
				}
			}
			break;
		case 4:		// 'c' command - cylinder or cone or truncated cone
			{
				VectorR3 baseCenter;
				VectorR3 topCenter;
				double baseRadius;
				double topRadius;
				scanCode = sscanf( args, "%lf %lf %lf %lf %lf %lf %lf %lf",
									&baseCenter.x, &baseCenter.y, &baseCenter.z, &baseRadius,
									&topCenter.x, &topCenter.y, &topCenter.z, &topRadius );
				if ( scanCode==8 ) {
					ProcessConeCylNFF( baseCenter, baseRadius, topCenter, topRadius );
				}
				else { 
					ok = false;
				}
			}
		case 5:		// 's' command - sphere
			{
				VectorR3 sphereCenter;
				double radius;
				scanCode = sscanf( args, "%lf %lf %lf %lf", &sphereCenter.x, &sphereCenter.y, &sphereCenter.z, &radius );
				if ( scanCode==4 && radius>0.0 ) {
					ViewableSphere* vs = new ViewableSphere( sphereCenter, radius, curMaterial );
					theScene.AddViewable( vs );
				}
				else {
					ok = false;
				}
			}
			break;
		case 7:		// 'pp' command - normals will be ignored
			UnsupportedNormals();
			// Fall thru to 'p' command.
		case 6:		// 'p' command
			{
				int numVerts;
				const int maxNumVerts = 256;
				scanCode = sscanf( args, "%d", &numVerts );
				if (scanCode!=1 || numVerts<3 ) {
					ok = false;
				}
				else if ( numVerts>maxNumVerts ) {
					UnsupportedTooManyVerts( maxNumVerts );
				}
				else {
					ProcessFaceNFF( numVerts, curMaterial, infile );
				}
			}
			break;
		case 8:		// 'from' command
			{
				scanCode = sscanf( args, "%lf %lf %lf", &(viewPos.x), &(viewPos.y), &(viewPos.z) );
				if ( scanCode!=3 || !viewCmdStatus ) {
					ok = false;
					viewCmdStatus = false;
				}
				break;
			}
		case 9:		// 'lookat' command
			{
				scanCode = sscanf( args, "%lf %lf %lf", &(lookAtPos.x), &(lookAtPos.y), &(lookAtPos.z) );
				if ( scanCode!=3 || !viewCmdStatus ) {
					ok = false;
					viewCmdStatus = false;
				}
				break;
			}
		case 10:		// 'up' command
			{
				scanCode = sscanf( args, "%lf %lf %lf", &(upVector.x), &(upVector.y), &(upVector.z) );
				if ( scanCode!=3 || !viewCmdStatus ) {
					ok = false;
					viewCmdStatus = false;
				}
				break;
			}
		case 11:		// 'angle' command
			{
				scanCode = sscanf( args, "%lf", &fovy );
				if ( scanCode!=1 || !viewCmdStatus ) {
					ok = false;
					viewCmdStatus = false;
				}
				else {
					fovy *= PI/180.0;		// Convert to radians
				}
				break;
			}
		case 12:		// 'hither' command
			{
				scanCode = sscanf( args, "%lf", &hither );
				if ( scanCode!=1 || !viewCmdStatus ) {
					ok = false;
					viewCmdStatus = false;
				}
				break;
			}
		case 13:		// 'resolution' command
			{
				scanCode = sscanf( args, "%d %d", &screenWidth, &screenHeight );
				if ( scanCode!=2 || !viewCmdStatus ) {
					ok = false;
					viewCmdStatus = false;
				}
				break;
			}
		default:
			parseErrorOccurred  = true;
			ok = false;
			break;
		}

		if ( !ok ) {
			fprintf(stderr, "Parse error in NFF file, line %ld: %40s.\n", FileLineNumber, inbuffer );
			parseErrorOccurred = true;
		}

	}
}

bool NffFileLoader::ProcessFaceNFF( int numVerts, const Material* mat, FILE* infile )
{
	VectorR3 firstVert, prevVert, thisVert;
	if ( !ReadVertexR3(firstVert, infile) ) {
		return false;
	}
	if ( !ReadVertexR3(prevVert, infile) ) {
		return false;
	}
	int i;
	for ( i=2; i<numVerts; i++ ) {
		if ( !ReadVertexR3(thisVert, infile) ) {
			return false;
		}
		ViewableTriangle* vt = new ViewableTriangle();
		vt->Init( firstVert, prevVert, thisVert );
		if ( vt->IsWellFormed() ) {
			vt->SetMaterial( mat );
			ScenePtr->AddViewable( vt );
		}
		prevVert = thisVert;
	}
	return true;
}

bool NffFileLoader::ReadVertexR3( VectorR3& vert, FILE* infile )
{
	char inbuffer[258];
	if ( !fgets( inbuffer, 256, infile ) ) {
		return false;
	}
	int scanCode;
	scanCode = sscanf( inbuffer, "%lf %lf %lf", &vert.x, &vert.y, &vert.z );
	return (scanCode == 3);
}

void NffFileLoader::ProcessConeCylNFF( const VectorR3& baseCenter, double baseRadius, 
							const VectorR3& topCenter, double topRadius )
{
	bool isCone = (topRadius==0.0);
	if ( !isCone && topRadius!=baseRadius ) {
		UnsupportedTruncatedCone();
		if ( topRadius<0.5*baseRadius ) {
			isCone = true;				// Render as a true code
		}
		else {
			topRadius = baseRadius;   // Render as a cylinder
		}
	}
	VectorR3 centerLine = topCenter;
	centerLine -= baseCenter;
	double height = centerLine.Norm();
	if ( height==0.0 ) {
		return;				// We do not support zero height cylinders (Nor does NFF!)
	}
	centerLine /= height;	// Normalize
	if ( isCone ) {
		ViewableCone* vc = new ViewableCone();
		vc->SetApex(topCenter);
		vc->SetCenterAxis(centerLine);
		vc->SetSlope( baseRadius/height );
		vc->SetHeight( height );
 		ScenePtr->AddViewable(vc);
   }
	else {	
		// Create a cylinder
		ViewableCylinder* vc = new ViewableCylinder();
		vc->SetCenterAxis(centerLine);
		centerLine = topCenter;
		centerLine += baseCenter;
		centerLine *= 0.5;
		vc->SetCenter( centerLine );
		vc->SetRadius( baseRadius );
		vc->SetHeight( height );
		ScenePtr->AddViewable(vc);
	}
}

void NffFileLoader::SetCameraViewInfo( CameraView& theView,
						const VectorR3& viewPos, const VectorR3& lookAtPos, 
						const VectorR3& upVector, double fovy,
						int screenWidth, int screenHeight, double nearClipping )
{
	theView.SetLookAt( viewPos, lookAtPos, upVector );
	if ( IgnoreResolution ) {
		double screenDistance = theView.GetScreenDistance();
		// NFF files always use aspect ratio of one
		// Resize view of theView to encompass the NFF window and more
		// But do not change the aspect ratio or pixel dimensions
        double nffScreenSize = 2.0*screenDistance*tan(fovy*0.5);
		double r = theView.GetAspectRatio();
		if ( r<=1.0 ) {
			theView.SetScreenDimensions( nffScreenSize, nffScreenSize/r );
		}
		else {
			theView.SetScreenDimensions( r*nffScreenSize, nffScreenSize );
		}
	}
	else {
		theView.SetViewFrustum( 1.0, fovy );
		theView.SetScreenPixelSize( screenWidth, screenHeight );
	}
}

char* NffFileLoader::PreparseNff( char* inbuf ) 
{
	// Change white space to real spaces
	// Change '#' into end of line
	char *s;
	for ( s=inbuf; *s!=0; s++ ) {
		if ( *s=='\t' || *s=='\n' ) {
			*s = ' ';
		}
		else if ( *s=='#' ) {
			*s = 0;
			break;
		}
	}
	return ObjFileLoader::ScanForNonwhite( inbuf );
}

int NffFileLoader::GetCommandNumber( char *cmd ) {
	long i;
	for ( i=0; i<numCommands; i++ ) {
		if ( strcmp( cmd, nffCommandList[i] ) == 0 ) {
			return i;
		}
	}
	return -1;		// Command not found
}


void NffFileLoader::Reset()
{
	for ( long i=0; i<UnsupportedCmds.SizeUsed(); i++ ) {
		delete UnsupportedCmds[i];
	}
	UnsupportedCmds.Reset();
}

void NffFileLoader::UnsupportedTooManyVerts( int maxVerts)
{ 
	if ( (!UnsupFlagTooManyVerts) && ReportUnsupportedFeatures ) {
		fprintf(stderr, "NffFileLoader: Faces with more than %d verts not supported. (Line %ld.)\n", maxVerts, FileLineNumber );
		UnsupFlagTooManyVerts = true;
	}
}

void NffFileLoader::UnsupportedNormals()
{ 
	if ( (!UnsupFlagNormals) && ReportUnsupportedFeatures ) {
		fprintf(stderr, "NffFileLoader: Normals on triangles ignored.  ('pp' command, line %ld.)\n", FileLineNumber );
		UnsupFlagNormals = true;
	}
}

void NffFileLoader::UnsupConeCylinderWarning()
{
	if ( (!UnsupFlagConeCylinderWarning) && ReportUnsupportedFeatures ) {
		fprintf( stderr, "NffFileLoader: Cones and Cylinders modelled with end caps.\n");
		UnsupFlagConeCylinderWarning = true;
	}
}

void NffFileLoader::UnsupportedTruncatedCone()
{
	if ( (!UnsupFlagTruncatedCone) && ReportUnsupportedFeatures ) {
		fprintf( stderr, "NffFileLoader: Truncated Cones changed to Cones or Cylinders.\n");
		UnsupFlagTruncatedCone = true;
	}
}

void NffFileLoader::AddUnsupportedCmd( char *cmd )
{
	for ( long i=0; i<UnsupportedCmds.SizeUsed(); i++ ) {
		if ( strcmp( cmd, UnsupportedCmds[i] ) == 0 ) {
			return;
		}
	}
	char* newstring = new char[strlen(cmd)+1];
	strcpy( newstring, cmd );
	UnsupportedCmds.Push( newstring );
}

void NffFileLoader::PrintCmdNotSupportedErrors( FILE* outstream ) {
	if ( !ReportUnsupportedFeatures ) {
		return;
	}
	int numUnsupCmds = UnsupportedCmds.SizeUsed();
	if ( numUnsupCmds > 0 ) {
		fprintf( outstream, "NffFileLoader: Unsupported commands in .nff file:  " );
		for ( long i=0; i<numUnsupCmds; i++ ) {
			if ( i!=0 ) {
				fprintf( outstream, ", " );
			}
			fprintf( outstream, "%s", UnsupportedCmds[i] );
		}
		fprintf( outstream, ".\n" );
	}
}

