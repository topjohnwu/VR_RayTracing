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
#include "LoadObjFile.h"

#include "../Graphics/ViewableParallelogram.h"
#include "../Graphics/ViewableTriangle.h"

const int numCommands = 4;
char* commandList[numCommands] = 
{ 
	"v",
	"vt",
	"f",
	"l"
};


bool LoadObjFile( const char* filename, SceneDescription& theScene )
{
	ObjFileLoader myLoader;
	return myLoader.Load( filename, theScene );
}

bool ObjFileLoader::Load( const char* filename, SceneDescription& theScene )
{
	Reset();
	ScenePtr = &theScene;

	FILE* infile = fopen( filename, "r" );
	FileLineNumber = 0;

	if ( !infile ) {
		fprintf(stderr, "LoadObjFile: Unable to open file: %s\n", filename);
		return false;
	}

	char inbuffer[1026];
	while ( true ) {
		if ( !fgets( inbuffer, 1026, infile ) ) {
			fclose( infile );
			PrintCmdNotSupportedErrors(stderr);
			return true;
		}
		FileLineNumber++;

		char *findStart = Preparse( inbuffer );
		if ( findStart==0 || (*findStart)=='#' ) {
			continue;				// Ignore if a comment or a blank line
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
		
		char* args = ScanForSecondField( findStart );
		bool ok = true;
		switch ( cmdNum ) {
		case 0:   // 'v' command
			{
				VectorR4* vertData = Vertices.Push();
				ok = ReadVectorR4Hg ( args, vertData );
			}
			break;
		case 1:   // "vt" command
			{
				VectorR2* texData = TextureCoords.Push();
				ok = ReadTexCoords( args, texData );
			}
			break;
		case 2:   // The 'f' command
			{
				ok = ProcessFace( args );
			}
			break;
		case 3:   // 'l' command
			UnsupportedLines();
			break;
		default:
			parseErrorOccurred  = true;
			break;
		}
		if ( !ok ) {
			parseErrorOccurred = true;
		}
	}
}

char* ObjFileLoader::Preparse( char* inbuf ) 
{
	// Change white space to real spaces
	char *s;
	for ( s=inbuf; *s!=0; s++ ) {
		if ( *s=='\t' || *s=='\n' ) {
			*s = ' ';
		}
	}
	return ScanForNonwhite( inbuf );
}

char* ObjFileLoader::ScanForSecondField( char* inbuf )
{
	char *s;
	for ( s=inbuf; (*s)!=' ' && (*s)!=0; s++ ) {}
	return ScanForNonwhite( s );
}

// Return zero (null pointer) if no non-white found
char* ObjFileLoader::ScanForNonwhite( char* inbuf ) 
{
	char* s;
	for ( s = inbuf; (*s)!=0; s++ ) {
		if ( (*s)!=' ' ) {
			return s;
		}
	}
	return 0;
}

// Scan for white space (or end of string)
char* ObjFileLoader::ScanForWhite( char* inbuf ) 
{
	char* s;
	for ( s = inbuf; (*s)!=0 && (*s)!=' '; s++ ) {
		continue;
	}
	return s;
}

// Return zero (null pointer) if no white space or slash found
char* ObjFileLoader::ScanForWhiteOrSlash( char* inbuf ) 
{
	char* s;
	for ( s = inbuf; (*s)!=' ' && (*s)!='/'; s++ ) {
		if ( (*s)==0 ) {
			return 0;
		}
	}
	return s;
}

int ObjFileLoader::GetCommandNumber( char *cmd ) {
	long i;
	for ( i=0; i<numCommands; i++ ) {
		if ( strcmp( cmd, commandList[i] ) == 0 ) {
			return i;
		}
	}
	return -1;		// Command not found
}

bool ObjFileLoader::ReadVectorR4Hg( char* inbuf, VectorR4 *theVec )
{
	int scanCode = sscanf ( inbuf, "%lf %lf %lf %lf", &(theVec->x), &(theVec->y), &(theVec->z), &(theVec->w) );
	bool retCode = (scanCode==3 || (scanCode==4 && theVec->w!=0.0) );
	if ( scanCode==3 || theVec->w == 0.0 ) {
		theVec->w = 1.0;
	}
	return retCode;
}

bool ObjFileLoader::ReadTexCoords( char* inbuf, VectorR2* theVec )
{
	double depth;
	int scanCode = sscanf( inbuf, "%lf %lf %lf", &(theVec->x), &(theVec->y), &depth );
	bool retCode = true;
	switch ( scanCode ) 
	{
	case 1:
		theVec->y = 0.0;
		break;
	case 2:
		break;
	case 3:
		if ( depth!=0.0 ) {
			UnsupportedTextureDepth();
		}
		break;
	default:
		retCode = false;
		break;
	}
	return retCode;
}

bool ObjFileLoader::ProcessFace( char* inbuf)
{

	const int maxNumVerts = 256;

	// Phase 1 : Load vertex data into arrays

	// This array holds the vertex numbers for
	//	(a) vertices,  (b) texture coordinates,  (c)  vertex normals.
	long vertNums[3*maxNumVerts];		// Use -1 for missing values

	bool missingNormals = false;
	bool missingTexCoords = false;

	int i;
	char* s = inbuf;
	for ( i=0; i<maxNumVerts+1; i++ ) {
		s = ScanForNonwhite( s );
		if ( s==0 ) {
			break;	
		}
		if ( i>=maxNumVerts ) {
			UnsupportedTooManyVerts(maxNumVerts);
			return false;
		}
		long scannedInt;
		int scanCode = sscanf( s, "%ld", &scannedInt );
		if ( scanCode==0 ) {
			return false;
		}
		// Negative indices refer to counting backwards
		vertNums[3*i] = (scannedInt>0) ? scannedInt : Vertices.SizeUsed()+scannedInt;
		if ( vertNums[3*i]<1 || vertNums[3*i]>Vertices.SizeUsed() ) {
			return false;
		}
		s = ScanForWhiteOrSlash( s );
		if ( (*s)!='/' ) {
			vertNums[3*i+1] = vertNums[3*i+2] = -1;	// No texture coords or normal
			missingTexCoords = true;
			missingNormals = true;
			continue;
		}
		if ( (*(s+1))=='/' || (*(s+1))==' ' || (*(s+1))==0 ) {
			vertNums[3*i+1] = -1;
			missingTexCoords = true;
		}
		else {
			scanCode = sscanf( s+1, "%ld", &scannedInt );
			if ( scanCode==0 ) {
				return false;
			}
			// Negative indices refer to counting backwards
			vertNums[3*i+1] = (scannedInt>0) ? scannedInt : TextureCoords.SizeUsed()+scannedInt;
			if ( vertNums[3*i+1]<1 || vertNums[3*i+1]>TextureCoords.SizeUsed() ) {
				return false;
			}
		}
		s = ScanForWhiteOrSlash( s+1 );
		if ( (*s)!='/' ) {
			vertNums[3*i+2] = -1;	// No normal
			missingNormals = true;
			continue;
		}
		if ( (*(s+1))==' ' || (*(s+1))==0 ) {
			vertNums[3*i+2] = -1;
			missingNormals = true;
		}
		else {
			scanCode = sscanf( s+1, "%ld", vertNums+3*i+2 );
			if ( scanCode!=1 ) {
				return false;
			}
			// Negative indices refer to counting backwards
			vertNums[3*i+2] = (scannedInt>0) ? scannedInt : VertexNormals.SizeUsed()+scannedInt;
			// XXX TO DO: PUT THIS BACK ONCE NORMALS ARE SCANNED IN!
			//if ( vertNums[3*i+2]<1 || vertNums[3*i+2]>VertexNormals.SizeUsed() ) {
			//	return false;
			//}
		}
		s = ScanForWhite( s+1 );
	}

	int numVertsInFace = i;
	if ( numVertsInFace<3 ) {
		return false;
	}

	// Create the ViewableTriangles

	// Textures: At the moment, we do not support materials, so it does not 
	//		make any sense to support textures and texture coordinates.

	VectorR3 vA, vB, vC;
#if 1
	// Check for perfect parallolgram first
	if ( numVertsInFace==4 ) {
		VectorR3 vD;
		vA.SetFromHg( Vertices[vertNums[0]-1] );
		vB.SetFromHg( Vertices[vertNums[3]-1] );
		vC.SetFromHg( Vertices[vertNums[6]-1] );
		vD.SetFromHg( Vertices[vertNums[9]-1] );
		if ( (vD-vA)==(vC-vB) && (vB-vA)==(vC-vD) ) {
			// Add parallelogram
			ViewableParallelogram* vp = new ViewableParallelogram();
            vp->Init( vA, vB, vC );
			ScenePtr->AddViewable( vp );
			return true;
		}
	}
	// Otherwise, add as (numVertsInFace-2) many triangles.
	int startIdx = 0;
	int stepIdx = 1;
	for ( i=0; i<numVertsInFace-2; i++ ) {
		// Add i-th face of (numVertsInFace-2) total triangles.
		int idx2 = NextTriVertIdx( startIdx, &stepIdx, numVertsInFace );
		int idx3 = NextTriVertIdx( idx2, &stepIdx, numVertsInFace );
		int i1 = vertNums[3*startIdx]-1;
		int i2 = vertNums[3*idx2]-1;
		int i3 = vertNums[3*idx3]-1;
		if ( i1==i2 || i1==i3 || i2==i3 ) {
			// Format error: duplicated vertex in planar, convex polygon!
			return false;
		}
		else {
			vA.SetFromHg( Vertices[i1] );
			vB.SetFromHg( Vertices[i2] );
			vC.SetFromHg( Vertices[i3] );
			startIdx = idx3;
			assert ( 0 <= idx2 && idx2 < numVertsInFace );
			assert ( 0 <= idx3 && idx3 < numVertsInFace );
			ViewableTriangle* vt = new ViewableTriangle();
			vt->Init( vA, vB, vC );
			if ( vt->IsWellFormed() ) {
				// If triangle has non-zero area, add it.
				ScenePtr->AddViewable( vt );
			}
		}
	}
#else
	// OLD CODE
	vA.SetFromHg( Vertices[vertNums[0]-1] );
	vB.SetFromHg( Vertices[vertNums[3]-1] );
	vC.SetFromHg( Vertices[vertNums[6]-1] );
	if ( numVertsInFace == 3 ) {
		ViewableTriangle* vt = new ViewableTriangle();
		vt->Init( vA, vB, vC );
		ScenePtr->AddViewable( vt );

	}
	else {
		// Four sided is the only other option for now
		VectorR3 vD;
		vD.SetFromHg( Vertices[vertNums[9]-1] );
		if ( (vD-vA)==(vC-vB) && (vB-vA)==(vC-vD) ) {
			// Add parallelogram
			ViewableParallelogram* vp = new ViewableParallelogram();
            vp->Init( vA, vB, vC );
			ScenePtr->AddViewable( vp );
		}
		else {
			// Add as two triangles
			ViewableTriangle* vt1 = new ViewableTriangle();
			ViewableTriangle* vt2 = new ViewableTriangle();
			if ( vA.DistSq(vC) < vB.DistSq(vD) ) {
				vt1->Init( vA, vB, vC );
				vt2->Init( vC, vD, vA );
			}
			else {
				vt1->Init( vA, vB, vD );
				vt2->Init( vB, vC, vD );
			}
			ScenePtr->AddViewable( vt1 );
			ScenePtr->AddViewable( vt2 );
		}
	}
#endif

	return true;
}

int ObjFileLoader::NextTriVertIdx( int start, int* step, int totalNum )
{
	int retIdx = start + (*step);
	if ( retIdx >= totalNum ) {
		retIdx = (retIdx-totalNum)<<1;
		(*step) <<= 1;
	}
	return retIdx;
}

void ObjFileLoader::Reset()
{
	Vertices.Reset();
	TextureCoords.Reset();
	// VertexNormals.Reset();
	for ( long i=0; i<UnsupportedCmds.SizeUsed(); i++ ) {
		delete UnsupportedCmds[i];
	}
	UnsupportedCmds.Reset();
}

void ObjFileLoader::UnsupportedTextureDepth()
{ 
	if ( (!UnsupFlagTextureDepth) && ReportUnsupportedFeatures ) {
		fprintf(stderr, "ObjFileLoader: Texture Depth - nonzero values not supported. (Line %ld.)\n", FileLineNumber );
		UnsupFlagTextureDepth = true;
	}
}

void ObjFileLoader::UnsupportedLines()
{ 
	if ( (!UnsupFlagLines) && ReportUnsupportedFeatures ) {
		fprintf(stderr, "ObjFileLoader: 'l' commands ignored: lines cannot be rendered.\n" );
		UnsupFlagLines = true;
	}
}

void ObjFileLoader::UnsupportedTooManyVerts( int maxVerts)
{ 
	if ( (!UnsupFlagTooManyVerts) && ReportUnsupportedFeatures ) {
		fprintf(stderr, "ObjFileLoader: Faces with more than %d verts not supported. (Line %ld.)\n", maxVerts, FileLineNumber );
		UnsupFlagTooManyVerts = true;
	}
}

void ObjFileLoader::AddUnsupportedCmd( char *cmd )
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

void ObjFileLoader::PrintCmdNotSupportedErrors( FILE* outstream ) {
	if ( !ReportUnsupportedFeatures ) {
		return;
	}
	int numUnsupCmds = UnsupportedCmds.SizeUsed();
	if ( numUnsupCmds > 0 ) {
		fprintf( outstream, "ObjFileLoader: Unsupported commands in .nff file:  " );
		for ( long i=0; i<numUnsupCmds; i++ ) {
			if ( i!=0 ) {
				fprintf( outstream, ", " );
			}
			fprintf( outstream, "%s", UnsupportedCmds[i] );
		}
		fprintf( outstream, ".\n" );
	}
}

