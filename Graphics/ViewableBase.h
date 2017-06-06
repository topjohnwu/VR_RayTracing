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

#ifndef VIEWABLEBASE_H
#define VIEWABLEBASE_H

// ****************************************************************************
// The classes    ViewableBase   and   VisiblePoint   are defined in this file.
// ****************************************************************************

#include <math.h>
#include "../VrMath/LinearR3.h"
class AABB;
#include "TextureMapBase.h"
#include "VisiblePoint.h"

// This is the purely abstract base class for viewable objects.
//		Any ViewableBase class is responsible for determining
//		it it intersects a given line of sight.  Thus it must
//		perform intersection and distance tests, and when successful
//		must return depth information.
class ViewableBase {

public:
	ViewableBase();

	// Returns an intersection if found with distance maxDistance
	// viewDir must be a unit vector.
	// intersectDistance and visPoint are returned values.
	virtual bool FindIntersection ( 
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double *intersectDistance, VisiblePoint& returnedPoint ) const;

	// Sets front and back texture maps.
	// Subclasses of ViewablePoint will have more routines.
	void TextureMap( const TextureMapBase* texture );	// Front & back
	void TextureMapFront( const TextureMapBase* texture );
	void TextureMapBack( const TextureMapBase* texture );
	void TextureMapOuter( const TextureMapBase* texture );
	void TextureMapInner( const TextureMapBase* texture );

	bool HasFrontTextureMap() const { return (TextureFront!=0); }
	bool HasBackTextureMap() const { return (TextureBack!=0); }
	bool HasInnerTextureMap() const { return (TextureFront!=0); }
	bool HasOuterTextureMap() const { return (TextureBack!=0); }

	// CalcBoundingPlanes:
	//   Computes the extents of the viewable object with respect to a
	//		a given normal direction  u.   u is the normal to a family of planes.
	//   Returns  min{u^x} and max{u^x} for x a point on the object (^ = dot product)
	//   Important: u must be a unit vector!
	virtual void CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const = 0;

	// Calculate the axis aligned bounding box
	virtual void CalcAABB( AABB& retAABB ) const;

	// Calculate the extent intersected with a bounding box
	virtual bool CalcExtentsInBox( const AABB& aabb, AABB& retAABB ) const;

	// CalcPartials:
	//   Returns partial derivatives with respect to u and v.
	//   Letting p(u,v) be the point on the surface with coord's u,v;
	//   CalcPartials returns (partial p)/(partial u) and (partial p)/(partial v).
	//		in the vectors retPartialU and retPartialV.
	//	 If at a singularity (so either partial is zero or infinity, then the boolean
	//		return value is FALSE.  Otherwise the boolean return value is TRUE to indicate
	//		that the returned partial derivative information is valid.
	// Needed only for bump mapping at present.
	virtual bool CalcPartials( const VisiblePoint& visPoint, 
							   VectorR3& retPartialU, VectorR3& retPartialV ) const = 0;

	// For run time typing, we use the following "type code":
	enum ViewableType { 
			Viewable_BezierSet, 
			Viewable_Cone, 
			Viewable_Cylinder, 
			Viewable_Ellipsoid,
			Viewable_Parallelepiped,
			Viewable_Parallelogram,
			Viewable_Sphere,
			Viewable_Torus,
			Viewable_Triangle };
	virtual ViewableType GetViewableType() const = 0;

protected:

	const TextureMapBase* TextureFront;		// Front texture map
	const TextureMapBase* TextureBack;		// Back Texture map

	// The "NT" version is the one that does all the work of finding
	//		the intersection point, and computing u,v coordinates.
	//	The "NT" version does not call the texture map: this is left for
	//		the non-NT version to do.
	virtual bool FindIntersectionNT ( 
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double *intersectDistance, VisiblePoint& returnedPoint ) const = 0;
};

inline ViewableBase::ViewableBase()
{
	TextureMap( 0 );
}

inline void ViewableBase::TextureMap( 
				const TextureMapBase* texture )
{
	TextureMapFront( texture );
	TextureMapBack( texture );
}

inline void ViewableBase::TextureMapFront( const TextureMapBase* texture )
{
	TextureFront = texture;
}

inline void ViewableBase::TextureMapBack( const TextureMapBase* texture )
{
	TextureBack = texture;
}

inline void ViewableBase::TextureMapOuter( const TextureMapBase* texture )
{
	TextureMapFront( texture );
}

inline void ViewableBase::TextureMapInner( const TextureMapBase* texture )
{
	TextureMapBack( texture );
}

inline bool ViewableBase::FindIntersection ( 
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double *intersectDistance, VisiblePoint& returnedPoint ) const 
{
	bool found;
	found = FindIntersectionNT(viewPos, viewDir, 
								maxDistance, intersectDistance, returnedPoint);
	if ( found ) {
		returnedPoint.SetObject( this );
		// Invoke the texture map (if any)
		const TextureMapBase* texmap = returnedPoint.IsFrontFacing() ? TextureFront : TextureBack;
		if ( texmap ) {
			texmap->ApplyTexture( returnedPoint, viewDir );
		}
	}
	return found;
}


// NOW THEY ARE PURELY VIRTUAL
// Eventually these routine should be purely virtual, but first I have to implement
//		lots of special cases.
//inline bool ViewableBase::CalcPartials( const VisiblePoint& visPoint, 
//									    VectorR3& retPartialU, VectorR3& retPartialV ) const
//{
//	assert(0);		// Not yet implemented for this class!
//	return false;
//}

// This routine could be made purely virtual, but perhaps it is not implemented
//		for all classes.
//inline void ViewableBase::CalcBoundingPlanes( const VectorR3& u, 
//											  double *minDot, double *maxDot )
//{
//	assert(0);			// Not implemented for this class (comment out if desired)
//	*minDot = -DBL_MAX;
//	*maxDot =  DBL_MAX;
//}

#endif // VIEWABLEBASE_H
