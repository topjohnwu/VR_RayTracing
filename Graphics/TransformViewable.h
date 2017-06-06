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

#ifndef TRANSFORM_VIEWABLE
#define TRANSFORM_VIEWABLE

// The routines in TransformViewable.cpp and .h allow you to transform 
//	  a viewable object with a rigid transformation.  It does this by
//    replacing the data in the viewable object appropriately.
//
// You create a viewable object and then invoke TransformWithRigid
//	  to update it according to the specified rigid transformation.


#include "../VrMath/LinearR4.h"
class ViewableBase;
class ViewableBezierSet;
class ViewableCone;
class ViewableCylinder;
class ViewableEllipsoid;
class ViewableParallelepiped;
class ViewableParallelogram;
class ViewableSphere;
class ViewableTorus;
class ViewableTriangle;
class BezierPatch;

void TransformWithRigid(  ViewableBase* theObject, const RigidMapR3& theTransform );
void TransformWithRigid(  ViewableBezierSet* theObject, const RigidMapR3& theTransform );
void TransformWithRigid(  ViewableCone* theObject, const RigidMapR3& theTransform );
void TransformWithRigid(  ViewableCylinder* theObject, const RigidMapR3& theTransform );
void TransformWithRigid(  ViewableEllipsoid* theObject, const RigidMapR3& theTransform );
void TransformWithRigid(  ViewableParallelepiped* theObject, const RigidMapR3& theTransform );
void TransformWithRigid(  ViewableParallelogram* theObject, const RigidMapR3& theTransform );
void TransformWithRigid(  ViewableSphere* theObject, const RigidMapR3& theTransform );
void TransformWithRigid(  ViewableTorus* theObject, const RigidMapR3& theTransform );
void TransformWithRigid(  ViewableTriangle* theObject, const RigidMapR3& theTransform );
void TransformBezierPatchRecursive( const RigidMapR3& theTransform, BezierPatch* theBp );

#endif    // TRANSFORM_VIEWABLE