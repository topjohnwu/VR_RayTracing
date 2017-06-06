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

#ifndef VIEWABLEBEZIERSET_H
#define VIEWABLEBEZIERSET_H

#include "ViewableBase.h"
#include "Material.h"
#include "../DataStructs/Array.h"
#include "../VrMath/Parallelepiped.h"
#include "../DataStructs/CLinkedList.h"

class BezierPatch;
class BezierPatchMgr;

// Dynamic arrays of Bezier patches
typedef Array<BezierPatch> BezierArray;

// Linked lists of Bezier patches
typedef CLinkedList<BezierPatch> BezierList;
typedef CLinkedListElt<BezierPatch> BezierListElt;

// ***********************************************************************************
// * ViewableBezierSet class - holds a set of degree 3x3 Bezier patches				 *
// ***********************************************************************************
class ViewableBezierSet : public ViewableBase {

	friend void TransformWithRigid(ViewableBezierSet*, const RigidMapR3& );

public:

	// Constructors
	ViewableBezierSet();
	virtual ~ViewableBezierSet();

	// Returns an intersection if found with distance maxDistance
	// viewDir must be a unit vector.
	// intersectDistance and visPoint are returned values.
	virtual bool FindIntersectionNT ( 
		const VectorR3& viewPos, const VectorR3& viewDir, double maxDistance,
		double *intersectDistance, VisiblePoint& returnedPoint ) const;
	void CalcBoundingPlanes( const VectorR3& u, double *minDot, double *maxDot ) const;
	bool CalcPartials( const VisiblePoint& visPoint, 
					   VectorR3& retPartialU, VectorR3& retPartialV ) const;
	ViewableType GetViewableType() const { return Viewable_BezierSet; }

	int AddPatch(int uOrder, int vOrder, const double* controlPoints, int uStride = 0, int vStride = 0);
	int AddRationalPatch(int uOrder, int vOrder, const double* controlPoints, int uStride = 0, int vStride = 0);
	int AddPatch(int uOrder, int vOrder, const float* controlPoints, int uStride = 0, int vStride = 0);
	int AddRationalPatch(int uOrder, int vOrder, const float* controlPoints, int uStride = 0, int vStride = 0);
	int AddPatch(int uOrder, int vOrder, const VectorR3* controlPoints, int uStride = 0, int vStride = 0);
	int AddPatch(int uOrder, int vOrder, const VectorR4* controlPoints, int uStride = 0, int vStride = 0);

	// Set the default materials (can be different for each patch).
	void SetMaterial( const MaterialBase *material ) 
						{ FrontMaterial = material; BackMaterial = material; };
	void SetMaterialFront( const MaterialBase *material ) 
						{ FrontMaterial = material; };
	void SetMaterialBack( const MaterialBase *material ) 
						{ BackMaterial = material; };

	// Set u-v texture map translation amount
	// Only square translation permitted.
	void SetUvRange ( double uMin, double vMin, double uMax, double vMax );
	void SetUvRange ( const VectorR2& uvMin, const VectorR2& uvMax );

	// Bounding sphere info.  Will be automatically calculated as best it can.
	// The following routines can be used to override the default center of the bounding sphere
	void SetBoundingSphereCenter( double x, double y, double z );
	void SetBoundingSphereCenter( const double *center );
	void SetBoundingSphereCenter( const float *center );
	void SetBoundingSphereCenter( const VectorR3& center );
	void UnsetBoundingSphereCenter( );

	const MaterialBase* GetMaterialFront () const { return FrontMaterial; }
	const MaterialBase* GetMaterialBack () const { return BackMaterial; }
	void GetBoundingSphereCenter( double *center);
	void GetBoundingSphereCenter( float *center);
	const VectorR3& GetBoundingSphereCenter() const { return BoundingSphereCenter; }

	BezierArray& GetPatchList() { return OriginalPatches; }
	const BezierArray& GetPatchList() const { return OriginalPatches; }
	BezierArray& GetInteralPatchList() { return PatchList; }
	const BezierArray& GetInteralPatchList() const { return PatchList; }

protected:

	int PatchCounter;					// Number of logical patches

	BezierArray PatchList;				// A list of degree 3 Bezier patches.
										// Includes refinements of the original patches

	BezierArray OriginalPatches;		// The original 4x4 patches before subdivisions

	// The uv values for the next patch specified by AddPatch or AddRationalPatch
	VectorR2 NextUvMin, NextUvMax;

	// These are the default materials for each patch, but can be overridden.
	const MaterialBase* FrontMaterial;
	const MaterialBase* BackMaterial;

	bool BoundingSphereSet;				// Bounding sphere calculated?
	bool BoundingSphereManuallySet;		// Bounding sphere center been set by the user?
	VectorR3 BoundingSphereCenter;		// Center of bounding sphere
	double BoundingSphereRadiusSq;		// Square of radius of bounding sphere

	// Mostly intended for internal use
	int AddPatchInner(int uOrder, int vOrder, VectorR4 controlPoints[4][4]);
	void CalcBoundingSphere();
	void CalcBoundingSphereCenter();

};

// ***********************************************************************************
// * BezierPatch class -  A single degree 3x3 Bezier patch							 *
// ***********************************************************************************
class RigidMapR3;

class BezierPatch {
	friend class ViewableBezierSet;
	friend class BezierPatchMgr;
	friend void TransformWithRigid(ViewableBezierSet*, const RigidMapR3& );
	friend void TransformBezierPatchRecursive( const RigidMapR3& theTransform, BezierPatch* theBp );

public:
	BezierPatch();
	BezierPatch( VectorR4 controlPts[4][4] );
	void SetControlPoints( VectorR4 controlPts[4][4] );

	void SetUvRange( const VectorR2& uvMin, const VectorR2 uvMax );

	bool HasPointAtInfinity();			// True if some point at infinity
	void EvalPatch( const VectorR2& Uv, VectorR3* val) const;
	void EvalPatch( const VectorR2& Uv, VectorR4* valRational) const;
	void EvalPatchNormal( const VectorR2& Uv, const VectorR4& patchValue, 
						  VectorR3* val, const VectorR3* backupNormal = 0) const;

	const VectorR4* GetControlPoints( ) const { return &CntlPts[0][0]; }

	static void BiLinearInvert( const VectorR3& point, 
								const VectorR4& cornerX, const VectorR4& cornerY,
								const VectorR4& cornerZ, const VectorR4& cornerW, 
								const VectorR3& Normal, VectorR2* UvValue );

	static void DeCasteljauSplit( const VectorR4 in[4], VectorR4[7] );
	static void DeCasteljauSplit( double alpha, const VectorR4 in[4], VectorR4[7] );
	static void EvalDeriv3( double u, const VectorR4& value,
							const VectorR4 cntlPts[4], VectorR3* retVal );
	static bool EvalDeriv4( double u, const VectorR4 cntlPts[4], VectorR4* retVal );
	static void EvalFirstDeriv3( double u, const VectorR4& value,
								 const VectorR4 cntlPts[4], VectorR3* retVal );
	static void EvalFirstDeriv4( double u, const VectorR4 cntlPts[4], VectorR4* retVal );
	bool CalcPartials( const VectorR2& uvCoords, 
					   VectorR3& retPartialU, VectorR3& retPartialV ) const;

protected:
	VectorR4 CntlPts[4][4];		// 4x4 array of control for order 4x4 patch
	VectorR2 UvMin, UvMax;		// U and V min and max coordinates (texture coordinates)

	const MaterialBase* BackMaterial;
	const MaterialBase* FrontMaterial;

	int FaceNum;			// The number of the original patch this belongs to

	// Next variables describe the bounding parallelepiped.
	VectorR3 NormalC;			// In the upward direction, more-or-less
	VectorR3 NormalA;			// In the "u" direction, more-or-less
	VectorR3 NormalB;			// In the "v" direction, more-or-less
	double MinDotC, MaxDotC;
	double MinDotA, MaxDotA;
	double MinDotB, MaxDotB;
	Parallelepiped BoundingPpd;	// Bounding parallelepiped (redundantly specified)

	bool BoundingPpdBad;

	int MgrRecurseLevel;			// Recursion level ==0 for "original" patch
	bool IsMgrAutoAllocated();		// If allocated by the BezierPatchMgr
	bool MgrNeedToRecurse();		// If needs more recursion
	BezierPatch* SplitPatchA;		// Pointer to first subpatch
	BezierPatch* SplitPatchB;		// Pointer to second subpatch
	bool IsSplitIntoTwo() const { return (SplitPatchA!=0); } 

	void MakeSplitU ( BezierPatch* u0, BezierPatch* u1 );
	void MakeSplitV ( BezierPatch* v0, BezierPatch* v1 );
	void CalcBoundingPpd();		// Compute a bounding parallelepiped
	bool BoundingPpdNice();		// Is the bounding parallelepiped nice?
	void GetMinMaxDot( const VectorR3& n, double* minDot, double* maxDot ) const;
	void GetMinMaxDotRecursive( const VectorR3& n, double* minDot, double* maxDot ) const;
};

// ***********************************************************************************
// * BezierPatchMgr class -  Manages allocating and freeing Bezier patchs			 *
// ***********************************************************************************

class BezierPatchMgr {
	friend class ViewableBezierSet;
public:
	static int MaxRecurseSave;
	static int MinIsectRecurse;

protected:
	static void GetTwoSubPatchs ( BezierPatch& bpIn, 
								  BezierPatch** bpOut1,BezierPatch** bpOut2 );
	static void ReleaseBezierPatch ( BezierPatch* bp );
};

// ***********************************************************************************
// * ViewableBezierSet class - Member functions										 *
// ***********************************************************************************

inline ViewableBezierSet::ViewableBezierSet() {
	PatchCounter = 0;
	SetUvRange(0.0, 0.0, 1.0, 1.0);
	BoundingSphereSet = false;
	BoundingSphereManuallySet = false;
	BackMaterial = &Material::Default;
	FrontMaterial = &Material::Default;
}

inline ViewableBezierSet::~ViewableBezierSet() {}

inline void ViewableBezierSet::SetUvRange ( double uMin, double vMin, 
										    double uMax, double vMax )
{
	NextUvMin.Set(uMin,vMin);
	NextUvMax.Set(uMax,vMax);
}

inline void ViewableBezierSet::SetUvRange ( const VectorR2& uvMin, const VectorR2& uvMax )
{
	NextUvMin = uvMin;
	NextUvMax = uvMax;
}

inline void ViewableBezierSet::SetBoundingSphereCenter( double x, double y, double z ) {
	BoundingSphereCenter.Set(x, y, z);
	BoundingSphereSet = false;
	BoundingSphereManuallySet = true;
}

inline void ViewableBezierSet::SetBoundingSphereCenter( const double *center ) {
	BoundingSphereCenter.Load(center);
	BoundingSphereSet = false;
	BoundingSphereManuallySet = true;
}

inline void ViewableBezierSet::SetBoundingSphereCenter( const float *center ) {
	BoundingSphereCenter.Load(center);
	BoundingSphereSet = false;
	BoundingSphereManuallySet = true;
}

inline void ViewableBezierSet::SetBoundingSphereCenter( const VectorR3& center )
{ 
	BoundingSphereCenter = center; 
	BoundingSphereSet = false;
	BoundingSphereManuallySet = true;
}

inline void ViewableBezierSet::UnsetBoundingSphereCenter() {
	BoundingSphereManuallySet = false;
	BoundingSphereSet = false;
}

inline void ViewableBezierSet::GetBoundingSphereCenter( double* center ) {
	BoundingSphereCenter.Dump(center);
}

inline void ViewableBezierSet::GetBoundingSphereCenter( float* center ) {
	BoundingSphereCenter.Dump(center);
}

// ***********************************************************************************
// * BezierPatch class - Member functions											 *
// ***********************************************************************************

inline void BezierPatch::SetUvRange( const VectorR2& uvMin, const VectorR2 uvMax )
{
	UvMin = uvMin;
	UvMax = uvMax;
}

inline bool BezierPatch::MgrNeedToRecurse () {
	return ( MgrRecurseLevel<BezierPatchMgr::MinIsectRecurse
				&& MaxDotC-MinDotC > 1.0e-10 ) ;
}

// ***********************************************************************************
// * BezierPatch class - Member functions											 *
// ***********************************************************************************

inline void BezierPatchMgr::GetTwoSubPatchs ( BezierPatch& bpIn, 
											 BezierPatch** bpOut1,BezierPatch** bpOut2 ) 
{
	if ( bpIn.SplitPatchA==0 ) {
		*bpOut1 = new BezierPatch();
		*bpOut2 = new BezierPatch();
		int i = bpIn.MgrRecurseLevel + 1;
		(*bpOut1)->MgrRecurseLevel = i;
		(*bpOut2)->MgrRecurseLevel = i;
		VectorR4 temp;
		double sizeA, sizeB;
		temp = bpIn.CntlPts[3][0];
		temp -= bpIn.CntlPts[0][0];
		sizeA = temp.NormSq();
		temp = bpIn.CntlPts[3][3];
		temp -= bpIn.CntlPts[0][3];
		sizeA += temp.NormSq();
		temp = bpIn.CntlPts[0][3];
		temp -= bpIn.CntlPts[0][0];
		sizeB = temp.NormSq();
		temp = bpIn.CntlPts[3][3];
		temp -= bpIn.CntlPts[3][0];
		sizeB += temp.NormSq();
		if ( sizeA >= sizeB ) {		
			bpIn.MakeSplitU(*bpOut1,*bpOut2);		// Split in U direction
		}
		else {
			bpIn.MakeSplitV(*bpOut1,*bpOut2);		// Split in V direction
		}
		if ( bpIn.MgrRecurseLevel<MaxRecurseSave ) {
			// Save for next time
			bpIn.SplitPatchA = *bpOut1;
			bpIn.SplitPatchB = *bpOut2;
		}
	}
	else {
		*bpOut1 = bpIn.SplitPatchA;
		*bpOut2 = bpIn.SplitPatchB;
	}
}

inline void BezierPatchMgr::ReleaseBezierPatch ( BezierPatch* bp ) {
	if ( bp->MgrRecurseLevel>MaxRecurseSave ) {
		delete bp;
	}
}

inline bool BezierPatch::IsMgrAutoAllocated() {
	return ( MgrRecurseLevel!=0 );
}

#endif // VIEWABLEBEZIERPATCH_H
