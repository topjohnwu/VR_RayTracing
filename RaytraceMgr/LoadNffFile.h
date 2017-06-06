/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006
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

#ifndef LOAD_NFF_FILE_H
#define LOAD_NFF_FILE_H

#include "../DataStructs/Array.h"
#include "SceneDescription.h"
#include "../VrMath/LinearR2.h"
#include "../VrMath/LinearR3.h"
#include "../VrMath/LinearR4.h"

class SceneDescription;
class ObjFileLoader;
class CameraView;

// This is the preferred method for loading from nff (neutral file format) files.
//    Filename should usually end with ".nff".
//    Any items in the SceneDescription already are unaltered.
// Returns true if parsing succeeded.
bool LoadNffFile( const char* filename, SceneDescription& theScene );


// NffFileLoader are intended for future internal use.

class NffFileLoader {

public:
	NffFileLoader();

	// The "Load()" routines reads from the file and includes whatever it
	//	knows how to process into the scene.  (If the scene already includes
	//  items, they are left unchanged.)
	bool Load( const char* filename, SceneDescription& theScene );

	// By default, the screen resolution is ignored. 
	// Change IgnoreResolution to false to have the screen resolution
	//		loaded into the Scene Description
	bool IgnoreResolution;  

private:
	bool ReportUnsupportedFeatures;
	bool UnsupFlagTooManyVerts;
	bool UnsupFlagTruncatedCone;
	bool UnsupFlagNormals;
	bool UnsupFlagConeCylinderWarning;
	long FileLineNumber;

private:
	SceneDescription* ScenePtr;
	void Reset();

	static char* PreparseNff( char* inbuf );
	static int GetCommandNumber( char *cmd );

	void SetCameraViewInfo( CameraView& theView,
							const VectorR3& viewPos, const VectorR3& lookAtPos, 
							const VectorR3& upVector, double fovy,
							int screenWidth, int screenHeight, double nearClipping);

	bool ProcessFaceNFF( int numVerts, const Material* mat, FILE* infile );
	void ProcessConeCylNFF( const VectorR3& baseCenter, double baseRadius, 
							const VectorR3& topCenter, double topRadius );
	static bool ReadVertexR3( VectorR3& vertReturned, FILE* infile );

	//static char* ScanForNonwhite( char* inbuf );
	//static char* ScanForWhite( char* inbuf );
	//static char* ScanForWhiteOrSlash( char* inbuf );
	//static char* ScanForSecondField( char* inbuf );
	//static bool ReadVectorR4Hg( char* inbuf, VectorR4* theVec );
	//bool ReadTexCoords( char* inbuf, VectorR2* theVec );
	//static int NextTriVertIdx( int start, int* step, int totalNum );

	void UnsupportedTooManyVerts( int maxVerts );
	void UnsupportedNormals();
	void UnsupConeCylinderWarning();
	void UnsupportedTruncatedCone();
	void AddUnsupportedCmd( char *cmd );
	void PrintCmdNotSupportedErrors( FILE* outstream );

	Array<char*> UnsupportedCmds;

};

inline NffFileLoader::NffFileLoader()
{
	IgnoreResolution = true;		// By default, ignore the view resolution specification

	ReportUnsupportedFeatures = true;
	UnsupFlagTooManyVerts = false;
	UnsupFlagNormals = false;
	UnsupFlagTruncatedCone = false;
	UnsupFlagConeCylinderWarning = false;
	UnsupFlagTruncatedCone = false;
}


#endif // LOAD_NFF_FILE_H