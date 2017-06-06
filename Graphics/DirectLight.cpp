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

#include "DirectLight.h"

class LightView;

// This program calculates direct illumination of a surface.
// Input is a position, and a normal, and a light


// For a viewpoint with explicit position
void DirectIlluminateViewPos( const VectorR3& position, const VectorR3& normal,
					  const VectorR3& ViewPos,
					  const Light& light,
					  const MaterialBase& material,
					  VectorR3& colorReturned,
					  const VectorR3& percentLit ) 
{
	// Compute the unnormalized view vector
	VectorR3 viewVector = ViewPos;
	viewVector -= position;
	VectorR3 lightVector;
	double lightReduction;		// Net attenuation factor for light

	if ( !CalcLightDirAndFactor( light, position, 
								 &lightVector, &lightReduction) ) {
		// Hidden from spotlight.
		CalcAmbientOnly(material,light,lightReduction,colorReturned);
	}
	else {
		// Compute the normalized view vector
		viewVector.Normalize();

		// Call the general purpose function with missing H vector
		DirectIlluminateBasic( colorReturned, material, light, 
								  percentLit, lightReduction,
								  normal, viewVector, lightVector, NULL );
	}
}

// For a viewpoint with explicit direction of view 
//    ViewDir must be a normal vector in the direction towards the viewer
void DirectIlluminateViewDir( const VectorR3& position, const VectorR3& normal,
					  const VectorR3& ViewDir,
					  const Light& light,
					  const MaterialBase& material,
					  VectorR3& colorReturned,
					  const VectorR3& percentLit ) 
{
	VectorR3 lightVector;
	double lightReduction;		// Net attenuation factor for light

	if ( !CalcLightDirAndFactor( light, position, 
								 &lightVector, &lightReduction) ) {
		CalcAmbientOnly(material,light,lightReduction,colorReturned);
	}
	else {
		// Call the general purpose function with null H vector ptr
		DirectIlluminateBasic( colorReturned, material, light, 
								  percentLit, lightReduction,
								  normal, ViewDir, lightVector, NULL );
	}
}


// For a view structure (may or may not be a local viewer)
void DirectIlluminate( const VectorR3& position, const VectorR3& normal,
					  const View& view,
					  const Light& light,
					  const MaterialBase& material,
					  VectorR3& colorReturned,
					  const VectorR3& percentLit )
{
	if ( view.IsLocalViewer() ) {
		DirectIlluminateViewPos( position, normal, view.GetPosition(), light, material,
						  colorReturned, percentLit );
	}
	else {
		VectorR3 viewVector(view.GetDirection());
		viewVector.Negate();
		DirectIlluminateViewDir( position, normal, viewVector, light, material,
						  colorReturned, percentLit );
	}
}

// For a LightView combined class
void DirectIlluminate( const VectorR3& position, const VectorR3& normal,
					  const LightView& lv,
					  const MaterialBase& material,
					  VectorR3& colorReturned,
					  const VectorR3& percentLit )
{
	if ( !lv.GetView().IsLocalViewer() ) {
		DirectIlluminateViewPos( position, normal, lv.GetView().GetPosition(), 
								lv.GetLight(), material,
								colorReturned, percentLit );
		return;
	}

	VectorR3 viewVector;
	VectorR3 lightVector;
	double lightReduction;			// Net attenuation factor for light

	viewVector = lv.GetView().GetDirection();
	viewVector.Negate();				// Unit vector towards viewer
	
	if ( !CalcLightDirAndFactor( lv.GetLight(), position, 
								 &lightVector, &lightReduction) ) {
		// Hidden from spotlight.
		CalcAmbientOnly(material,lv.GetLight(),lightReduction,colorReturned);
	}
	else {
		// Call the general purpose function
		DirectIlluminateBasic( colorReturned, material, lv.GetLight(), 
								  percentLit, lightReduction,
								  normal, viewVector, lightVector, &(lv.GetH()) );
	}
}

// Calculate light attenuation due to distance and to spotlight effect.
// Verify that the light is shining on the surface.
// Returns: (a) whether the light is shining on the surface (bool)
//			(b) the lightVector (unit vector towards the light)
//			(c) light attenuation factor

bool CalcLightDirAndFactor(const Light& light, 
						   const VectorR3& position, 
						   VectorR3 *lightVector, 
						   double *lightAttenuate ) 
{
	*lightAttenuate = 1.0f;

	if ( light.IsPositional() ) {
		// Compute the (un)normalized light vector
		*lightVector = light.GetPosition();
		*lightVector -= position;
		double distSq = lightVector->NormSq();
		double dist = sqrt( distSq );
		// Normalize the vector towards the light
		*lightVector /= dist;			// Unit vector towards light
		// Compute the attenuation factor for the light
		if ( light.AttenuateActive() ) {
			*lightAttenuate = 1.0 / ( light.GetAttenuateConstant() 
									 + dist*light.GetAttenuateLinear() 
									 + distSq*light.GetAttenuateQuadratic() );
		}

		// Compute the spotlight attenuation factor for the light
		if ( light.SpotActive() ) {
			double cosine = -((*lightVector)^light.GetSpotDirection());
			if ( cosine < light.GetSpotCutoff() ) {
				return false;
			}
			if ( light.GetSpotExponent() != 0.0 ) {
				if ( light.GetSpotExponent() == 1.0 ) {
					*lightAttenuate *= cosine;
				}
				else {
					*lightAttenuate *= pow( cosine, light.GetSpotExponent() );
				}
			}
		}
	}
	else {
		*lightVector = light.GetPosition();
		lightVector->Negate();						// Unit vector towards light
	}

	return true;

}

void LightView::CalcH() {

	// Calculate the direction towards the light
	if ( light->IsDirectional() ) {
		H = light->GetPosition();
	}
	else {
		H = light->GetPosition();
		H -= view->GetPosition();	// Direction from the viewer
		H.Normalize();
	}

	// Add on direction *towards* the viewer
	H -= view->GetDirection();

	H.Normalize();		// Make a unit vector
}

// Calculate the response to the ambient light
// This calculation can apply to both the Phong and Cook-Torrance models
//		so is currently used for all lights.
void CalcAmbientOnly( const MaterialBase& mat, const Light& light, double lightAttenuation,
					  VectorR3& colorReturned )
{
	colorReturned = mat.GetColorAmbient();
	colorReturned.ArrayProd(light.GetColorAmbient());
	colorReturned *= lightAttenuation;
}

// Here is the fundamental routine for calculating direct illumination from
//		a single light.  It calls the material, which uses its illumination model.
//		(i.e., Phong or Cook-Torrance).
// N = surface normal
// L = Unit vector towards light (surface may be backfacing to light)
// V = Unit vector towards viewer (surface may be backfacing to viewer)
// H = H vector (or null pointer).
// lightAttenuation - net attenuation factor for this light.
// light and material have the basic relevant properties needed for the illumination
//		calculation.

void DirectIlluminateBasic( VectorR3& colorReturned, const MaterialBase& material, 
						    const Light& light, 
							const VectorR3& percentLit, double lightAttenuation,
							const VectorR3& N, const VectorR3& V, 
							const VectorR3& L, const VectorR3* H )
{
	material.CalcLocalLighting( colorReturned, light, percentLit, lightAttenuation,
								N, V, L, H );
	return;
}



