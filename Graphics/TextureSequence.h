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

#ifndef TEXTURESEQUENCE_H
#define TEXTURESEQUENCE_H

#include "TextureMapBase.h"

//  A TextureSequence object applies a series of texture maps
//	   sequentially.  You cannot change the number of texture maps
//		after the TextureSequence is created; however, you can use a 
//		null pointer for the identity texture map.

class TextureSequence : public TextureMapBase {

public:
	TextureSequence( int numTexturesMaps );		// Number of texture (cannot be changed)
	TextureSequence( int numTexturesMaps, TextureMapBase* textureMaps[] );		
	TextureSequence( TextureMapBase* textureMap0, TextureMapBase* textureMap1 );		
	TextureSequence( TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
					 TextureMapBase* textureMap2 );		
	TextureSequence( TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
					 TextureMapBase* textureMap2, TextureMapBase* textureMap3 );	
	
	virtual ~TextureSequence();			// Destructor does not free the texture maps
	void Init( int numTexturesMaps );		// Number of texture (cannot be changed)


	void ApplyTexture( VisiblePoint& visPoint ) const;

	void SetTexture( const TextureMapBase* textureMap, int textureIndex );
	const TextureMapBase* GetTexture( int textureIndex ) const { return TexMapPtrs[textureIndex]; }

	void DeleteAll();			// Frees (deletes) all the texture maps

private:
	typedef const TextureMapBase* TextureMapPointer;
	
	int NumTextureMaps;						// Number of texture maps
	TextureMapPointer* TexMapPtrs;	// Array of pointers to texture maps			

};

typedef const TextureMapBase* TextureMapPointer;

inline TextureSequence::TextureSequence( int numTextureMaps )
{
	Init(numTextureMaps); 
	for ( int i=0; i<numTextureMaps; i++ ) {
		TexMapPtrs[i] = 0;
	}
	return;
}

inline TextureSequence::TextureSequence( TextureMapBase* textureMap0, TextureMapBase* textureMap1 )
{
	Init(2);
	TexMapPtrs[0] = textureMap0;
	TexMapPtrs[1] = textureMap1;
}

inline TextureSequence::TextureSequence( TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
					 TextureMapBase* textureMap2 )	
{
	Init(3);
	TexMapPtrs[0] = textureMap0;
	TexMapPtrs[1] = textureMap1;
	TexMapPtrs[2] = textureMap2;
}

inline TextureSequence::TextureSequence( TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
					 TextureMapBase* textureMap2, TextureMapBase* textureMap3 )	
{
	Init(4);
	TexMapPtrs[0] = textureMap0;
	TexMapPtrs[1] = textureMap1;
	TexMapPtrs[3] = textureMap2;
	TexMapPtrs[3] = textureMap3;
}

inline void TextureSequence::Init( int numTextureMaps ) {
	NumTextureMaps = numTextureMaps; 
	TexMapPtrs = new TextureMapPointer[NumTextureMaps];
	return;
}

inline TextureSequence::~TextureSequence() {
	delete[] TexMapPtrs;
}

inline void TextureSequence::SetTexture(const TextureMapBase* textureMap, int textureIndex) {
	TexMapPtrs[textureIndex] = textureMap;
}

#endif // TEXTURESEQUENCE_H

