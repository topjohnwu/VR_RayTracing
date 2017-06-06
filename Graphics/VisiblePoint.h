/*
 *
 * RayTrace Software Package, release 3.2.  May 3, 2007.
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

#ifndef VISIBLEPOINT_H
#define VISIBLEPOINT_H

// ****************************************************************************
// The class   VisiblePoint   is defined in this file.
// ****************************************************************************

#include "../VrMath/LinearR2.h"
#include "../VrMath/LinearR3.h"
#include "MaterialBase.h"
class ViewableBase;

//  VisiblePoint is a class storing information about a visible point.

class VisiblePoint {
	friend class ViewableBase;
	
public:
	VisiblePoint() { FrontFace = true; MatNeedsFreeing = false; };
	VisiblePoint(const VisiblePoint &p);
	~VisiblePoint();

	VisiblePoint& operator=(const VisiblePoint& p);

	void SetPosition( const VectorR3& pos ) { Position = pos;}
	void SetNormal( const VectorR3& normal ) { Normal = normal; }
	void SetMaterial( const MaterialBase& material );
	void SetFrontFace ( bool frontface=true ) { FrontFace = frontface; }
	void SetBackFace () { FrontFace = false; }

	bool IsFrontFacing() const { return FrontFace; }
	bool IsBackFacing() const { return !FrontFace; }

	const VectorR3& GetPosition() const { return Position; }
	const VectorR3& GetNormal() const { return Normal; }
	const MaterialBase& GetMaterial() const { return *Mat; }
	MaterialBase& GetMaterialMutable() { assert(MatNeedsFreeing); return *Mat; }

	void SetUV( double u, double v ) { uvCoords.Set(u,v); }
	void SetUV( const VectorR2& uv ) { uvCoords = uv; }

	double GetU() const { return uvCoords.x; }
	double GetV() const { return uvCoords.y; }
	const VectorR2& GetUV() const { return uvCoords; }
	VectorR2& GetUV() { return uvCoords; }

	// Face numbers allow different texture maps to be applied to different faces of an object.
	// Typically, the front and back side of a face get the same face number.  However, they
	//  get different texture maps, and also "FrontFace" can be used to distinguish front and back faces.
	// Face numbers are non-negative integers.  Generally: 0 is the "main" face.
	void SetFaceNumber( int faceNumber ) { FaceNumber = faceNumber; }	
	int GetFaceNumber() const { return FaceNumber; }		
	
	void SetObject( const ViewableBase *object ) { TheObject = object; }
	const ViewableBase& GetObject() const { return *TheObject; }

	void MakeMaterialMutable();	

private:
	VectorR3 Position;
	VectorR3 Normal;		// Outward Normal
	MaterialBase* Mat;
	VectorR2 uvCoords;		// (u,v) coordinates for texture mapping & etc.
	int FaceNumber;			// Index of face number (non-negative).
	const ViewableBase* TheObject;		// The object from which the visible point came.
	bool FrontFace;			// Is it being viewed from the front side?
	
	bool MatNeedsFreeing;	// true if we are responsible for freeing the material.

};

inline VisiblePoint::VisiblePoint(const VisiblePoint& vp)
{
	MatNeedsFreeing = false;
	*this = vp;
}

inline VisiblePoint::~VisiblePoint() 
{
	if ( MatNeedsFreeing ) {
		delete Mat;
	}
}

inline VisiblePoint& VisiblePoint::operator=(const VisiblePoint& vp) 
{
	Position = vp.Position;
	Normal = vp.Normal;
	uvCoords = vp.uvCoords;
	FaceNumber = vp.FaceNumber;
	TheObject = vp.TheObject;
	FrontFace = vp.FrontFace;

	if ( MatNeedsFreeing ) {
		delete Mat;
	}

	MatNeedsFreeing = vp.MatNeedsFreeing;
	if (MatNeedsFreeing) {
		Mat = vp.Mat->Clone();
	} else {
		Mat = vp.Mat;
	}

	return *this;
}

inline void VisiblePoint::SetMaterial( const MaterialBase& material )
{
	if ( MatNeedsFreeing ) {
		delete Mat;
	}
	Mat = const_cast<MaterialBase*>(&material);
	MatNeedsFreeing = false;
}

// Mutable and deletable materials are the same thing (properties not separable)

inline void VisiblePoint::MakeMaterialMutable( ) 
{
	if ( MatNeedsFreeing ) {
		return;						// Material already mutable.
	}
	else {
		Mat = Mat->Clone();
		MatNeedsFreeing = true;
	}
}


#endif // VISIBLEPOINT_H
