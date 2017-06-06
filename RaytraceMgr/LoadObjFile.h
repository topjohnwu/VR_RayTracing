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

#ifndef LOAD_OBJ_FILE_H
#define LOAD_OBJ_FILE_H

#include "../DataStructs/Array.h"
#include "SceneDescription.h"
#include "../VrMath/LinearR2.h"
#include "../VrMath/LinearR3.h"
#include "../VrMath/LinearR4.h"

class SceneDescription;
class ObjFileLoader;

// This is the preferred method for loading from obj files.
//    Filename should end with ".obj".
// Returns true if parsing succeeded.
bool LoadObjFile( const char* filename, SceneDescription& theScene );


// ObjFileLoader are intended for future internal use.

class ObjFileLoader {

	friend class NffFileLoader;

public:
	ObjFileLoader();

	// The "Load()" routines reads from the file and includes whatever it
	//	knows how to process into the scene.  (If the scene already includes
	//  items, they are left unchanged.)
	bool Load( const char* filename, SceneDescription& theScene );

private:
	bool ReportUnsupportedFeatures;
	bool UnsupFlagTextureDepth;
	bool UnsupFlagTooManyVerts;
	bool UnsupFlagLines;
	long FileLineNumber;

private:
	SceneDescription* ScenePtr;
	void Reset();

	static char* Preparse( char* inbuf );
	static char* ScanForNonwhite( char* inbuf );
	static char* ScanForWhite( char* inbuf );
	static char* ScanForWhiteOrSlash( char* inbuf );
	static char* ScanForSecondField( char* inbuf );
	static int GetCommandNumber( char *cmd );
	static bool ReadVectorR4Hg( char* inbuf, VectorR4* theVec );
	bool ReadTexCoords( char* inbuf, VectorR2* theVec );
	bool ProcessFace( char *inbuf );
	static int NextTriVertIdx( int start, int* step, int totalNum );

	void UnsupportedTextureDepth();
	void UnsupportedLines();
	void UnsupportedTooManyVerts( int maxVerts );
	void AddUnsupportedCmd( char *cmd );
	void PrintCmdNotSupportedErrors( FILE* outstream );

	Array<VectorR4> Vertices;			// Vertices in homogenous format
	Array<VectorR2> TextureCoords;		// Texture coordinates not supported yet
	Array<VectorR3> VertexNormals;		// Vertex normals not supported yet

	Array<char*> UnsupportedCmds;

};

inline ObjFileLoader::ObjFileLoader()
{
	ReportUnsupportedFeatures = true;
	UnsupFlagTextureDepth = false;
	UnsupFlagTooManyVerts = false;
	UnsupFlagLines = false;
}


#endif // LOAD_OBJ_FILE_H