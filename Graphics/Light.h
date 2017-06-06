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

// New in version 3.0 and later: 
//  The "Color4" versions of colors are no longer supported.
//  Likewise, the "Color3" routines have been renamed.
//  To convert your code, replace each "Color3" or "Color4"
//    function name with just "Color"

#ifndef LIGHT_H
#define LIGHT_H

#include "../VrMath/LinearR4.h"

class Light {

public:

	Light() { Reset(); }

	void Reset();

	// Call SetPosition... to set the light's position.
	//		This makes the light positional.
	void SetPosition( double x, double y, double z);
	void SetPosition( double* pos );
	void SetPosition( float* pos );
	void SetPosition( const VectorR3& pos );

	// SetDirectional() - makes the light directional
	void SetDirectional( double x, double y, double z );
	void SetDirectional( double *dir );
	void SetDirectional( float *dir );
	void SetDirectional( const VectorR3& pos );

	bool IsDirectional() const { return Directional; }
	bool IsPositional() const { return !Directional; }

	void SetColor( double r, double g, double b);
	void SetColorAmbient( double r, double g, double b);
	void SetColorDiffuse( double r, double g, double b);
	void SetColorSpecular( double r, double g, double b);

	void SetColor( double* color );
	void SetColorAmbient( double* color );
	void SetColorDiffuse( double* color );
	void SetColorSpecular( double* color );

	void SetColor( float* color );
	void SetColorAmbient( float* color );
	void SetColorDiffuse( float* color );
	void SetColorSpecular( float* color );

	void SetColor( const VectorR3& color );
	void SetColorAmbient( const VectorR3& color );
	void SetColorDiffuse( const VectorR3& color );
	void SetColorSpecular( const VectorR3& color );

	void GetPosition( double* pos ) const;
	void GetPosition( float* pos ) const;
	const VectorR3& GetPosition() const;

	void GetColorAmbient( double* ) const;
	void GetColorDiffuse( double* ) const;
	void GetColorSpecular( double* ) const;
	void GetColorAmbient( float* ) const;
	void GetColorDiffuse( float* ) const;
	void GetColorSpecular( float* ) const;

	const VectorR3& GetColorAmbient() const;
	const VectorR3& GetColorDiffuse() const;
	const VectorR3& GetColorSpecular() const;

	void SetAttenuate( double AConst, double ALinear=0.0, double AQuadratic=0.0 );
	void ResetAttenuate();
	bool AttenuateActive() const { return AttenuateFlag; }
	double GetAttenuateConstant() const { return AttenuateConstant; }
	double GetAttenuateLinear() const { return AttenuateLinear; }
	double GetAttenuateQuadratic() const { return AttenuateQuadratic; }
	void GetAttenuate( double* coefs ) const;
	void GetAttenuate( float* coefs ) const;

	bool SpotActive() const { return SpotlightFlag; }
	void ResetSpotlight();
	void SetSpotDirection( double x, double y, double z );
	void SetSpotCutoff( double cosvalue );
	void SetSpotExponent( double exponent );

	void GetSpotDirection( double* dir ) const;
	void GetSpotDirection( float* dir ) const;
	const VectorR3& GetSpotDirection() const { return SpotDirection; }
	double GetSpotCutoff()  const { return SpotCutoffCosine; }
	double GetSpotCutoffAngle()  const { return acos(SpotCutoffCosine); }
	double GetSpotExponent()  const { return SpotAttenuate; }

private:

	bool Directional;		// Equals true if the light is directional
							// Equals false if the light is positional
	VectorR3 Position;		// Coordinates of position if positional
							// (Negation of) direction if directional

	VectorR3 ColorAmbient;	// Ambient component's color
	VectorR3 ColorDiffuse;	// Diffuse component's color
	VectorR3 ColorSpecular; // Specular component's color

	// Distance attenuation constants
	bool AttenuateFlag;			// Is attenuation active?
	double AttenuateConstant;	// Constant term in attenuation
	double AttenuateLinear;		// Linear term in attenuation
	double AttenuateQuadratic;	// Quadratic term in attenuation

	// Spotlight data. Used only for positional lights
	bool SpotlightFlag;			// Is this light a spotlight?
	VectorR3 SpotDirection;		// Direction of spotlight
	double SpotCutoffCosine;	// Cosine of angle for cutoff
	double SpotAttenuate;		// Attenuation exponent

};

inline void Light::Reset() {

	Directional=true;
	Position.Set(0.0, 0.0, 1.0);	// Pointing down z-axis

	SetColorAmbient( 1.0, 1.0, 1.0 );
	SetColorDiffuse( 1.0, 1.0, 1.0 );
	SetColorSpecular( 1.0, 1.0, 1.0 );

	ResetAttenuate();

	ResetSpotlight();
}
	
inline void Light::SetPosition( double x, double y, double z ) {
	Directional = false;
	Position.Set( x, y, z );
}

inline void Light::SetDirectional( double x, double y, double z ) {
	Directional = true;
	Position.Set( x, y, z );		// This is positioned at infinity
									// The direction is (-x, -y, -z)
	Position.Normalize();
}

inline void Light::SetPosition( double* pos ) {
	SetPosition( *pos, *(pos+1), *(pos+2) );
}

inline void Light::SetPosition( float* pos ) {
	SetPosition( *pos, *(pos+1), *(pos+2) );
}

inline void Light::SetDirectional( double *dir ) {
	SetDirectional( *dir, *(dir+1), *(dir+2) );
}

inline void Light::SetDirectional( float *dir ) {
	SetDirectional( *dir, *(dir+1), *(dir+2) );
}

inline void Light::SetPosition( const VectorR3& pos ) {
	SetPosition( pos.x, pos.y, pos.z );
}

inline void Light::SetDirectional( const VectorR3& dir ) {
	SetDirectional( dir.x, dir.y, dir.z );
}

inline void Light::GetPosition( double* pos ) const {
	Position.Dump(pos);
}

inline void Light::GetPosition( float* pos ) const {
	Position.Dump(pos);
}

inline const VectorR3& Light::GetPosition() const {
	return Position;
}

inline void Light::SetColor( double r, double g, double b ) {
	SetColorAmbient ( r, g, b );
	SetColorDiffuse ( r, g, b );
	SetColorSpecular ( r, g, b );
}

inline void Light::SetColorAmbient( double r, double g, double b ) {
	ColorAmbient.Set( r, g, b );
}

inline void Light::SetColorDiffuse( double r, double g, double b ) {
	ColorDiffuse.Set( r, g, b );
}

inline void Light::SetColorSpecular( double r, double g, double b ) {
	ColorSpecular.Set( r, g, b );
}

inline void Light::SetColor( double* color ) {
	SetColorAmbient( color );
	SetColorDiffuse( color );
	SetColorSpecular( color );
}

inline void Light::SetColorAmbient( double* color ) {
	SetColorAmbient( *color, *(color+1), *(color+2) );
}

inline void Light::SetColorDiffuse( double* color ) {
	SetColorDiffuse( *color, *(color+1), *(color+2) );
}

inline void Light::SetColorSpecular( double* color ) {
	SetColorSpecular( *color, *(color+1), *(color+2) );
}

inline void Light::SetColor( float* color ) {
	SetColorAmbient( color );
	SetColorDiffuse( color );
	SetColorSpecular( color );
}

inline void Light::SetColorAmbient( float* color ) {
	SetColorAmbient( *color, *(color+1), *(color+2) );
}

inline void Light::SetColorDiffuse( float* color ) {
	SetColorDiffuse( *color, *(color+1), *(color+2) );
}

inline void Light::SetColorSpecular( float* color ) {
	SetColorSpecular( *color, *(color+1), *(color+2) );
}

inline void Light::SetColor( const VectorR3& color ) {
	SetColorAmbient( color );
	SetColorDiffuse( color );
	SetColorSpecular( color );
}

inline void Light::SetColorAmbient( const VectorR3& color ) {
	SetColorAmbient( color.x, color.y, color.z );
}

inline void Light::SetColorDiffuse( const VectorR3& color ) {
	SetColorDiffuse( color.x, color.y, color.z );
}

inline void Light::SetColorSpecular( const VectorR3& color ) {
	SetColorSpecular( color.x, color.y, color.z );
}

inline void Light::GetColorAmbient( double* c) const {
	ColorAmbient.Dump(c);
}

inline void Light::GetColorDiffuse( double* c) const {
	ColorDiffuse.Dump(c);
}

inline void Light::GetColorSpecular( double* c) const {
	ColorSpecular.Dump(c);
}

inline void Light::GetColorAmbient( float* c) const {
	ColorAmbient.Dump(c);
}

inline void Light::GetColorDiffuse( float* c) const {
	ColorDiffuse.Dump(c);
}

inline void Light::GetColorSpecular( float* c) const {
	ColorSpecular.Dump(c);
}

inline const VectorR3& Light::GetColorAmbient() const {
	return ColorAmbient;
}

inline const VectorR3& Light::GetColorDiffuse() const {
	return ColorDiffuse;
}

inline const VectorR3& Light::GetColorSpecular() const {
	return ColorSpecular;
}

inline void Light::SetAttenuate( double AConst, double ALinear, double AQuadratic ) {
	AttenuateConstant = AConst;
	AttenuateLinear = ALinear;
	AttenuateQuadratic = AQuadratic;
	AttenuateFlag = (AConst!=1.0) || (ALinear!=0.0) || (AQuadratic!=0.0);
}

inline void Light::ResetAttenuate() {
	AttenuateConstant = 1.0;
	AttenuateLinear = 0.0;
	AttenuateQuadratic = 0.0;
	AttenuateFlag = false;
}		

inline void Light::GetAttenuate( double* coefs ) const {
	*(coefs) = AttenuateConstant;
	*(coefs+1) = AttenuateLinear;
	*(coefs+2) = AttenuateQuadratic;
}

inline void Light::GetAttenuate( float* coefs ) const {
	*(coefs) = (float)AttenuateConstant;
	*(coefs+1) = (float)AttenuateLinear;
	*(coefs+2) = (float)AttenuateQuadratic;
}

inline void Light::ResetSpotlight() {
	SpotlightFlag = false;
	SpotDirection.Set(0.0, 0.0, -1.0);
	SpotCutoffCosine = -1.0;
	SpotAttenuate = 0.0;
}

inline void Light::SetSpotDirection( double x, double y, double z ) {
	SpotDirection.Set(x,y,z);
	SpotDirection.Normalize();
	SpotlightFlag = true;
}

inline void Light::SetSpotCutoff( double cosvalue ) {
	SpotCutoffCosine = cosvalue;
	SpotlightFlag = true;
}

inline void Light::SetSpotExponent( double exponent ) {
	SpotAttenuate = exponent;
	SpotlightFlag = true;
}

inline void Light::GetSpotDirection( double* dir ) const {
	SpotDirection.Dump(dir);
}

inline void Light::GetSpotDirection( float* dir ) const {
	SpotDirection.Dump(dir);
}

#endif // LIGHT_H
