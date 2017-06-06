/*
 *
 * RayTrace Software Package, release 3.1.2.  February 12, 2007.
 *
 * Mathematics Subpackage (VrMath)
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

// KdTree.h
//
//   A general purpose KdTree which
//		a) holds 3D objects of generic type
//		b) supports ray tracing

// Author: Sam Buss based on work by Sam Buss and Alex Kulungowski
// Contact: sbuss@math.ucsd.edu


#ifndef KDTREE_H
#define KDTREE_H

#include "../DataStructs/ShellSort.h"
#include "../DataStructs/Array.h"
#include "../DataStructs/Stack.h"
#include "../VrMath/Aabb.h"
class RayTraceStats;	// Statistics for KdTree traversal

class KdTree;			// kd-tree.
class KdTreeNode;		// A single node in the kd-tree.

class Kd_TraverseNodeData;			// Holds information on a single node needing traversal.

// Next classes used only for creating tree
class ExtentTriple;				// A extent triples: a single max, min, or flat value
class ExtentTripleArrayInfo;	// Information about array of extent triples.

// Three callback routines that aid in tree building:
//     ExtentFunction returns bounding box (an AABB) enclosing the object.
typedef void ExtentFunction( long objectNum, AABB& boundingBox );
//     ExtentInBoxFunction returns a AABB that encloses the intersection of the object and the clippingBox.
//	   Returns the "boundingBox" and returns true if the box is non-empty.
typedef bool ExtentInBoxFunction( long objectNum, const AABB& clippingBox, AABB& boundingBox );
//     Returns the cost of testing an intersection against an object.
typedef double ObjectCostFunction( long objectNum );

// Callback routines that are used to return information while traversing the tree.
//    Gives an object in a leaf node.  
//	  Return code is "true" if the returned stop distance is relevant
typedef bool PotentialObjectCallback( long objectNum, double* retStopDistance );
//    Returns the list of objects in a leaf node.
//	  The list is sorted by objectNums.  
//	  The list is static, so the pointer may be saved for later use.
//	  Return code is "true" if the returned stop distance is relevant
typedef bool PotentialObjectsListCallback( int numberOfObjects, long* objectNums, double* retStopDistance );

enum KD_SplittingAxis {
	KD_SPLIT_X = 0,
	KD_SPLIT_Y = 1,
	KD_SPLIT_Z = 2,
	KD_LEAF = 3
};

// ************************************************************************************
// KdTree																			  *
// ************************************************************************************
class KdTree
{
public:
	// This constructor is used with "BuildTree"
	KdTree();

	// This constructor builds directly the entire KdTree.
	KdTree( long numObjects, ExtentFunction* extentFunc, ExtentInBoxFunction* extentInBoxFunc );

	// Destructor
	~KdTree();

	// ****** Tree traversal routines ******

	// Traverse: Simple traversal scheme.
	//	 startPos - beginning of the ray.
	//	 dir - direction of the ray.
	//   Returns "true" if traversal aborted by the callback function returning "true"
	bool Traverse( const VectorR3& startPos, const VectorR3& dir, 
					PotentialObjectCallback* pocFunc, double seekDistance = 0.0, bool useSeekDistance = false );
	bool Traverse( const VectorR3& startPos, const VectorR3& dir, 
					PotentialObjectsListCallback* polcFunc, double seekDistance = 0.0, bool useSeekDistance = false );

	// ******** Accessors ****************
	const KdTreeNode& GetNode( long i ) const;

	void ResetStats();
	void Stats_ObjectsInLeaves( long objNum = 1 );
	void Stats_NodeTraversed();
	void Stats_LeafTraversed();
	void Stats_GetAll( long* numNodes, long* numNonEmptyLeaves, long* numObjsInLeaves ) const;

private:
	bool Traverse( const VectorR3& startPos, const VectorR3& dir, double seekDistance = 0.0, bool useSeekDistance = false );

public:
	// ****** Tree building routines *******

	// Set the assumed cost for intersecting a single object.
	// By default this value is 4, and is the same for all objects.
	// That is, the cost of processing an object in a leaf node is
	//		four times the cost of traversing a internal node.
	// To change this call one of the following.  
	//	The first form sets the value to a new constant.
	//	The second form gives a callback function to get the relative costs
	void SetObjectCost ( double cost );		// Defaults 4.0.
	void SetObjectCost ( ObjectCostFunction costFunction );

	// Set the cost function for the splitting decision
	// By default, uses a MacDonald-Booth method based on Goldsmith-Salmon surface area values.
	// But this can be modified by the following functions
	void SetMacdonaldBoothSplitting( bool useModifiedCoefs = false );	
	void SetDoubleRecurseSplitting( bool useModifiedCoefs = false );

	// Set the stopping criterion
	//   numAccesses means the benefit required to justify adding a new tree node.
	//		The "benefit" is measured in terms of time savings, the time saved is
	//		is measured in units of time required to traverse a single tree node.
	//		(Same units are used for "SetObjectCost".
	//	numRays is the expected total number of rays to traced. 
	//	All that really matters is the ratio  numAccesses/numRays which equals the
	//		expected time savings per average ray.
	//  Default values are 1,000,000 and 4.0.
	void SetStoppingCriterion( long numRays, double numAccesses );

	// Can call BuildTree at most once.
	void BuildTree( long numObject, ExtentFunction* extentFunc, ExtentInBoxFunction* extentInBoxFunc );

	const static int ExtentTripleStorageMultiplier  = 4;	// m/(1-m) where m is the overlapping fraction expected

	long NumObjects;	// Number of objects stored in the tree (not counting duplications)
	double TotalObjectCosts;	// Total cost of all objects in the tree

private:
	long TreeSize() const { return TreeNodes.SizeUsed(); }		// Number of nodes in the tree

	Array<KdTreeNode> TreeNodes;
	long RootIndex() const { return 0; }	// Index for the first entry in the array.
	long NextIndex();		// Preallocate the next entry ahead of time.

	AABB BoundingBox;			// An AABB that encloses the entire tree

	// Traversal helper data
	bool UseListCallback;		// True for the "List" callback traversal
	void* CallbackFunction;		// Either PotentialObjectCallback* or PotentialObjectsListCallback*

	// Traversal statistics
	long Stats_NumberKdNodesTraversed;
	long Stats_NumberKdLeavesTraversed;
	long Stats_NumberKdObjectsInLeaves;


	// Following items are used only while building the tree.
	enum SplitAlgorithmType {
		MacDonaldBooth = 0,					// MacDonald-Booth method
		MacDonaldBoothModifiedCoefs = 1,	// MacDonald-Booth method
		DoubleRecurseGS = 2,				// Double recursion with Goldsmith-Salmon coefficients
		DoubleRecurseModifiedCoefs=3		// Double recursion with modified coefficients.
	};

	SplitAlgorithmType SplitAlgorithm;	// Which split cost function to use.

	double StoppingCostPerRay;				// Improved cost/ray needed to justify adding tree node

	bool UseConstantCost;
	double ObjectConstantCost;
	static double DefaultObjectCost() { return 2.0; }
	ObjectCostFunction* UserCostFunction;

	// Temporary data used only while building the kd-tree.
	ExtentFunction* ExtentFunc;	// Function for calculating the extents. 
	ExtentInBoxFunction* ExtentInBoxFunc;	// For extents within a box
	AABB* ObjectAABBs;			// Holds extents (and extents in boxes) for each object.
	ExtentTriple* ET_Lists;		// Tons of storage for extent lists.
	double BoundingBoxSurfaceArea;	// Surface area of the tree's bounding box
	unsigned char* LeftRightStatus;		// Info on whether objects go left or right in split.

	// Routines used for building the tree
	void BuildSubTree( long baseIndex, AABB& aabb, double totalObjectCost,
					ExtentTripleArrayInfo& xExtents, ExtentTripleArrayInfo& yExtents, 
					ExtentTripleArrayInfo& zExtents, long spaceAvailable );
	void CalcBestSplit( const AABB& aabb, const VectorR3& deltaAABB, double totalObjectCost, 
					const ExtentTripleArrayInfo& xExtents, const ExtentTripleArrayInfo& yExtents, 
					const ExtentTripleArrayInfo& zExtents,
					KD_SplittingAxis* splitAxisID, double* splitValue, 
					long* numTriplesToLeft, long* numObjectsToLeft, long* numObjectsToRight, 
					double* costObjectsToLeft, double* costObjectsToRight );
	bool CalcBestSplit( double totalObjectCost, double costToBeat, 
						const ExtentTripleArrayInfo& extents, 
						double minOnAxis, double maxOnAxis, 
						double secondAxisLen, double thirdAxisLen,
						double* newBestCost, double* splitValue, 
						long* numTriplesToLeft, long* numObjectsToLeft, long* numObjectsToRight,
						double* costObjectsToLeft, double* costObjectsToRight );
	void MakeAabbsForSubtree( unsigned char leftRightFlag, const ExtentTripleArrayInfo& theExtents,
								const AABB& theAabb );
	void CopyTriplesForSubtree( unsigned char leftRightFlag, int axisNumber,
										ExtentTripleArrayInfo& fromExtents, 
										ExtentTripleArrayInfo& toExtents ); 
	void UpdateLeftRightCosts( const ExtentTriple& et, long* numObjectsLeft, long* numObjectsRight, 
							   double *costLeft, double *costRight );
	double CalcTotalCosts( const ExtentTripleArrayInfo& extents ) const;

	// Routines and data used for split-cost-functions.  
	// Only needed while building a kd-Tree.
	//  CF = cost function.  
	double CF_MinOnAxis;			// Starting value for first axis
	double CF_MaxOnAxis;			// Ending value for first axis
	double CF_FirstAxisLenInv;		// One divided by length of first axis
	double CF_OldCost;				// Cost to beat
	double CF_TotalNodeObjectCosts;	// Total cost of all objects in current node
	double CF_LogTNOCinv;			// 1.0 over log(CF_TotalNodeObjectCosts)
	double CF_Area;					// Area of the node
	double CF_EndArea;				// Surface area of one end (side face) of the node combined.
	double CF_Wrap;					// Surface area of "wrap" portion of the node.
	double CF_C, CF_D;				// C and D coefs for the Buss double recurse method
	double CF_ExponentToBeat;		// Exponent to beat for double recurse method
	void InitSplitCostFunction( double minOnAxis, double maxOnAxis, double secondAxisLen, double thirdAxisLen,
								double costToBeat, double totalObjectCosts );
	void InitMacdonaldBooth( double minOnAxis, double maxOnAxis, double secondAxisLen, double thirdAxisLen,
								double costToBeat, double totalObjectCosts );
	void InitDoubleRecurse( double minOnAxis, double maxOnAxis, double secondAxisLen, double thirdAxisLen,
								double costToBeat, double totalObjectCosts );
	bool CalcSplitCost( double splitValue, double costLeft, double costRight, double* retCost );
	bool CalcMacdonaldBooth( double splitValue, double costLeft, double costRight, double* retCost );
	bool CalcMacdonaldBoothModifiedCoefs( double splitValue, double costLeft, double costRight, double* retCost );
	bool CalcDoubleRecurseGS( double splitValue, double costLeft, double costRight, double* retCost );
	bool CalcDoubleRecurseModifiedCoefs( double splitValue, double costLeft, double costRight, double* retCost );

	void MemoryError();					// If allocation of memory fails.
	void MemoryError2();				// If storage multiplier did not give enough memory
};

// See the end of this file for the inlined members of KdTree.


// ************************************************************************************
// KdTreeNode																		  *
// ************************************************************************************

class KdTreeNode {
public:
	friend class KdTree;

	bool IsLeaf() const { return (NodeType==KD_LEAF); }
	int SplitAxis() const { assert(NodeType!=KD_LEAF); return (int)NodeType; }
	bool IsRoot() const { return (ParentIdx == -1); }
	bool LeftChildEmpty() const { assert(NodeType!=KD_LEAF); return (Data.Split.LeftChildIdx == -1); }
	bool RightChildEmpty() const { assert(NodeType!=KD_LEAF); return (Data.Split.RightChildIdx == -1); }
	long GetNumObjects() const { assert (NodeType==KD_LEAF); return Data.Leaf.NumObjects; }

	double SplitValue() const { return Data.Split.SplitValue; }

	// Returns pointer to the left child, or Null if the left child is an empty leaf.
	long LeftChildIndex() const { return Data.Split.LeftChildIdx; }

	// Returns pointer to the right child, or Null if the left child is an empty leaf.
	long RightChildIndex() const { return Data.Split.RightChildIdx; }

	// Returns point to the parent, or Null if is the root.
	long ParentIndex() const { return ParentIdx; }

private:
	KD_SplittingAxis NodeType;			// The type of node

	long ParentIdx;				// Equals -1 if this is root and there is no parent

	struct InternalNodeValues {
		long LeftChildIdx;			// Equals -1 if left child is empty
		long RightChildIdx;			// Equals -1 if right child is empty
		double SplitValue;			// Value for the split plane
	};

	struct LeafNodeValues {
		long* ObjectList;			// Pointer to indices objects stored at the leaf
		long NumObjects;			// Number of objects in the leaf node
	};

	union {
		InternalNodeValues Split;	// The values for an internal node
		LeafNodeValues Leaf;		// The values for a leaf
	} Data;

};

// *******************************************************************
// Kd_TraverseNodeData												 *
//		Holds information on a node needing traversal				 *
// *******************************************************************

class Kd_TraverseNodeData {
	friend class KdTree;

public:
	Kd_TraverseNodeData() {}
	Kd_TraverseNodeData( long nodeNum, double minDist, double maxDist );
	void Set( long nodeNum, double minDist, double maxDist  );

	long GetNodeNumber() const { return NodeNumber; }
	double GetMinDist() const { return MinDistance; }
	double GetMaxDist() const { return MaxDistance; }

private:
	long NodeNumber;			// Index of the node
	double MinDistance;			// Minimum distance along ray to search (entry distance)
	double MaxDistance;			// Maximum distance along ray to search (exit distance)
};

inline Kd_TraverseNodeData::Kd_TraverseNodeData( long nodeNum, double minDist, double maxDist )
{
	Set ( nodeNum, minDist, maxDist );
}

inline void Kd_TraverseNodeData::Set( long nodeNum, double minDist, double maxDist  )
{
	NodeNumber = nodeNum;
	MinDistance = minDist;
	MaxDistance = maxDist;
}


//*********************************************************************
// ExtentTriple														  *
//   This data structure is used to help build the kd-tree.			  *
//	 And only for that purpose.										  *
//	 Triple holds information about a min or max extent of an object  *
//		inside a kd-tree node.	When the min and max values are equal,*
//		they are combined into a single triple, and called a "flat".  *
//*********************************************************************

class ExtentTriple {
	friend class KdTree;

public:
	enum TripleType {
		TT_MAX = 0,
		TT_FLAT = 1,
		TT_MIN = 2,
	};

	void Set( TripleType theType, double value, long objectID );
	void SetMin( double value, long objectID );
	void SetMax( double value, long objectID );
	void SetFlat( double value, long objectID );

	bool IsMin() const { return ExtentType==TT_MIN; }
	bool IsMax() const { return ExtentType==TT_MAX; }
	bool IsFlat() const { return ExtentType==TT_FLAT; }

	friend bool operator<(const ExtentTriple& x, const ExtentTriple y );
	ExtentTriple operator=( const ExtentTriple& );

private:
	TripleType ExtentType;		// Type of extent value
	double ExtentValue;			// The extent value
	long ObjectID;				// The integer number of the object

};

//*********************************************************************
// ExtentTripleArrayInfo											  *			  *
//   This data structure is used to help build the kd-tree.			  *
//	 And only for that purpose.										  *
//	 Holds a pointer to an array of ExtentTriples, along with info	  *
//		about the number of max's, min's and flat's.				  *
//   Each object that appears in the extent triple array			  *
//		has either exactly one each max and min record,				  *
//		or exactly one flat.										  *
//*********************************************************************
class ExtentTripleArrayInfo {
	friend class KdTree;

public:
	ExtentTripleArrayInfo() { Init( 0, 0, 0 ); }
	ExtentTripleArrayInfo( ExtentTriple* tripleArray, long numMaxMins, long numFlats )
		{ Init( tripleArray, numMaxMins, numFlats ); }
	void Init ( ExtentTriple* tripleArray, long numMaxMins, long numFlats );

	void SetNumbers( long numMaxMins, long numFlats );
	
	// Add either a min and a max or a flat.
	long AddToEnd ( double min, double max, long objectID );

	void Sort() { ShellSort( TripleArray, NumTriples() ); }

private:
	ExtentTriple* TripleArray;		// Pointer to an array of triples
	ExtentTriple* EndOfArray;		// Pointer to the first empty spot in array

	long NumMaxMins;				// Number of max/min records
	long NumFlats;					// Number of flat records;

	// You must maintain this condition:
	// EndOfArray will *always* equal TripleArray+NumTriples().

	const long NumObjects () const { return NumMaxMins + NumFlats; }
	const long NumTriples () const { return (NumMaxMins<<1) + NumFlats; }
};

inline void ExtentTriple::Set( TripleType theType, double value, long objectID )
{
	assert ( theType>=TT_MAX && theType<=TT_MIN );
	ExtentType = theType;
	ExtentValue = value;
	ObjectID = objectID;
}

inline void ExtentTriple::SetMin( double value, long objectID )
{
	ExtentType = TT_MIN;
	ExtentValue = value;
	ObjectID = objectID;
}

inline void ExtentTriple::SetMax( double value, long objectID )
{
	ExtentType = TT_MAX;
	ExtentValue = value;
	ObjectID = objectID;
}

inline void ExtentTriple::SetFlat( double value, long objectID )
{
	ExtentType = TT_FLAT;
	ExtentValue = value;
	ObjectID = objectID;
}

inline bool operator<(const ExtentTriple& x, const ExtentTriple y )
{
	if ( x.ExtentValue < y.ExtentValue ) {
		return true;
	}
	else if ( x.ExtentValue == y.ExtentValue ) {
		if ( x.ExtentType < y.ExtentType ) {
			return true;
		}
		else if ( x.ExtentType == y.ExtentType && x.ObjectID < y.ObjectID ) {  // Tie breaker!
			return true;
		}
	}
	return false;
}

inline ExtentTriple ExtentTriple::operator=( const ExtentTriple& other )
{
	ExtentValue = other.ExtentValue;
	ExtentType = other.ExtentType;
	ObjectID = other.ObjectID;
	return *this;
}

inline void ExtentTripleArrayInfo::Init ( ExtentTriple* tripleArray, long numMaxMins, long numFlats )
{
	TripleArray = tripleArray;
	NumMaxMins = numMaxMins;
	NumFlats = numFlats;
	EndOfArray = TripleArray + NumTriples();
}

// Use this routine to set the array size information after having
//	directly set the entries.
inline void ExtentTripleArrayInfo::SetNumbers( long numMaxMins, long numFlats )
{
	NumMaxMins = numMaxMins;
	NumFlats = numFlats;
	EndOfArray = TripleArray+(NumFlats+(NumMaxMins<<1));
	return;
}

// Add one or two triples to the end of the array.
// No checking is done for overflowing the array (array size is not known)
// Return code: number of new triples added to the array
//		Return 1 if add a flat.
//		Return 2 if add a min and a max.
inline long ExtentTripleArrayInfo::AddToEnd ( double min, double max, long objectID )
{
	if ( min==max ) {
		// Add a flat
		(EndOfArray++)->SetFlat( min, objectID );
		NumFlats++;
		return 1;
	}
	else {
		// Add a min and a max
		(EndOfArray++)->SetMin( min, objectID );
		(EndOfArray++)->SetMax( max, objectID );
		NumMaxMins++;
		return 2;
	}

}

// **********************************************************
// KdTree - inlined member functions
// **********************************************************

inline KdTree::KdTree()
{
	SplitAlgorithm = MacDonaldBooth;
	SetObjectCost ( DefaultObjectCost() );
	SetStoppingCriterion( 1000000, 4.0 );

	ResetStats();
}

inline KdTree::KdTree( long numObjects, ExtentFunction* extentFunc, ExtentInBoxFunction* extentInBoxFunc )
{
	SplitAlgorithm = MacDonaldBooth;
	SetObjectCost ( DefaultObjectCost() );
	SetStoppingCriterion( 1000000, 4.0 );
	BuildTree( numObjects, extentFunc, extentInBoxFunc );

	ResetStats();
}

// Set a cost function for objects
inline void KdTree::SetObjectCost ( double cost )
{
	UseConstantCost = true;
	ObjectConstantCost = cost;
	assert (cost>0.0);
}

// Set a user-defined callback function for cost calculations
inline void KdTree::SetObjectCost ( ObjectCostFunction costFunction )
{
	UseConstantCost = false;
	UserCostFunction = costFunction;
}

inline void KdTree::SetStoppingCriterion( long numRays, double numAccesses )
{
	StoppingCostPerRay = numAccesses/(double)numRays;
}

// Set to use MacDonald Booth splitting criterion.
// Argument == true to change the coefficients
inline void KdTree::SetMacdonaldBoothSplitting( bool useModifiedCoefs )
{
	SplitAlgorithm = useModifiedCoefs ? MacDonaldBoothModifiedCoefs : MacDonaldBooth;
}

// Set to use Buss Double-Recurse splitting criterion.
// Argument == true to change the coefficients
inline void KdTree::SetDoubleRecurseSplitting( bool useModifiedCoefs )
{
	SplitAlgorithm = useModifiedCoefs ? DoubleRecurseModifiedCoefs : DoubleRecurseGS;
}

inline void KdTree::ResetStats() 
{
	// Traversal statistics
	Stats_NumberKdNodesTraversed = 0;
	Stats_NumberKdLeavesTraversed = 0;
	Stats_NumberKdObjectsInLeaves = 0;
}

inline void KdTree::Stats_ObjectsInLeaves( long objNum ) 
{
	Stats_NumberKdObjectsInLeaves += objNum;
}

inline void KdTree::Stats_NodeTraversed( ) 
{
	Stats_NumberKdNodesTraversed++;
}

inline void KdTree::Stats_LeafTraversed( ) 
{
	Stats_NumberKdLeavesTraversed++;
}

inline void KdTree::Stats_GetAll( long* numNodes, long* numNonEmptyLeaves, long* numObjsInLeaves ) const
{
	*numNodes = Stats_NumberKdNodesTraversed;
	*numNonEmptyLeaves = Stats_NumberKdLeavesTraversed;
	*numObjsInLeaves = Stats_NumberKdObjectsInLeaves;
}

inline const KdTreeNode& KdTree::GetNode( long i ) const 
{ 
	return TreeNodes[i]; 
}

// Allocate the next entry for the KdTree 
//  Call this to pre-allocate to avoid having the Array for the KdTree
//		automatically re-sized at a bad time.
inline long KdTree::NextIndex() 
{ 
	long i = TreeNodes.SizeUsed(); 
	TreeNodes.Touch(i); 
	return i; 
}



#endif // KDTREE_H