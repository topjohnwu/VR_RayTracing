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

#ifndef MATERIAL_COOKTORRANCE_H
#define MATERIAL_COOKTORRANCE_H

#include "assert.h"
#include "../VrMath/LinearR4.h"
#include "../VrMath/MathMisc.h"
#include "MaterialBase.h"

// the Material class inplements Cook-Torrance lighting.  The base class MaterialBase
//		has the basic date elements that are common to Phong lighting and
//		Cook-Torrance lighting

class MaterialCookTorrance : public MaterialBase {

public:
	MaterialCookTorrance() { Reset(); }

	void Reset();

	void SetRoughness( double slope );		// The "m" value.
	double GetRoughness() const { return MeanSlope; }
	double GetPhongShininess() const;


	// The ReflectionFactor is an extra multiplicative factor applied
	//		to reflected light.   (The ReflectionFactor is also applied
	//		to transmitted light.)
	// Default value = 1.0.		I.e.  1.0.
	void SetReflectionFactor( double x );
	void SetReflectionFactor( double red, double green, double blue );
	void SetReflectionFactor( double* rgb );
	void SetReflectionFactor( float* rgb );
	void SetReflectionFactor( const VectorR3& reflectionfactors );
	const VectorR3& GetReflectionFactor () const { return ReflectionFactor; }

	// Transmission works much like the Cook-Torrance reflection formulas.
	//		The TransmissionFactor is an extra multiplicative factor applied
	//		to transmitted light.   (The ReflectionFactor is also applied
	//		to transmitted light.)
	// Suggested value = 1.0.
	void SetTransmissionFactor( double indexrefraction );
	void SetTransmissionFactor( double red, double green, double blue );
	void SetTransmissionFactor( double* rgb );
	void SetTransmissionFactor( float* rgb );
	void SetTransmissionFactor( const VectorR3& indicesrefraction );
	const VectorR3& GetTransmissionFactor () const { return TransmissionFactor; }

	// There are separate indices of refraction for red, green and blue.
	void SetIndexOfRefraction( double x );
	void SetIndexOfRefraction( double red, double green, double blue );
	void SetIndexOfRefraction( double* rgb );
	void SetIndexOfRefraction( float* rgb );
	void SetIndexOfRefraction( const VectorR3& indicesrefraction );
	const VectorR3& GetIndexOfRefraction () const { return IndexOfRefraction; }

	// Use Cook & Torrance's method to set index of refraction from normal reflectance
	void SetRefractionFromReflectance( double reflectance );
	void SetRefractionFromReflectance( double red, double green, double blue );
	void SetRefractionFromReflectance( double* rgb );
	void SetRefractionFromReflectance( float* rgb );
	void SetRefractionFromReflectance( const VectorR3& reflectance );

	bool CalcRefractDir( const VectorR3& normal, const VectorR3& indir, VectorR3& outdir ) const;

	bool IsTransmissive() const { return TransmissiveFlag; }
	bool IsReflective() const { return ReflectiveFlag; }

	// Here is the main local lighting routine for the Cook-Torrance lighting
	//		model.
	virtual void CalcLocalLighting( 
							VectorR3& colorReturned, const Light& light, 
							const VectorR3& percentLit, double lightAttenuation,
							const VectorR3& N, const VectorR3& V, 
							const VectorR3& L, const VectorR3* H ) const;

	// The next two routines are used for ray tracing.
	// GetReflectionColor is the main Cook-Torrance computation.  
	virtual VectorR3 GetReflectionColor( const VisiblePoint& visPoint, 
												const VectorR3& outDir, 
												const VectorR3& fromDir) const;

	// GetTransmissionColor is an extension of the Cook-Torrance model to
	//		transmission.
	virtual VectorR3 GetTransmissionColor( const VisiblePoint& visPoint, 
												const VectorR3& outDir, 
												const VectorR3& fromDir) const;

	MaterialBase* Clone() const;

							
private:

	VectorR3 ReflectionFactor;		// Multiplicative factor for reflection and transmission
	VectorR3 TransmissionFactor;	// An extra multiplicative factor for transmission.
	VectorR3 IndexOfRefraction;		// Indices of refraction for red, green and blue.
	double MeanSlope;				// Root Mean Slope
	bool ReflectiveFlag;			// Is the surface reflective (non-black)?
	bool TransmissiveFlag;			// Is the surface transmissive?

private:

	// The next three routines manage the "guts" of the Cook-Torrance lighting calculation
	void CalcReflectionColorAbove( const VectorR3& L, const VectorR3& N, const VectorR3 V,
								   VectorR3* returnedColor ) const;
	void CalcReflectionColorBelow( const VectorR3& L, const VectorR3& N, const VectorR3 V,
								   VectorR3* returnedColor ) const;
	void CalcTransmissionColor( const VectorR3& L, const VectorR3& N, const VectorR3 V,
								VectorR3* returnedColor ) const;

	double CalcTransmissionFactor( const VectorR3& L, const VectorR3& N,
											const VectorR3& V, double eta ) const;
	double CalcRefraction( double reflectance ) const;
	double CalcGeometricFactor( const VectorR3& L, const VectorR3& N, 
								const VectorR3& V, const VectorR3& H ) const;
	double CalcGeometricXmitFactor( const VectorR3& L, const VectorR3& N, 
									const VectorR3& V, const VectorR3& H ) const;
	double CalcSlopeDistribution( double cospsi ) const;
	static double CalcFresnelTerm( double costheta, double indexOfRefraction);
};

// Set default values
inline void MaterialCookTorrance::Reset()
{
	SetRoughness( 0.2 );			// Root mean slope
	SetReflectionFactor( 1.0 );		// Fully specularly reflective (value may be too high)
	SetTransmissionFactor( 0.0 );	// Not transmissive
	SetIndexOfRefraction( 1.3 );	// Index of refraction
									//  - Defaults to 1.3.  Should not be one except for
									//      surfaces with no specular reflection.
}

inline void MaterialCookTorrance::SetRoughness( double slope )		// The "m" value.
{
	MeanSlope = slope;
}

inline void MaterialCookTorrance::SetReflectionFactor( double x )
{
	SetReflectionFactor( x, x, x );
}

inline void MaterialCookTorrance::SetReflectionFactor( double red, double green, double blue )
{
	ReflectionFactor.Set( red, green, blue );
	ReflectiveFlag = (red!=0.0 || green!=0.0 || blue!=0.0);
}

inline void MaterialCookTorrance::SetReflectionFactor( double* rgb )
{
	SetReflectionFactor( *rgb, *(rgb+1), *(rgb+2) );
}

inline void MaterialCookTorrance::SetReflectionFactor( float* rgb )
{
	SetReflectionFactor( *rgb, *(rgb+1), *(rgb+2) );
}

inline void MaterialCookTorrance::SetReflectionFactor( const VectorR3& r ) 
{
	SetReflectionFactor( r.x, r.y, r.z );
}

inline void MaterialCookTorrance::SetTransmissionFactor( double x )
{
	SetTransmissionFactor( x, x, x );
}

inline void MaterialCookTorrance::SetTransmissionFactor( double red, double green, double blue )
{
	TransmissionFactor.Set( red, green, blue );
	TransmissiveFlag = (red!=0.0 || green!=0.0 || blue!=0.0);
}

inline void MaterialCookTorrance::SetTransmissionFactor( double* rgb )
{
	SetTransmissionFactor( *rgb, *(rgb+1), *(rgb+2) );
}

inline void MaterialCookTorrance::SetTransmissionFactor( float* rgb )
{
	SetTransmissionFactor( *rgb, *(rgb+1), *(rgb+2) );
}

inline void MaterialCookTorrance::SetTransmissionFactor( const VectorR3& t ) 
{
	SetTransmissionFactor( t.x, t.y, t.z );
}

// There are separate indices of refraction for red, green and blue.
inline void MaterialCookTorrance::SetIndexOfRefraction( double x )
{
	SetIndexOfRefraction( x, x, x );
}

inline void MaterialCookTorrance::SetIndexOfRefraction( double red, double green, double blue )
{
	assert ( red>0.0 && blue>0.0 && green>0.0 );
	IndexOfRefraction.Set( red, green, blue );
}

inline void MaterialCookTorrance::SetIndexOfRefraction( double* rgb )
{
	SetIndexOfRefraction( *rgb, *(rgb+1), *(rgb+2) );
}

inline void MaterialCookTorrance::SetIndexOfRefraction( float* rgb )
{
	SetIndexOfRefraction( *rgb, *(rgb+1), *(rgb+2) );
}

inline void MaterialCookTorrance::SetIndexOfRefraction( const VectorR3& t ) 
{
	SetIndexOfRefraction( t.x, t.y, t.z );
}

inline void MaterialCookTorrance::SetRefractionFromReflectance( double x )
{
	double eta = CalcRefraction(x);
	SetIndexOfRefraction( eta );
}

inline void MaterialCookTorrance::SetRefractionFromReflectance( double red, double green, double blue )
{
	double redEta = CalcRefraction(red);
	double greenEta = CalcRefraction(green);
	double blueEta = CalcRefraction(blue);
	SetIndexOfRefraction(redEta, greenEta, blueEta);
}

inline void MaterialCookTorrance::SetRefractionFromReflectance( double* rgb )
{
	SetRefractionFromReflectance( *rgb, *(rgb+1), *(rgb+2) );
}

inline void MaterialCookTorrance::SetRefractionFromReflectance( float* rgb )
{
	SetRefractionFromReflectance( *rgb, *(rgb+1), *(rgb+2) );
}

inline void MaterialCookTorrance::SetRefractionFromReflectance( const VectorR3& t ) 
{
	SetRefractionFromReflectance( t.x, t.y, t.z );
}

inline double MaterialCookTorrance::CalcRefraction( double F ) const
{
	assert ( F>=0.0 && F<0.99999 );  // Reflectance should be between 0.0 and 1.0
	double sqrtF = sqrt(F);
	return (1.0+sqrtF)/(1.0-sqrtF);
}

#endif		// MATERIAL_COOKTORRANCE_H
