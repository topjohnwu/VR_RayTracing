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

#include "MaterialCookTorrance.h"
#include "Light.h"
#include "ViewableBase.h"

// This Material works with the Cook-Torrance illumination model.

MaterialBase* MaterialCookTorrance::Clone() const {
	MaterialCookTorrance* ret = new MaterialCookTorrance();
	*ret = *this;
	return (MaterialBase*)ret;
}

double MaterialCookTorrance::GetPhongShininess() const
{
	// Just fake it
	if ( MeanSlope <= 0.2 ) {
		return 400.0*MeanSlope;
	}
	else {
		return Min( 127, (int)( (127.0-80.0)*(MeanSlope-0.2)/0.8 ) );
	}
}


void MaterialCookTorrance::CalcLocalLighting( 
							VectorR3& colorReturned, const Light& light, 
							const VectorR3& percentLit, double lightAttenuation,
							const VectorR3& N, const VectorR3& V, 
							const VectorR3& L, const VectorR3* H ) const
{

	VectorR3 LightValue;	// Used to hold light level components

	if ( percentLit.NearZero(1.0e-6) ) {
		colorReturned.SetZero();	// Light entirely hidden
	}
	else {
		bool facingViewer = ( (N^V)>=0.0 );
		bool facingLight = ( (N^L)>=0.0 );
		if ( (facingLight^facingViewer) && !this->IsTransmissive() ) {
			// Light and viewer on opposite sides and no transmission.
			colorReturned.SetZero();
		}
		else {
			VectorR3 facingNormal(N);	// The normal facing the light
			if (!facingLight) {
				facingNormal.Negate();
			}

			// Diffuse light
			colorReturned = this->GetColorDiffuse();
			colorReturned.ArrayProd(light.GetColorDiffuse());
			colorReturned *= (L^facingNormal);

			if ( !(facingLight^facingViewer) ) {	// If view and light on same side
				if ( facingLight ) {				// If both view and light above
					CalcReflectionColorAbove( L, N, V, &LightValue );
				}
				else {
					CalcReflectionColorBelow( L, N, V, &LightValue );
				}
			}
			else { // If viewer and light on opposite sides
				CalcTransmissionColor( L, N, V, &LightValue );
			}
			LightValue.ArrayProd(light.GetColorSpecular());
			colorReturned += LightValue;

			// Non-ambient light reduced by percentLit
			colorReturned.ArrayProd(percentLit);
		}
	}

	// Ambient light
	LightValue = this->GetColorAmbient();
	LightValue.ArrayProd(light.GetColorAmbient());
	colorReturned += LightValue;

	// Scale by attenuation (the ambient part too)
	colorReturned *= lightAttenuation;
}

VectorR3 MaterialCookTorrance::GetReflectionColor( 
												const VisiblePoint& visPoint, 
												const VectorR3& outDir, 
												const VectorR3& fromDir) const
{
	VectorR3 ret;
	if ( visPoint.IsFrontFacing() ) {
		assert ( (outDir^visPoint.GetNormal())>=0.0 );
		assert ( (fromDir^visPoint.GetNormal())>=0.0 );
		CalcReflectionColorAbove( fromDir, visPoint.GetNormal(), outDir, &ret );
	}
	else {
		assert ( (outDir^visPoint.GetNormal())<=0.0 );
		assert ( (fromDir^visPoint.GetNormal())<=0.0 );
		CalcReflectionColorBelow( fromDir, visPoint.GetNormal(), outDir, &ret );
	}
	ClampMax( &(ret.x), ReflectionFactor.x);
	ClampMax( &(ret.y), ReflectionFactor.y);
	ClampMax( &(ret.z), ReflectionFactor.z);
	return ret;
}

VectorR3 MaterialCookTorrance::GetTransmissionColor( 
												const VisiblePoint& visPoint, 
												const VectorR3& outDir, 
												const VectorR3& fromDir) const 
{
	VectorR3 ret;
	assert ( (outDir^visPoint.GetNormal())>=0.0 || (fromDir^visPoint.GetNormal())>=0.0 );
	assert ( (outDir^visPoint.GetNormal())<=0.0 || (fromDir^visPoint.GetNormal())<=0.0 );
	CalcTransmissionColor( fromDir, visPoint.GetNormal(), outDir, &ret );
	return ret;
}

void MaterialCookTorrance::CalcReflectionColorAbove( 
								const VectorR3& L, const VectorR3& N, const VectorR3 V,
								VectorR3* returnedColor ) const
{
	(*returnedColor) = ColorSpecular;
	if ( (N^V)==0.0 ) {
		returnedColor->SetZero();
		return;
	}
	(*returnedColor) *= (N^L)/(N^V);
	(*returnedColor).ArrayProd(ReflectionFactor);
	VectorR3 H = (L+V);
	if ( !H.NearZero(1.0e-3) ) {
		H.Normalize();
		(*returnedColor) *= CalcGeometricFactor( L, N, V, H );					// G
		double cospsi = (H^N);
		(*returnedColor) *= CalcSlopeDistribution( cospsi );					// D
		double cosphi = (H^L);
		(*returnedColor).x *= CalcFresnelTerm( cosphi, IndexOfRefraction.x );		// F
		(*returnedColor).y *= CalcFresnelTerm( cosphi, IndexOfRefraction.y );
		(*returnedColor).z *= CalcFresnelTerm( cosphi, IndexOfRefraction.z );
	}
}

void MaterialCookTorrance::CalcReflectionColorBelow( const VectorR3& L, const VectorR3& N, const VectorR3 V,
								   VectorR3* returnedColor ) const
{
	(*returnedColor) = ColorSpecular;
	(*returnedColor).ArrayProd(ReflectionFactor);
	if ( (N^V)==0.0 ) {
		returnedColor->SetZero();
		return;
	}
	(*returnedColor) *= (N^L)/(N^V);
	VectorR3 H = (L+V);
	if ( !H.NearZero(1.0e-3) ) {
		H.Normalize();
		(*returnedColor) *= CalcGeometricFactor( L, N, V, H );					// G
		double cospsi = (H^N);
		(*returnedColor) *= CalcSlopeDistribution( cospsi );					// D
		double cosphi = (H^L);
		(*returnedColor).x *= CalcFresnelTerm( cosphi, 1.0/IndexOfRefraction.x );		// F
		(*returnedColor).y *= CalcFresnelTerm( cosphi, 1.0/IndexOfRefraction.y );
		(*returnedColor).z *= CalcFresnelTerm( cosphi, 1.0/IndexOfRefraction.z );
	}
}

void MaterialCookTorrance::CalcTransmissionColor( const VectorR3& L, const VectorR3& N, const VectorR3 V,
								VectorR3* returnedColor ) const
{
	(*returnedColor) = ColorSpecular;
	(*returnedColor).ArrayProd(ReflectionFactor);
	(*returnedColor).ArrayProd(TransmissionFactor);
	if ( (N^V)==0.0 ) {
		returnedColor->SetZero();
		return;
	}
	(*returnedColor) *= -(N^L)/(N^V);
	bool lightAbove = ( (L^N)>0 ) || ( (L^N)<=0 && (V^N)<0.0 );

	// Handle x component
	double eta;
	eta = lightAbove ? IndexOfRefraction.x : 1.0/IndexOfRefraction.x;
	(returnedColor->x) *= CalcTransmissionFactor(L, N, V, eta);

	// Handle y component
	eta = lightAbove ? IndexOfRefraction.y : 1.0/IndexOfRefraction.y;
	(returnedColor->y) *= CalcTransmissionFactor(L, N, V, eta);

	// Handle z component.
	eta = lightAbove ? IndexOfRefraction.z : 1.0/IndexOfRefraction.z;
	(returnedColor->z) *= CalcTransmissionFactor(L, N, V, eta);

}

double MaterialCookTorrance::CalcTransmissionFactor( const VectorR3& L, const VectorR3& N,
													 const VectorR3& V, double eta ) const
{
	VectorR3 H = -(L + eta*V);
	double ret;
	if ( !H.NearZero(1.0e-3) ) {
		H.Normalize();
		if ( (H^L)<0.0 ) {
			H.Negate();
		}
		ret = CalcGeometricXmitFactor( L, N, V, H );					// G
		double cospsi = fabs(H^N);
		ret *= CalcSlopeDistribution( cospsi );	// D
		double cosphi = (H^L);
		ret *= 1.0-CalcFresnelTerm(cosphi, eta);
	}
	else {
		ret = 0.0;
	}
	return ret;
}

// Calculates the Fresnel value F for the Cook-Torrance model (at one wavelength)
// c = cos(theta)
// eta = index of refraction
double MaterialCookTorrance::CalcFresnelTerm( double c, double eta)
{
	double g = SafeSqrt( c*c + eta*eta - 1 );
	double F = 0.5*Square((g-c)/(g+c))*(1+Square((c*(g+c)-1)/(c*(g-c)+1)));
	if ( F<0.0 ) {
		F = 0.0;
	}
	assert ( F<=1.0 );
	return F;
}

// Calculates the D() value for the Cook-Torrance slope distribution.
// Uses the Beckmann distribution function.
// cospsi = cosine of microfacet surface with overall surface normal.
double MaterialCookTorrance::CalcSlopeDistribution( double cospsi ) const
{
	double cosSqInv = 1.0/Square(cospsi);
	double tanpsiSq = cosSqInv-1.0;		// (tan^2 \psi)
	double rSqInv = 1.0/Square(MeanSlope);
	return exp(-tanpsiSq*rSqInv)*Square(cosSqInv)*rSqInv*PIinv;
}

// Compute the geometric occlusion (masking and shadowing) term.
double MaterialCookTorrance::CalcGeometricFactor( const VectorR3& L, const VectorR3& N, 
							const VectorR3& V, const VectorR3& H ) const
{
	VectorR3 M;
	if ( NearZero(N^(L*V),1.0e-7) ) {
		M = N;
	}
	else {
		M = (N^L)*L + (N^V)*V - ((V^L)*(V^N))*L - ((V^L)*(L^N))*V;
		M.Normalize();
	}
	VectorR3 Hprime(M);
	Hprime *= 2.0*(H^M);
	Hprime -= H;			// Normal vector for opposite side of groove
	double G;
	if ( (V^Hprime)>=0 ) {				// If no masking
		G = 1;
	}
	else if ( (L^Hprime)>=0 ) {			// If masking but no shadowing
		G = 2.0*(M^H)*(M^V)/(H^V);
		assert ( G <= 1.0 );
	}
	else if ( (M^V)<(M^L) ) {			// If more masking than shadowing
		G = (M^V)/(M^L);
	}
	else {								// If more shadowing than masking
		G = 1;
	}
	return G;
}

// Compute the geometric occlusion (masking and shadowing) term.
//    This is done by calculating the nonshadowing term s,
//	  the nonmasking term m, and then using   (s+m-1)/s (if positive).
double MaterialCookTorrance::CalcGeometricXmitFactor( const VectorR3& L, const VectorR3& N, 
							const VectorR3& V, const VectorR3& H ) const
{
	VectorR3 M;							// Could save work by passing M in.
	if ( NearZero(N^(L*V),1.0e-7) ) {
		M = N;
	}
	else {
		M = (N^L)*L + (N^V)*V - ((V^L)*(V^N))*L - ((V^L)*(L^N))*N;
		M.Normalize();
	}
	VectorR3 Hprime(M);
	Hprime *= 2.0*(H^M);
	Hprime -= H;			// Normal vector for opposite side of groove

	double s;	// Non-shadowed fraction
	if ( (L^Hprime)<0.0 ) {		
		s = 2.0*(M^H)*(M^L)/(H^L);
	}
	else {
		s = 1.0;		// No shadowing
	}

	double m;
	if ( (V^Hprime)<0.0 ) {
		m = 2.0*(M^H)*(M^V)/(H^V);
	}
	else {
		m = 1.0;		// No masking
	}

	if ( s==0.0 || s+m<=1.0 ) {
		return 0.0;
	}
	else {
		return ( (s+m-1.0)/s );
	}
}

// Use the average of the indices of refraction.
bool MaterialCookTorrance::CalcRefractDir(const VectorR3& normal, const VectorR3& indir, VectorR3& outdir ) const
{
	double meanEta;			// Mean of indices of refraction

	meanEta = (IndexOfRefraction.x+IndexOfRefraction.y+IndexOfRefraction.z)/3.0;
	double meanEtaInv = 1.0/meanEta;
	return MaterialBase::CalcRefractDir( meanEta, meanEtaInv, normal, indir, outdir );
}
