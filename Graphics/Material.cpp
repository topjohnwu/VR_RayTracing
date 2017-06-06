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

#include "Material.h"
#include "Light.h"

// This Material works with the Phong illumination model.

const Material Material::Default;

MaterialBase* Material::Clone() const {
	Material* ret = new Material();
	*ret = *this;
	return (MaterialBase*)ret;
}

void Material::CalcLocalLighting( 
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

			// Specular light
			LightValue = this->GetColorSpecular();
			LightValue.ArrayProd(light.GetColorSpecular());
			double specularFactor;
			if ( !(facingLight^facingViewer) ) {	// If view and light on same side
				if ( H ) {
					specularFactor = ((*H)^facingNormal);
				}
				else {
					// R = -(L-(L^N)N) + (L^N)N = 2(L^N)N - L  // reflected direction
					// R^V = 2(L^N)(V^N) - V^L;
					specularFactor = 2.0*(L^N)*(V^N) - (V^L);	// R^V.
				}
			}
			else { // If viewer and light on opposite sides
				VectorR3 T;		// Transmission direction
				facingNormal = V;
				facingNormal.Negate();		// Use temporarily as incoming vector
				this->CalcRefractDir(N, facingNormal, T);
				specularFactor = (T^L);
			}
			if ( specularFactor>0.0 ) {
				if ( this->GetShininess() != 0.0) {
					specularFactor = pow(specularFactor,this->GetShininess());
				}
				LightValue *= specularFactor;
				colorReturned += LightValue;
			}

			// Non-ambient light reduced by percentLit
			colorReturned.ArrayProd(percentLit);

			if ( (facingLight^facingViewer) ) {		// ^ is "exclusive or"
				// If on opposite sides
				colorReturned.ArrayProd(this->GetColorTransmissive());
			}
		}
	}

	// Ambient light
	LightValue = this->GetColorAmbient();
	LightValue.ArrayProd(light.GetColorAmbient());
	colorReturned += LightValue;

	// Scale by attenuation (the ambient part too)
	colorReturned *= lightAttenuation;

}
