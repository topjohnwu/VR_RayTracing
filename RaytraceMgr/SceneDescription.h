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

#ifndef SCENE_DESCRIPTION_H
#define SCENE_DESCRIPTION_H

#include "../DataStructs/Array.h"
#include "../VrMath/LinearR3.h"
#include "../Graphics/CameraView.h"
#include "../Graphics/Light.h"
#include "../Graphics/MaterialBase.h"
#include "../Graphics/Material.h"
#include "../Graphics/MaterialCookTorrance.h"
#include "../Graphics/TextureMapBase.h"
#include "../Graphics/TextureAffineXform.h"
#include "../Graphics/TextureBilinearXform.h"
#include "../Graphics/TextureCheckered.h"
#include "../Graphics/TextureMultiFaces.h"
#include "../Graphics/TextureRgbImage.h"
#include "../Graphics/TextureSequence.h"
#include "../Graphics/BumpMapFunction.h"
#include "../Graphics/ViewableBase.h"

class SceneDescription
{

public:

	SceneDescription();

	void SetBackGroundColor( float* color ) { TheBackgroundColor.Load( color ); }
	void SetBackGroundColor( double* color ) { TheBackgroundColor.Load( color ); }
	void SetBackGroundColor( const VectorR3& color ) { TheBackgroundColor = color; }
	void SetBackGroundColor( double r, double g, double b ) { TheBackgroundColor.Set(r, g, b); }
	VectorR3& BackgroundColor() { return TheBackgroundColor; }
	const VectorR3& BackgroundColor() const { return TheBackgroundColor; }

	void SetGlobalAmbientLight( float* color ) { TheGlobalAmbientLight.Load( color ); }
	void SetGlobalAmbientLight( double* color ) { TheGlobalAmbientLight.Load( color ); }
	void SetGlobalAmbientLight( const VectorR3& color ) { TheGlobalAmbientLight = color; }
	void SetGlobalAmbientLight( double r, double g, double b ) { TheGlobalAmbientLight.Set(r, g, b); }
	VectorR3& GlobalAmbientLight() { return TheGlobalAmbientLight; }
	const VectorR3& GlobalAmbientLight() const { return TheGlobalAmbientLight; }

	CameraView& GetCameraView() { return CameraAndViewer; }
	const CameraView& GetCameraView() const { return CameraAndViewer; }

	// Once you have set up an initial CameraView, you can call RegisterCameraView.
	//  After that, you may call CalcNewSceenDims( aspectRatio ) to get
	//	a suggested width and height for the camera screen.
	void RegisterCameraView();
	void CalcNewScreenDims( float aspectRatio );

	int NumLights() const { return LightArray.SizeUsed(); }
	Light* NewLight() { return *(LightArray.Push()) ; }
	int AddLight( Light* newLight );
	Light& GetLight( int i ) { return *LightArray[i]; }
	const Light& GetLight( int i ) const { return *LightArray[i]; }
	Array<Light*>& GetLightArray() { return LightArray; }
	const Array<Light*>& GetLightArray() const { return LightArray; }

	int NumMaterials() const { return MaterialArray.SizeUsed(); }
	Material* NewMaterial();
	MaterialCookTorrance* NewMaterialCookTorrance();
	int AddMaterial( MaterialBase* newMaterial );
	MaterialBase& GetMaterial( int i ) { return *MaterialArray[i]; }
	const MaterialBase& GetMaterial( int i ) const { return *MaterialArray[i]; }
	Array<MaterialBase*>& GetMaterialArray() { return MaterialArray; }
	const Array<MaterialBase*>& GetMaterialArray() const { return MaterialArray; }

	int NumTextures() const { return TextureArray.SizeUsed(); }

	int AddTexture( TextureMapBase* newTexture );
	TextureAffineXform* NewTextureAffineXform();
	TextureBilinearXform* NewTextureBilinearXform();
	TextureCheckered* NewTextureCheckered();

	TextureMultiFaces* NewTextureMultiFaces( int numTexMaps );
	TextureMultiFaces* NewTextureMultiFaces( int numTexturesMaps, TextureMapBase* textureMaps[] );
	TextureMultiFaces* NewTextureMultiFaces( TextureMapBase* textureMap0, TextureMapBase* textureMap1 );
	TextureMultiFaces* NewTextureMultiFaces( TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
										 TextureMapBase* textureMap2 );
	TextureMultiFaces* NewTextureMultiFaces( TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
										 TextureMapBase* textureMap2, TextureMapBase* textureMap3 );
	TextureMultiFaces* NewTextureMultiFaces( TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
										 TextureMapBase* textureMap2, TextureMapBase* textureMap3,
										 TextureMapBase* textureMap4, TextureMapBase* textureMap5  );

	TextureRgbImage* NewTextureRgbImage();
	TextureRgbImage* NewTextureRgbImage( const RgbImage& rgbImage );
	TextureRgbImage* NewTextureRgbImage( const char* filename );

	TextureSequence* NewTextureSequence( int numTexMaps );
	TextureSequence* NewTextureSequence( int numTexturesMaps, TextureMapBase* textureMaps[] );
	TextureSequence* NewTextureSequence( TextureMapBase* textureMap0, TextureMapBase* textureMap1 );
	TextureSequence* NewTextureSequence( TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
										 TextureMapBase* textureMap2 );
	TextureSequence* NewTextureSequence( TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
										 TextureMapBase* textureMap2, TextureMapBase* textureMap3 );

	BumpMapFunction* NewBumpMapFunction();
	TextureMapBase& GetTexture( int i ) { return *TextureArray[i]; }
	const TextureMapBase& GetTexture( int i ) const { return *TextureArray[i]; }
	Array<TextureMapBase*>& GetTextureArray() { return TextureArray; }
	const Array<TextureMapBase*>& GetTextureArray() const { return TextureArray; }

	int NumViewables() const { return ViewableArray.SizeUsed(); }
	int AddViewable( ViewableBase* newViewable );
	ViewableBase& GetViewable( int i ) { return *ViewableArray[i]; }
	const ViewableBase& GetViewable( int i ) const { return *ViewableArray[i]; }
	Array<ViewableBase*>& GetViewableArray() { return ViewableArray; }
	const Array<ViewableBase*>& GetViewableArray() const { return ViewableArray; }

	void DeleteAllLights();
	void DeleteAllTextures();
	void DeleteAllMaterials();
	void DeleteAllViewables();
	void DeleteAll();

private:

	VectorR3 TheGlobalAmbientLight;
	VectorR3 TheBackgroundColor;

	CameraView CameraAndViewer;
	double RegisteredScreenWidth;
	double RegisteredScreenHeight;
	bool ScreenRegistered;

	Array<Light*> LightArray;

	Array<MaterialBase*> MaterialArray;

	Array<TextureMapBase*> TextureArray;

	Array<ViewableBase*> ViewableArray;

};

inline SceneDescription::SceneDescription()
{
	TheBackgroundColor.Set( 0.0, 0.0, 0.0 );
	TheGlobalAmbientLight.SetZero();
	ScreenRegistered = false;
}

inline int SceneDescription::AddLight( Light* newLight ) 
{ 
	int index = (int)LightArray.SizeUsed();
	LightArray.Push( newLight );
	return index;
}

inline Material* SceneDescription::NewMaterial() 
{ 
	Material* newMat = new Material();
	MaterialBase* newMatBase = (MaterialBase*)newMat;
	MaterialArray.Push( newMatBase );
	return newMat;
}

inline MaterialCookTorrance* SceneDescription::NewMaterialCookTorrance() 
{ 
	MaterialCookTorrance* newMatCT = new MaterialCookTorrance();
	MaterialBase* newMatBase = (MaterialBase*)newMatCT;
	MaterialArray.Push( newMatBase );
	return newMatCT;
}

inline int SceneDescription::AddMaterial( MaterialBase* newMaterial ) 
{ 
	int index = (int)MaterialArray.SizeUsed();
	MaterialArray.Push( newMaterial ); 
	return index;
}

inline int SceneDescription::AddTexture( TextureMapBase* newTexture ) 
{ 
	int index = (int)TextureArray.SizeUsed();
	TextureArray.Push( newTexture );
	return index;
}

inline int SceneDescription::AddViewable( ViewableBase* newViewable ) 
{ 
	int index = (int)ViewableArray.SizeUsed();
	ViewableArray.Push( newViewable );
	return index;
}

inline TextureAffineXform* SceneDescription::NewTextureAffineXform() 
{ 
	TextureAffineXform* newTex = new TextureAffineXform();
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}

inline TextureBilinearXform* SceneDescription::NewTextureBilinearXform() 
{ 
	TextureBilinearXform* newTex = new TextureBilinearXform();
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}

inline TextureCheckered* SceneDescription::NewTextureCheckered() 
{ 
	TextureCheckered* newTex = new TextureCheckered();
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}

inline TextureRgbImage* SceneDescription::NewTextureRgbImage() 
{ 
	TextureRgbImage* newTex = new TextureRgbImage();
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}

inline TextureRgbImage* SceneDescription::NewTextureRgbImage( const RgbImage& rgbImage) 
{ 
	TextureRgbImage* newTex = new TextureRgbImage( rgbImage );
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}

inline TextureRgbImage* SceneDescription::NewTextureRgbImage( const char* filename ) 
{ 
	TextureRgbImage* newTex = new TextureRgbImage();
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}

inline TextureMultiFaces* SceneDescription::NewTextureMultiFaces( int numTexMaps ) 
{ 
	TextureMultiFaces* newTex = new TextureMultiFaces( numTexMaps );
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}

inline TextureMultiFaces* SceneDescription::NewTextureMultiFaces( int numTexturesMaps, TextureMapBase* textureMaps[] ) 
{ 
	TextureMultiFaces* newTex = new TextureMultiFaces( numTexturesMaps, textureMaps );
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}

inline TextureMultiFaces* SceneDescription::NewTextureMultiFaces( TextureMapBase* textureMap0, TextureMapBase* textureMap1 ) 
{ 
	TextureMultiFaces* newTex = new TextureMultiFaces( textureMap0, textureMap1 );
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}

inline TextureMultiFaces* SceneDescription::NewTextureMultiFaces( TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
																  TextureMapBase* textureMap2 ) 
{ 
	TextureMultiFaces* newTex = new TextureMultiFaces( textureMap0, textureMap1, textureMap2 );
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}

inline TextureMultiFaces* SceneDescription::NewTextureMultiFaces(TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
																 TextureMapBase* textureMap2, TextureMapBase* textureMap3 ) 
{ 
	TextureMultiFaces* newTex = new TextureMultiFaces( textureMap0, textureMap1, textureMap2, textureMap3 );
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}

inline TextureMultiFaces* SceneDescription::NewTextureMultiFaces( TextureMapBase* textureMap0, TextureMapBase* textureMap1, 
										 TextureMapBase* textureMap2, TextureMapBase* textureMap3,
										 TextureMapBase* textureMap4, TextureMapBase* textureMap5 ) 
{ 
	TextureMultiFaces* newTex = new TextureMultiFaces( textureMap0, textureMap1, textureMap2, textureMap3,
													   textureMap4, textureMap5 );
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}

inline TextureSequence* SceneDescription::NewTextureSequence( int numTexMaps ) 
{ 
	TextureSequence* newTex = new TextureSequence( numTexMaps );
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}

inline TextureSequence* SceneDescription::NewTextureSequence( int numTexturesMaps, TextureMapBase* textureMaps[] ) 
{ 
	TextureSequence* newTex = new TextureSequence( numTexturesMaps, textureMaps );
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}
inline TextureSequence* SceneDescription::NewTextureSequence( TextureMapBase* textureMap0, TextureMapBase* textureMap1 ) 
{ 
	TextureSequence* newTex = new TextureSequence( textureMap0, textureMap1 );
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}

inline TextureSequence* SceneDescription::NewTextureSequence( TextureMapBase* textureMap0, TextureMapBase* textureMap1,
															  TextureMapBase* textureMap2 ) 
{ 
	TextureSequence* newTex = new TextureSequence( textureMap0, textureMap1, textureMap2 );
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}

inline TextureSequence* SceneDescription::NewTextureSequence( TextureMapBase* textureMap0, TextureMapBase* textureMap1,
															  TextureMapBase* textureMap2, TextureMapBase* textureMap3 ) 
{ 
	TextureSequence* newTex = new TextureSequence( textureMap0, textureMap1, textureMap2, textureMap3 );
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}

inline BumpMapFunction* SceneDescription::NewBumpMapFunction() 
{ 
	BumpMapFunction* newTex = new BumpMapFunction();
	TextureMapBase* newTexBase = (TextureMapBase*)newTex;
	TextureArray.Push( newTexBase );
	return newTex;
}

inline void SceneDescription::DeleteAll()
{
	DeleteAllLights();
	DeleteAllTextures();
	DeleteAllMaterials();
	DeleteAllViewables();
}

#endif // SCENE_DESCRIPTION_H