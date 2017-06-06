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

#ifndef MATERIAL_H
#define MATERIAL_H

#include "assert.h"
#include "../VrMath/LinearR4.h"
#include "../VrMath/MathMisc.h"
#include "MaterialBase.h"

// the Material class inplements Phong lighting.  The base class MaterialBase
//		has the basic date elements that are common to Phong lighting and
//		Cook-Torrance lighting

class Material : public MaterialBase {

public:

	static const Material Default;

	Material() { Reset(); }

	void Reset();

	void SetShininess( double exponent );
	double GetPhongShininess() const { return Shininess; }

	void SetIndexOfRefraction( double x );
	double GetIndexOfRefraction () const { return IndexOfRefraction; }

	bool CalcRefractDir( const VectorR3& normal, const VectorR3& indir, VectorR3& outdir ) const;

	bool IsTransmissive() const { return TransmissiveFlag; }
	bool IsReflective() const { return ReflectiveFlag; }

	void SetColorTransmissive( double r, double g, double b);
	void SetColorReflective( double r, double g, double b);

	void SetColorTransmissive( double* color );
	void SetColorReflective( double* color );

	void SetColorTransmissive( float* color );
	void SetColorReflective( float* color );

	void SetColorTransmissive( const VectorR3& color );
	void SetColorReflective( const VectorR3& color );

	double GetShininess()  const { return Shininess; }

	void GetColorTransmissive( double* ) const;
	void GetColorReflective( double* ) const;

	const VectorR3& GetColorTransmissive() const;
	const VectorR3& GetColorReflective() const;

	virtual VectorR3 GetReflectionColor( const VisiblePoint& visPoint, 
												const VectorR3& outDir, 
												const VectorR3& fromDir) const
							{ return GetColorReflective(); }
	virtual VectorR3 GetTransmissionColor( const VisiblePoint& visPoint, 
												const VectorR3& outDir, 
												const VectorR3& fromDir) const
							{ return GetColorTransmissive(); }

	virtual void CalcLocalLighting( 
							VectorR3& colorReturned, const Light& light, 
							const VectorR3& percentLit, double lightAttenuation,
							const VectorR3& N, const VectorR3& V, 
							const VectorR3& L, const VectorR3* H ) const;

	MaterialBase* Clone() const;


protected:
	VectorR3 ColorTransmissive;  
	VectorR3 ColorReflective;		// Global reflection color coefficients

	double IndexOfRefraction;
	double IndexOfRefractionInv;	// 1/(index of refraction)

	bool TransmissiveFlag;		// Is transmissive color non-black?
	bool ReflectiveFlag;		// Is reflective color non-black?

	double Shininess;			// Shininess exponent

};

inline void Material::Reset() 
{	
	Shininess = 0.0f;
	SetColorAmbient( 0.2, 0.2, 0.2 );
	SetColorDiffuse( 0.8, 0.8, 0.8 );
	SetColorSpecular( 0.0, 0.0, 0.0 );
	SetColorEmissive( 0.0, 0.0, 0.0 );
	SetColorTransmissive( 0.0, 0.0, 0.0 );
	SetColorReflective( 0.2, 0.2, 0.2 );
	SetIndexOfRefraction( 1.0 );
}

inline void Material::SetShininess( double exponent ) {
	Shininess = exponent;
}

inline void Material::SetIndexOfRefraction( double x )
{
	IndexOfRefraction = x;
	IndexOfRefractionInv = 1.0/x;
}

inline void Material::SetColorTransmissive(double r, double g, double b) {
	ColorTransmissive.Set( r, g, b );
	TransmissiveFlag = !(r==0.0 && g==0.0 && b==0.0);
}

inline void Material::SetColorReflective(double r, double g, double b) {
	ColorReflective.Set( r, g, b );
	ReflectiveFlag = !(r==0.0 && g==0.0 && b==0.0);
}

inline void Material::SetColorTransmissive( const VectorR3& color ) {
	SetColorTransmissive(color.x,color.y,color.z);
}

inline void Material::SetColorReflective( const VectorR3& color ) {
	SetColorReflective(color.x,color.y,color.z);
}

inline void Material::SetColorTransmissive( double* color ) {
	SetColorTransmissive( *color, *(color+1), *(color+2) );
}

inline void Material::SetColorTransmissive( float* color ) {
	SetColorTransmissive( *color, *(color+1), *(color+2) );
}

inline void Material::SetColorReflective( double* color ) {
	SetColorReflective( *color, *(color+1), *(color+2) );
}

inline void Material::SetColorReflective( float* color ) {
	SetColorReflective( *color, *(color+1), *(color+2) );
}

inline void Material::GetColorTransmissive( double* c) const {
	ColorReflective.Dump(c);
}

inline void Material::GetColorReflective( double* c) const {
	ColorReflective.Dump(c);
}

inline const VectorR3& Material::GetColorTransmissive() const {
	return ColorTransmissive;
}

inline const VectorR3& Material::GetColorReflective() const {
	return ColorReflective;
}

inline bool Material::CalcRefractDir( const VectorR3& normal, const VectorR3& indir, VectorR3& outdir ) const
{
	return MaterialBase::CalcRefractDir(IndexOfRefraction, IndexOfRefractionInv, 
										normal, indir, outdir);
}

#endif // MATERIAL_H
