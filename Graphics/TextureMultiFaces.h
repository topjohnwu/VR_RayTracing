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

#ifndef TEXTUREMULTIFACES_H
#define TEXTUREMULTIFACES_H

#include "TextureMapBase.h"

//  A TextureSequence object applies different texture maps to different faces
//	   You cannot change the number of texture maps
//		after the TextureSequence is created; however, you can use a 
//		null pointer for the identity texture map.

class TextureMultiFaces : public TextureMapBase {

public:
	TextureMultiFaces( int numTexturesMaps );		// Number of texture (cannot be changed)
	TextureMultiFaces( int numTexturesMaps, TextureMapBase* textureMaps[] );		
	TextureMultiFaces( TextureMapBase* textureMap0, TextureMapBase* textureMap1 );		
	TextureMultiFaces( TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
					   TextureMapBase* textureMap2 );		
	TextureMultiFaces( TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
					   TextureMapBase* textureMap2, TextureMapBase* textureMap3 );	
	TextureMultiFaces( TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
					   TextureMapBase* textureMap2, TextureMapBase* textureMap3,	
					   TextureMapBase* textureMap4, TextureMapBase* textureMap5 );	
	
	virtual ~TextureMultiFaces();			// Destructor does not free the texture maps

	void ApplyTexture( VisiblePoint& visPoint ) const;

	// Set just one texture map.
	void SetTexture( const TextureMapBase* textureMap, int textureIndex );

	const TextureMapBase* GetTexture( int textureIndex ) const { return TexMapPtrs[textureIndex]; }

	void DeleteAll();			// Frees (deletes) all the texture maps

private:
	typedef const TextureMapBase* TextureMapPointer;
	
	int NumTextureMaps;						// Number of texture maps
	TextureMapPointer* TexMapPtrs;	// Array of pointers to texture maps			

};

typedef const TextureMapBase* TextureMapPointer;

inline TextureMultiFaces::TextureMultiFaces( int numTextureMaps )
{
	NumTextureMaps = numTextureMaps; 
	TexMapPtrs = new TextureMapPointer[NumTextureMaps];
	return;
}

inline TextureMultiFaces::TextureMultiFaces( TextureMapBase* textureMap0, TextureMapBase* textureMap1 )
{
	TextureMultiFaces(2);
	TexMapPtrs[0] = textureMap0;
	TexMapPtrs[1] = textureMap1;
}

inline TextureMultiFaces::TextureMultiFaces( TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
					 TextureMapBase* textureMap2 )	
{
	TextureMultiFaces(3);
	TexMapPtrs[0] = textureMap0;
	TexMapPtrs[1] = textureMap1;
	TexMapPtrs[2] = textureMap2;
}

inline TextureMultiFaces::TextureMultiFaces( TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
					 TextureMapBase* textureMap2, TextureMapBase* textureMap3 )	
{
	TextureMultiFaces(4);
	TexMapPtrs[0] = textureMap0;
	TexMapPtrs[1] = textureMap1;
	TexMapPtrs[2] = textureMap2;
	TexMapPtrs[3] = textureMap3;
}

inline TextureMultiFaces::TextureMultiFaces( TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
					 TextureMapBase* textureMap2, TextureMapBase* textureMap3,	
					 TextureMapBase* textureMap4, TextureMapBase* textureMap5 )	
{
	TextureMultiFaces(6);
	TexMapPtrs[0] = textureMap0;
	TexMapPtrs[1] = textureMap1;
	TexMapPtrs[2] = textureMap2;
	TexMapPtrs[3] = textureMap3;
	TexMapPtrs[4] = textureMap4;
	TexMapPtrs[5] = textureMap5;
}

inline TextureMultiFaces::~TextureMultiFaces() {
	delete[] TexMapPtrs;
}

inline void TextureMultiFaces::SetTexture(const TextureMapBase* textureMap, int textureIndex) {
	TexMapPtrs[textureIndex] = textureMap;
}

#endif // TEXTUREMULTIFACES_H

