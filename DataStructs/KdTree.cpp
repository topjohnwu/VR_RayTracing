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

// KdTree.cpp
//
//   A general purpose KdTree which
//		a) holds 3D objects of generic type
//		b) supports ray tracing

// Author: Sam Buss based on work by Sam Buss and Alex Kulungowski
// Contact: sbuss@math.ucsd.edu

#include <assert.h>
#include <stdio.h>

#include "KdTree.h"
#include "DoubleRecurse.h"

Stack<Kd_TraverseNodeData> traverseStack;

// Destructor
KdTree::~KdTree()
{
	if ( TreeSize()==0 ) {
		return;
	}
	// Traverse the tree and delete object lists in each non-empty leaf node
	Stack<long> IdxStack;
	long currentNodeIndex = RootIndex();			// The current node in the traversal
	while ( true ) {
		KdTreeNode* currentNode = &TreeNodes[currentNodeIndex];
		if ( currentNode->IsLeaf() ){
			delete[] currentNode->Data.Leaf.ObjectList;
		}
		else {
			if ( !currentNode->LeftChildEmpty() ) {
				IdxStack.Push( currentNode->Data.Split.LeftChildIdx );
			}
			if ( !currentNode->RightChildEmpty() ) {
				currentNodeIndex = currentNode->Data.Split.RightChildIdx;
				continue;
			}
		}
		if ( IdxStack.IsEmpty() ) {
			break;
		}
		currentNodeIndex = IdxStack.Pop();
	}
}


/***********************************************************************************************
 * Tree traversal functions.
 ***********************************************************************************************/

// Traverse: Simple traversal scheme.
//	 startPos - beginning of the ray.
//	 dir - direction of the ray.
//   Returns "true" if traversal aborted by the callback function returning "true"
bool KdTree::Traverse( const VectorR3& startPos, const VectorR3& dir, 
				PotentialObjectCallback* pocFunc, double seekDistance, bool obeySeekDistance  )
{
	CallbackFunction = (void*) pocFunc;
	UseListCallback = false;

	return Traverse( startPos, dir, seekDistance, obeySeekDistance );
}

bool KdTree::Traverse( const VectorR3& startPos, const VectorR3& dir, 
				PotentialObjectsListCallback* polcFunc, double seekDistance, bool obeySeekDistance  )
{
	CallbackFunction = (void*) polcFunc;
	UseListCallback = true;

	return Traverse( startPos, dir, seekDistance, obeySeekDistance );
}

bool KdTree::Traverse( const VectorR3& startPos, const VectorR3& dir, double seekDistance, bool obeySeekDistance )
{
	double entryDist, exitDist;
	int entryFaceId, exitFaceId;

	// Set sign of dir components and inverse values of non-zero entries.
	VectorR3 dirInv;
	int signDirX = Sign(dir.x);
	if ( signDirX!=0 ) {
		dirInv.x = 1.0/dir.x;
	}
	int signDirY = Sign(dir.y);
	if ( signDirY!=0 ) {
		dirInv.y = 1.0/dir.y;
	}
	int signDirZ = Sign(dir.z);
	if ( signDirZ!=0 ) {
		dirInv.z = 1.0/dir.z;
	}

	bool intersectsAABB = BoundingBox.RayEntryExit( startPos, 
													signDirX, signDirY, signDirZ, dirInv, 
													&entryDist, &entryFaceId, 
													&exitDist, &exitFaceId );
	if ( !intersectsAABB || exitDist<0.0 ) {
		return false;
	}

	// Main traversal loop

	long currentNodeIndex = RootIndex();			// The current node in the traversal
	assert ( currentNodeIndex != -1 ) ;				// The tree should not be empty
    KdTreeNode* currentNode = &TreeNodes[currentNodeIndex];
	double minDistance = Max(0.0, entryDist);					
	double maxDistance = exitDist;
	bool hitParallel = false;
	double parallelHitMax = -DBL_MAX;
	bool stopDistanceActive = obeySeekDistance;
	double stopDistance = seekDistance;
	if ( obeySeekDistance && maxDistance>seekDistance ) {
		if ( seekDistance<minDistance ) {
			return false;
		}
		maxDistance = seekDistance;
	}
	assert ( minDistance<=maxDistance );
	traverseStack.Reset();

	while ( true ) {
		
		if ( ! currentNode->IsLeaf() ) {
			Stats_NodeTraversed();
			// Handle non-leaf nodes
			//		These do not contain primitive objects.
			int thisSign;
			double thisDirInv;
			double thisStartPt;
			switch ( currentNode->NodeType ) 
			{
			case KD_SPLIT_X:
				thisSign = signDirX;
				thisDirInv = dirInv.x;
				thisStartPt = startPos.x;
				break;
			case KD_SPLIT_Y:
				thisSign = signDirY;
				thisDirInv = dirInv.y;
				thisStartPt = startPos.y;
				break;
			case KD_SPLIT_Z:
				thisSign = signDirZ;
				thisDirInv = dirInv.z;
				thisStartPt = startPos.z;
				break;
			case KD_LEAF:
				assert (0);		// Included to make gcc compiler happy.
			}
			long nearNodeIdx;
			long farNodeIdx;
			if ( thisSign==0 ) {
				// Handle hitting exactly parallel to the splitting plane
				double thisSplitVal = currentNode->SplitValue();
				if ( thisSplitVal<thisStartPt ) {
					currentNodeIndex = currentNode->RightChildIndex();
				}
				else if ( thisSplitVal>thisStartPt ) {
					currentNodeIndex = currentNode->LeftChildIndex();
				}
				else {
					// Exactly hit the splitting plane (not so good!)
					int leftIdx = currentNode->LeftChildIndex();
					int rightIdx = currentNode->RightChildIndex();
					if ( leftIdx == -1 ) {
						currentNodeIndex = rightIdx;
					}
					else if ( rightIdx == -1 ) {
						currentNodeIndex = leftIdx;
					}
					else {
						traverseStack.Push();
						traverseStack.Top().Set( rightIdx, minDistance, maxDistance );
						currentNodeIndex = leftIdx;
						hitParallel = true;
						UpdateMax(maxDistance,parallelHitMax);
					}
				}
			}
			else {
				if ( thisSign>0 ) {
					nearNodeIdx = currentNode->LeftChildIndex();
					farNodeIdx = currentNode->RightChildIndex();
				}
				else {
					nearNodeIdx = currentNode->RightChildIndex();
					farNodeIdx = currentNode->LeftChildIndex();
				}
				double splitDistance = (currentNode->SplitValue()-thisStartPt)*thisDirInv;
				if ( splitDistance<minDistance ) {
					// Far node is the new current node
					currentNodeIndex = farNodeIdx;
				}
				else if ( splitDistance>maxDistance ) {
					// Near node is the new current node
					currentNodeIndex = nearNodeIdx;
				}
				else if ( nearNodeIdx == -1 ) {
						minDistance = splitDistance;
						currentNodeIndex = farNodeIdx;
				}
				else {
					// Push the far node -- if it exists
					if ( farNodeIdx != -1 ) {
						traverseStack.Push();
						traverseStack.Top().Set( farNodeIdx, splitDistance, maxDistance );
					}
					// Near node is the new current node
					maxDistance = splitDistance;
					currentNodeIndex = nearNodeIdx;
				}
			}
			if ( currentNodeIndex != -1 ) {
				currentNode = &TreeNodes[currentNodeIndex];
				continue;
			}
			// If we reach here, we are at an empty leaf and can fall through.

		}

		else {
			// Handle leaf nodes by invoking the callback function
			Stats_LeafTraversed();
			if ( UseListCallback ) {
				// Pass whole list of objects back to the user
				bool stopFlag;
				double newStopDist;
				Stats_ObjectsInLeaves( currentNode->Data.Leaf.NumObjects );
				stopFlag = (*((PotentialObjectsListCallback*)CallbackFunction))( 
										currentNode->Data.Leaf.NumObjects, 
										currentNode->Data.Leaf.ObjectList, 
										&newStopDist );
				if ( stopFlag ) {
					stopDistanceActive = true;
					stopDistance = newStopDist;
				}
			}
			else {
				// Pass the objects back to the user one at a time
				double newStopDist;
				int i = currentNode->Data.Leaf.NumObjects;
				Stats_ObjectsInLeaves( i );
				long* objectIdPtr = currentNode->Data.Leaf.ObjectList;
				for ( ; i>0; i-- ) {
					if ( (*((PotentialObjectCallback*)CallbackFunction))( 
												*objectIdPtr, &newStopDist )  )  
					{
						stopDistanceActive = true;
						stopDistance = newStopDist;
					}
					objectIdPtr++;
				}
			}
		}

		// Get to this point if done with a leaf node (possibly empty, possibly not).
		if ( traverseStack.IsEmpty() ) {
			return stopDistanceActive;
		}
		else {
			Kd_TraverseNodeData& topNode = traverseStack.Pop();
			minDistance = topNode.GetMinDist();
			if ( stopDistanceActive && minDistance>stopDistance ) {
				if ( !hitParallel || minDistance>=parallelHitMax ) {
					// Exit loop.  Fully done.
					return true;
				}
			}
			currentNodeIndex = topNode.GetNodeNumber();
			currentNode = &TreeNodes[currentNodeIndex];
			maxDistance = topNode.GetMaxDist(); 
		}

	}

}


/***********************************************************************************************
 * Tree building functions.
 ***********************************************************************************************/
void KdTree::BuildTree(long numObjects, ExtentFunction* extentFunc, ExtentInBoxFunction* extentInBoxFunc )
{
	assert (TreeSize() == 0);
	NumObjects = numObjects;
	ExtentFunc = extentFunc;
	ExtentInBoxFunc = extentInBoxFunc;

	// Get total cost of all objects
	if ( UseConstantCost ) {
		TotalObjectCosts = ObjectConstantCost*NumObjects;
	}
	else {
		TotalObjectCosts = 0;
		for ( long i=0; i<NumObjects; i++ ) {
			TotalObjectCosts += (*UserCostFunction)(  i );
		}
	}

	// Allocate space for the AABB's for each object
	//	This is used only during the tree construction and is then released.
	ObjectAABBs = new AABB[numObjects];

	// Calculate all initial extents
	long i;
	AABB* ObjectAabbPtr = ObjectAABBs;
	for (i=0; i<numObjects; i++ ) {
		(*ExtentFunc)( i, *ObjectAabbPtr );
		ObjectAabbPtr++;
	}

	// Pick the overall BoundingBox to enclose all the individual bounding boxes.
	BoundingBox = *ObjectAABBs;
	ObjectAabbPtr = ObjectAABBs+1;
	for (i=1; i<numObjects; i++ ) {
		BoundingBox.EnlargeToEnclose( *ObjectAabbPtr );
		ObjectAabbPtr++;
	}
	BoundingBoxSurfaceArea = BoundingBox.SurfaceArea();

	// Set up the initial extent lists
	ET_Lists = new ExtentTriple[ (3*2*ExtentTripleStorageMultiplier)*NumObjects ];
	if ( !ET_Lists ) {
		MemoryError();
	}
	ExtentTripleArrayInfo XextentList( ET_Lists, 0, 0 );
	ExtentTripleArrayInfo YextentList( ET_Lists + (2*ExtentTripleStorageMultiplier)*NumObjects, 0, 0 );
	ExtentTripleArrayInfo ZextentList( ET_Lists + (2*2*ExtentTripleStorageMultiplier)*NumObjects, 0, 0 );
	LeftRightStatus = new unsigned char[NumObjects];

	// Loop over all objects, creating the extent triples.
	ObjectAabbPtr = ObjectAABBs;
	for ( i=0; i<numObjects; i++ ) {
		XextentList.AddToEnd( ObjectAabbPtr->GetMinX(), ObjectAabbPtr->GetMaxX(), i );
		YextentList.AddToEnd( ObjectAabbPtr->GetMinY(), ObjectAabbPtr->GetMaxY(), i );
		ZextentList.AddToEnd( ObjectAabbPtr->GetMinZ(), ObjectAabbPtr->GetMaxZ(), i );
		ObjectAabbPtr++;
	}

	// Estimate upper bound on the space available.  
	// Need this for memory management of ExtentTriple lists
	long spaceAvailable = 2*(ExtentTripleStorageMultiplier-1)*NumObjects;

	// Sort the triples
	XextentList.Sort();
	YextentList.Sort();
	ZextentList.Sort();
		
	// Recursively build the entire tree!
	KdTreeNode& RootNode = TreeNodes[RootIndex()];
	RootNode.ParentIdx = -1;				// No parent, it is the root node
	BuildSubTree ( RootIndex(), BoundingBox, TotalObjectCosts, XextentList, YextentList, ZextentList, spaceAvailable );

	delete[] ObjectAABBs;
	delete[] ET_Lists;
	delete[] LeftRightStatus;
}

// Recursively build a subtree.
// Pick a splitting point on one of the three axes
// Then call the routine recursively twice, once for each child
//		as appropriate
// spaceAvailable gives the amount of room for growth of the ExtentTripleLists.
void KdTree::BuildSubTree( long baseIndex, AABB& aabb, double totalObjectCost,
					ExtentTripleArrayInfo& xExtents, ExtentTripleArrayInfo& yExtents, 
					ExtentTripleArrayInfo& zExtents, long spaceAvailable )
{

	VectorR3 deltaAABB = aabb.GetBoxMax();
	deltaAABB -= aabb.GetBoxMin();

	// Step 1.
	// Try all three axes to find the best split decision
	KD_SplittingAxis splitAxisID;	// 0-2 for axis x,y,z OR 3 for no split
	ExtentTripleArrayInfo* splitExtentList;	// Will point to the split axis extext list
	double splitValue;				// Point where the split occurs
	long numTriplesToLeft;			// Number of triples on left side of split
	long numObjectsToLeft;			// Number of objects on left side of split
	long numObjectsToRight;			// Number of objects on right side of split
	double costObjectsToLeft;		// Total cost of objects on the left side of split
	double costObjectsToRight;		// Total cost of objects on the right side of split
	CalcBestSplit( aabb, deltaAABB, totalObjectCost, xExtents, yExtents, zExtents,
					&splitAxisID, &splitValue, 
					&numTriplesToLeft, &numObjectsToLeft, &numObjectsToRight, 
					&costObjectsToLeft, &costObjectsToRight );
	switch ( splitAxisID ) {
		case KD_LEAF:
			{
				// No splitting occurs
				// Copy object triples into an array
				KdTreeNode& baseNode = TreeNodes.GetEntry(baseIndex);
				baseNode.NodeType = KD_LEAF;
				long numInLeaf = xExtents.NumObjects();
				assert ( yExtents.NumObjects() == numInLeaf && zExtents.NumObjects() == numInLeaf );
				baseNode.Data.Leaf.NumObjects = numInLeaf;
				long* objectArray = new long[numInLeaf];	
				if ( !objectArray ) {
					MemoryError();
				}
				baseNode.Data.Leaf.ObjectList = objectArray;
				ExtentTriple* triple = xExtents.TripleArray;		// Pick any one of the three axes
				for ( long i=0; i<numInLeaf; triple++ ) {
					if ( !( triple->IsMax() ) ) {
						*(objectArray++) = triple->ObjectID;
						i++;
					}
				}
			}
			return;									// Finished: leaf is completely set
		case KD_SPLIT_X:
			splitExtentList = &xExtents;
			break;
		case KD_SPLIT_Y:
			splitExtentList = &yExtents;
			break;
		case KD_SPLIT_Z:
			splitExtentList = &zExtents;
			break;
	}

	// Step 2. Handle splits where one subtree is empty
	if ( numObjectsToLeft==0 || numObjectsToRight==0 ) {
		assert ( numObjectsToLeft!=0 || numObjectsToRight!=0 );
		// One child is empty
		long childIndex = NextIndex();		// WARNING: NextIndex() can trigger memory movement
		KdTreeNode& baseNode = TreeNodes.GetEntry(baseIndex);
		KdTreeNode& childNode = TreeNodes[childIndex];
		childNode.ParentIdx = baseIndex;
		baseNode.NodeType = splitAxisID;
		baseNode.Data.Split.SplitValue = splitValue;
		AABB childAabb(aabb);
		if ( numTriplesToLeft==0 ) {
			baseNode.Data.Split.LeftChildIdx = -1;
			baseNode.Data.Split.RightChildIdx = childIndex;
			childAabb.SetNewAxisMin( splitAxisID, splitValue );
		}
		else {
			baseNode.Data.Split.LeftChildIdx = childIndex;
			baseNode.Data.Split.RightChildIdx = -1;
			childAabb.SetNewAxisMax( splitAxisID, splitValue );
		}
		BuildSubTree( childIndex, childAabb, totalObjectCost,
						xExtents, yExtents, zExtents, spaceAvailable );
		return;
	}
	
	// Step 3. 
	// Two subtrees must be formed.
	// Decide which objects go left and right - Store info in LeftRightStatus[]
	ExtentTriple* etPtr = splitExtentList->TripleArray;
	long i;
	for ( i=0; i<numTriplesToLeft; i++, etPtr++ ) {
		// It is on the left, don't know if on right yet, so set as not.
		LeftRightStatus[ etPtr->ObjectID ] = 1;			// Set first bit, reset second bit
	}
	long n = splitExtentList->NumTriples();
	for ( ; i<n; i++, etPtr++ ) {
		if ( etPtr->ExtentType == ExtentTriple::TT_MAX ) {
			// On right side.  Maybe on left side too.
			LeftRightStatus[ etPtr->ObjectID ] |= 2;		// Set second bit
		}
		else {
			// On right side only
			LeftRightStatus[ etPtr->ObjectID ] = 2;			// Set second bit, reset first bit
		}
	}

	// Step 4.  Build the child AABB's
	AABB leftChildAabb = aabb;
	leftChildAabb.SetNewAxisMax( splitAxisID, splitValue );
	AABB rightChildAabb = aabb;
	rightChildAabb.SetNewAxisMin( splitAxisID, splitValue );
	AABB* smallerChildAabb;
	AABB* largerChildAabb;

	// Step 5.
	// Allocate the left and right children
	// Set entries in baseNode for internal node
	// Set all other tree pointers. (Indices)
	long leftChildIndex = NextIndex();		// Warning: NextIndex() can trigger memory movement
	long rightChildIndex = NextIndex();
	KdTreeNode& baseNode = TreeNodes.GetEntry(baseIndex);
	KdTreeNode& leftChildNode = TreeNodes[leftChildIndex];
	KdTreeNode& rightChildNode = TreeNodes[rightChildIndex];
	baseNode.NodeType = splitAxisID;
	baseNode.Data.Split.LeftChildIdx = leftChildIndex;
	baseNode.Data.Split.RightChildIdx = rightChildIndex;
	baseNode.Data.Split.SplitValue = splitValue;
	leftChildNode.ParentIdx = baseIndex;
	rightChildNode.ParentIdx = baseIndex;
	long smallerChildIdx;
	long largerChildIdx;

	// Step 6: Build the extent lists for the two subtrees.
	unsigned char leftRightFlag;
	long newSpaceAvailable;		// Space available for extent lists for smaller subtree building
	double smallerTotalCost;		// total cost of objects in the subtree with fewer objects
	double largerTotalCost;		// total cost of objects in the other subtree
	long smallerNumObjects;
	long largerNumObjects;
	// First build the extent for the smaller of the two subtrees
	if (  numObjectsToLeft<numObjectsToRight ) {
		leftRightFlag = 1;
		newSpaceAvailable = spaceAvailable-2*numObjectsToLeft;
		smallerChildAabb = &leftChildAabb;
		largerChildAabb = &rightChildAabb;
		smallerChildIdx = leftChildIndex;
		largerChildIdx = rightChildIndex;
		smallerTotalCost = costObjectsToLeft;
		largerTotalCost = costObjectsToRight;
		smallerNumObjects = numObjectsToLeft;
		largerNumObjects = numObjectsToRight;
	}
	else {
		leftRightFlag = 2;
		newSpaceAvailable = spaceAvailable-2*numObjectsToRight;
		smallerChildAabb = &rightChildAabb;
		largerChildAabb = &leftChildAabb;
		smallerChildIdx = rightChildIndex;
		largerChildIdx = leftChildIndex;
		smallerTotalCost = costObjectsToRight;
		largerTotalCost = costObjectsToLeft;
		smallerNumObjects = numObjectsToRight;
		largerNumObjects = numObjectsToLeft;
	}
	if ( newSpaceAvailable<0 ) {
		MemoryError2();
	}
	// Step 7.  Allocate space for the smaller subtree
	ExtentTripleArrayInfo newXextents( xExtents.EndOfArray, 0, 0 );
	ExtentTripleArrayInfo newYextents( yExtents.EndOfArray, 0, 0 );
	ExtentTripleArrayInfo newZextents( zExtents.EndOfArray, 0, 0 );
	// Create the AABB's for the smaller subtree
	MakeAabbsForSubtree( leftRightFlag, xExtents, *smallerChildAabb );
	// Copy the extent triples for the smaller subtree
	CopyTriplesForSubtree( leftRightFlag, 0, xExtents, newXextents ); 
	CopyTriplesForSubtree( leftRightFlag, 1, yExtents, newYextents ); 
	CopyTriplesForSubtree( leftRightFlag, 2, zExtents, newZextents ); 
	// Recalculate total cost if necessary, i.e., if some objects go missing
	if ( newXextents.NumObjects()!=smallerNumObjects ) {
		smallerTotalCost = CalcTotalCosts( newXextents );
	}
	
	// Step 8.
	leftRightFlag = 3-leftRightFlag;
	// Create the AABB's for the larger subtree
	MakeAabbsForSubtree( leftRightFlag, xExtents, *largerChildAabb );
	// Copy the extent triples for the larger subtree
	CopyTriplesForSubtree( leftRightFlag, 0, xExtents, xExtents ); 
	CopyTriplesForSubtree( leftRightFlag, 1, yExtents, yExtents ); 
	CopyTriplesForSubtree( leftRightFlag, 2, zExtents, zExtents ); 
	leftRightFlag = 3-leftRightFlag;		// Reset to smaller subtree again
	// Recalculate total cost if necessary, i.e., if some objects go missing
	if ( xExtents.NumObjects()!=largerNumObjects ) {
		largerTotalCost = CalcTotalCosts( xExtents );
	}

	// Step 9.
	// Invoke BuildSubTree recursively for the two subtrees
	BuildSubTree(smallerChildIdx, *smallerChildAabb, smallerTotalCost,
					newXextents, newYextents, newZextents, newSpaceAvailable);
	BuildSubTree(largerChildIdx, *largerChildAabb, largerTotalCost,
					xExtents, yExtents, zExtents, spaceAvailable );

}


void KdTree::CalcBestSplit( const AABB& aabb, const VectorR3& deltaBox, double totalObjectCost, 
					const ExtentTripleArrayInfo& xExtents, const ExtentTripleArrayInfo& yExtents, 
					const ExtentTripleArrayInfo& zExtents,
					KD_SplittingAxis* splitAxisID, double* splitValue, 
					long* numTriplesToLeft, long* numObjectsToLeft, long* numObjectsToRight, 
					double* costObjectsToLeft, double* costObjectsToRight )
{
	assert( xExtents.NumObjects() == yExtents.NumObjects() );
	assert( yExtents.NumObjects() == zExtents.NumObjects() );

	*splitAxisID = KD_LEAF;			// Default is no split, unless find a better option.

	double surfaceArea = 2.0*(deltaBox.x*deltaBox.y + deltaBox.x*deltaBox.z + deltaBox.y*deltaBox.z);
	double minImprove = (BoundingBoxSurfaceArea/surfaceArea)*StoppingCostPerRay;
	double costToBeat = totalObjectCost - minImprove;
	if ( costToBeat<=1.0+1.0e-7 ) {
		return;						// There is no way to improve enough to bother.
	}

	// Try each of the three axes in turn.
	bool foundBetter = false;
	double bestCostSoFar = totalObjectCost; 
	if ( CalcBestSplit( totalObjectCost, costToBeat, xExtents, 
						aabb.GetMinX(), aabb.GetMaxX(), deltaBox.y, deltaBox.z,
						&bestCostSoFar, splitValue, 
						numTriplesToLeft, numObjectsToLeft, numObjectsToRight,
						costObjectsToLeft, costObjectsToRight ) )
	{
		foundBetter = true;
		*splitAxisID = KD_SPLIT_X;
		costToBeat = bestCostSoFar;
	}
	if ( CalcBestSplit( totalObjectCost, costToBeat, yExtents, 
						aabb.GetMinY(), aabb.GetMaxY(), deltaBox.x, deltaBox.z,
						&bestCostSoFar, splitValue, 
						numTriplesToLeft, numObjectsToLeft, numObjectsToRight,
						costObjectsToLeft, costObjectsToRight ) )
	{
		foundBetter = true;
		*splitAxisID = KD_SPLIT_Y;
		costToBeat = bestCostSoFar;
	}
	if ( CalcBestSplit( totalObjectCost, costToBeat, zExtents, 
						aabb.GetMinZ(), aabb.GetMaxZ(), deltaBox.x, deltaBox.y,
						&bestCostSoFar, splitValue, 
						numTriplesToLeft, numObjectsToLeft, numObjectsToRight,
						costObjectsToLeft, costObjectsToRight ) )
	{
		foundBetter = true;
		*splitAxisID = KD_SPLIT_Z;
	}

}

// Returns true if a new better split is found on the axis.
// The other return values will NOT be changed unless "true" is returned.
bool KdTree::CalcBestSplit( double totalObjectCosts, double costToBeat, 
						const ExtentTripleArrayInfo& extents, 
						double minOnAxis, double maxOnAxis, 
						double secondAxisLen, double thirdAxisLen,
						double* retNewBestCost, double* retSplitValue, 
						long* retNumTriplesToLeft, long* retNumObjectsToLeft, long* retNumObjectsToRight,
						double* retCostObjectsToLeft, double* retCostObjectsToRight )
{
	if ( minOnAxis>=maxOnAxis ) {
		return false;		// We do not support splitting a zero length axis.
	}

	InitSplitCostFunction( minOnAxis, maxOnAxis, secondAxisLen, thirdAxisLen,
							costToBeat, totalObjectCosts );

	bool foundBetter = false;
	double bestCost = costToBeat;			// Cost to beat
	long numTriplesLeft = 0;				// number of triples processed so far
	long numTriples = extents.NumTriples(); // total number of triples
	long numObjectsLeft = 0;					// number of ojects on the left during the scan
	long numObjectsRight = extents.NumObjects();	// number of objects on the right during the scan
	double costLeft = 0.0;					// cost of objects on the left during the scan
	double costRight = totalObjectCosts;	// total cost of objects still on the right side during scan	
	ExtentTriple* etPtr = extents.TripleArray;
	bool inFirstHalf = true;			// If still scanning first half, measured in distance along axis
	double midPoint = 0.5*(minOnAxis+maxOnAxis);
	while ( numTriplesLeft<numTriples ) {
		// The split can occur either right before or right after the split value.
		double thisSplitValue = etPtr->ExtentValue;
		bool sameSplitValue = true;
		ExtentTriple::TripleType thisType = etPtr->ExtentType;
		if ( inFirstHalf && thisSplitValue>midPoint ) {
			inFirstHalf = false;
		}

		// Skip past all TT_MAX's, and TT_FLAT's in first half, with this split value
		while ( numTriplesLeft<numTriples ) {
			if ( !( ((thisType==ExtentTriple::TT_MAX 
						|| (thisType==ExtentTriple::TT_FLAT && inFirstHalf)))
					&& sameSplitValue ) )
			{
				break;
			}
            UpdateLeftRightCosts( *etPtr, &numObjectsLeft, &numObjectsRight, &costLeft, &costRight );
			etPtr++;
			numTriplesLeft++;
			thisType = etPtr->ExtentType;
			sameSplitValue = (etPtr->ExtentValue <= thisSplitValue);
		}

		// Ready to call the cost function
		// If the cost function gives better value, save everything appropriately
		if ( CalcSplitCost( thisSplitValue, costLeft, costRight, &bestCost ) ) {
			foundBetter = true;
			*retNewBestCost = bestCost;
			*retSplitValue = thisSplitValue;
			*retNumTriplesToLeft = numTriplesLeft;
			*retNumObjectsToLeft = numObjectsLeft;
			*retNumObjectsToRight = numObjectsRight;
			*retCostObjectsToLeft = costLeft;
			*retCostObjectsToRight = costRight;
		}

		// Skip past all TT_MIN's, and TT_FLAT's in second half, with this split value
		while ( numTriplesLeft<numTriples ) {
			if ( !( sameSplitValue &&  
					(thisType==ExtentTriple::TT_MIN 
					|| (thisType==ExtentTriple::TT_FLAT && !inFirstHalf)) ) ) 
			{
				break;
			}
			// Move rightward a triple
			UpdateLeftRightCosts( *etPtr, &numObjectsLeft, &numObjectsRight, &costLeft, &costRight );
			etPtr++;
			numTriplesLeft++;
			thisType = etPtr->ExtentType;
			sameSplitValue = (etPtr->ExtentValue <= thisSplitValue);
		}

	}

	return foundBetter;
}

void KdTree::UpdateLeftRightCosts( const ExtentTriple& et, long* numObjectsLeft, long* numObjectsRight, 
							   double *costLeft, double *costRight )
{
	double cost;
	if ( UseConstantCost ) {
		cost = ObjectConstantCost;
	}
	else {
		cost = (*UserCostFunction)(et.ObjectID);
	}

	switch ( et.ExtentType ) {
	case ExtentTriple::TT_MAX:
		(*numObjectsRight) --;
		(*costRight) -= cost;
		break;
	case ExtentTriple::TT_FLAT:
		(*numObjectsRight)--;
		(*costRight) -= cost;
		(*numObjectsLeft)++;
		(*costLeft) += cost;
		break;
	case ExtentTriple::TT_MIN:
		(*numObjectsLeft)++;
		(*costLeft) += cost;
		break;
	}
}


// Create the Aabb's for one of the subtrees
void KdTree::MakeAabbsForSubtree( unsigned char leftRightFlag, const ExtentTripleArrayInfo& theExtents,
									const AABB& theAabb )
{
	ExtentTriple* etPtr = theExtents.TripleArray;
	long i;
	long n = theExtents.NumTriples();
	for ( i=0; i<n; i++, etPtr++ ) {
		long objectID = etPtr->ObjectID;
		if ( (LeftRightStatus[ objectID ] & leftRightFlag) != 0 ) {
			// Don't bother if a Max on the left, or a Min on the right.
			//		In these cases, the extent will be computed anyway
			if ( !((etPtr->ExtentType==(ExtentTriple::TT_MIN) && leftRightFlag==2)
				|| (etPtr->ExtentType==(ExtentTriple::TT_MAX) && leftRightFlag==1)) )
			{
				assert ( 0<=objectID && objectID<NumObjects );
				bool stillIn = (*ExtentInBoxFunc)( objectID, theAabb, ObjectAABBs[objectID] );
				bool flatX = ObjectAABBs[objectID].IsFlatX();
				bool flatY = ObjectAABBs[objectID].IsFlatY();
				bool flatZ = ObjectAABBs[objectID].IsFlatZ();
				if ( !stillIn ||(flatX&&flatY) || (flatY&&flatZ) || (flatX&&flatZ) ) {
					// Remove from being in this subtree (bitwise OR with complement of leftRightFlag)
					LeftRightStatus[objectID] &= ~leftRightFlag;
				}
			}
		}
	}
}


// Selectively copy and recalculate extent triples for a sub tree.
//	leftRightFlag is 1 or 2 depending on whether this is for the left 
//		or right subtree.
//	The Aabb's have already been set correctly for the new subtree.
//  The new triples are created and copied in an order that promises to 
//		be as sorted as possible, but they are not fully sorted yet.  
//		After that, they are ShellSorted.
void KdTree::CopyTriplesForSubtree( unsigned char leftRightFlag, int axisNumber,
											ExtentTripleArrayInfo& fromExtents, 
											ExtentTripleArrayInfo& toExtents )
{
	ExtentTriple* fromET = fromExtents.TripleArray;
	ExtentTriple* toET = toExtents.TripleArray;
	long n = fromExtents.NumTriples();			// Number of "from" items left
	long iF = 0;								// Number of "to" flats created
	long iM = 0;								// Number of "to" max/mins created
	for ( ; n>0; n--, fromET++ ) {
		long objectID = fromET->ObjectID;
		assert ( 0<=objectID && objectID<NumObjects );
		if ( LeftRightStatus[objectID] & leftRightFlag ) {
			toET->ObjectID = objectID;
			toET->ExtentType = fromET->ExtentType;
			switch ( fromET->ExtentType ) 
			{
			case ExtentTriple::TT_MIN:
				{
					const AABB& theAABB = ObjectAABBs[objectID];
					double newMinExtent = theAABB.GetBoxMin()[axisNumber];
					double newMaxExtent = theAABB.GetBoxMax()[axisNumber];
					toET->ExtentValue = newMinExtent;
					assert ( newMinExtent<=newMaxExtent );
					if ( newMinExtent < newMaxExtent ) {
						iM++;
					}
					else {
						toET->ExtentType = ExtentTriple::TT_FLAT;
						iF++;
					}
				}
				toET++;
				break;
			case ExtentTriple::TT_FLAT:
				toET->ExtentValue = fromET->ExtentValue;
				iF++;
				toET++;
				break;
			case ExtentTriple::TT_MAX:
				{
					const AABB& theAABB = ObjectAABBs[objectID];
					double newMinExtent = theAABB.GetBoxMin()[axisNumber];
					double newMaxExtent = theAABB.GetBoxMax()[axisNumber];
					toET->ExtentValue = newMaxExtent;
					assert ( newMinExtent<=newMaxExtent );
					if ( newMinExtent < newMaxExtent ) {
						iM++;
						toET++;
					}
					// If newMinExtent>=newMaxExtent, then this was changed to a flat already.
				}
				break;
			default:
				assert(0);
			}
		}
	}
	assert ( (iM&0x01) == 0 );
	toExtents.SetNumbers( iM>>1, iF );
	
	// Now sort the new array of triples
	toExtents.Sort();
}

double KdTree::CalcTotalCosts( const ExtentTripleArrayInfo& extents ) const
{
	long n = extents.NumObjects();
	if ( UseConstantCost ) {
		return ObjectConstantCost*n;
	}
	else {
		double totalCosts = 0;
		ExtentTriple* etPtr = extents.TripleArray;
		for ( long i=0; i<n; i++, etPtr++ ) {
			if ( etPtr->ExtentType != ExtentTriple::TT_MAX ) {
				totalCosts += (*UserCostFunction)(  etPtr->ObjectID );
			}
		}
		return totalCosts;
	}
}


// ****************************************************************************
// Code for cost functions
// ****************************************************************************
inline void KdTree::InitSplitCostFunction( double minOnAxis, double maxOnAxis, 
											double secondAxisLen, double thirdAxisLen,
											double costToBeat, double totalObjectCosts )
{
	switch ( SplitAlgorithm ) {
	case MacDonaldBooth:
	case MacDonaldBoothModifiedCoefs:
		// MacDonald-Booth method
		InitMacdonaldBooth( minOnAxis, maxOnAxis, secondAxisLen, thirdAxisLen,
							costToBeat,totalObjectCosts);
		break;
	case DoubleRecurseGS:
	case DoubleRecurseModifiedCoefs:
		InitDoubleRecurse( minOnAxis, maxOnAxis, secondAxisLen, thirdAxisLen,
							 costToBeat, totalObjectCosts );
		break;
	}
}

inline void KdTree::InitMacdonaldBooth( double minOnAxis, double maxOnAxis, 
										double secondAxisLen, double thirdAxisLen,
										double costToBeat, double totalObjectCosts )
{
	CF_MinOnAxis = minOnAxis;
	CF_MaxOnAxis = maxOnAxis;
	CF_FirstAxisLenInv = 1.0/(maxOnAxis-minOnAxis);
	CF_OldCost = costToBeat;
	CF_TotalNodeObjectCosts = totalObjectCosts;
	CF_EndArea = secondAxisLen*thirdAxisLen;
	CF_Wrap = 2.0*(maxOnAxis-minOnAxis)*(secondAxisLen+thirdAxisLen);
	CF_Area = 2.0*CF_EndArea + CF_Wrap;
}

inline void KdTree::InitDoubleRecurse( double minOnAxis, double maxOnAxis, 
										double secondAxisLen, double thirdAxisLen,
										double costToBeat, double totalObjectCosts )
{
	CF_MinOnAxis = minOnAxis;
	CF_MaxOnAxis = maxOnAxis;
	CF_FirstAxisLenInv = 1.0/(maxOnAxis-minOnAxis);
	CF_OldCost = costToBeat;
	CF_TotalNodeObjectCosts = totalObjectCosts;
	CF_LogTNOCinv = 1.0/log(CF_TotalNodeObjectCosts);
	CF_EndArea = secondAxisLen*thirdAxisLen;
	CF_Wrap = 2.0*(maxOnAxis-minOnAxis)*(secondAxisLen+thirdAxisLen);
	CF_Area = 2.0*CF_EndArea + CF_Wrap;

	// Calculate double recurse cost exponent to beat
	if ( CF_EndArea > 1.0e-14*CF_Area ) {
		CF_D = -CF_Area/(2.0*CF_EndArea);
		CF_C = 1.0 - CF_D;
		CF_ExponentToBeat = log((costToBeat-CF_D)/CF_C) * CF_LogTNOCinv;
		assert ( 0<CF_ExponentToBeat && CF_ExponentToBeat<1.0 );
	}
	else {
		CF_EndArea = 0.0;		// End area is small enough to treat as being exactly zero
	}
}

bool KdTree::CalcSplitCost( double splitValue, double costLeft, double costRight, double* retCost )
{
	switch ( SplitAlgorithm ) {
	case MacDonaldBooth:
		// MacDonald-Booth method
		return CalcMacdonaldBooth( splitValue, costLeft, costRight, retCost );
	case MacDonaldBoothModifiedCoefs:
		// MacDonald-Booth method with modified coefs
		return CalcMacdonaldBoothModifiedCoefs( splitValue, costLeft, costRight, retCost );
	case DoubleRecurseGS:
	case DoubleRecurseModifiedCoefs:
		// Buss double-recurse method
		return CalcDoubleRecurseGS( splitValue, costLeft, costRight, retCost );
		break;
	default:
		assert(0);
		return false;
	}
}

bool KdTree::CalcMacdonaldBooth( double splitValue, double costLeft, double costRight, double* retCost )
{
	double gamma = (splitValue-CF_MinOnAxis)*CF_FirstAxisLenInv;
	double surfaceAreaLeft = 2.0*CF_EndArea + gamma*CF_Wrap;
	double surfaceAreaRight = 2.0*CF_EndArea + (1.0-gamma)*CF_Wrap;
	double newCost = 1.0 + (surfaceAreaLeft*costLeft + surfaceAreaRight*costRight)/CF_Area;
	if ( newCost<CF_OldCost ) {
		*retCost = newCost;
		CF_OldCost = newCost;
		return true;
	}
	else {
		return false;
	}
}

bool KdTree::CalcMacdonaldBoothModifiedCoefs( double splitValue, double costLeft, double costRight, double* retCost )
{
	double gamma = (splitValue-CF_MinOnAxis)*CF_FirstAxisLenInv;
	double surfaceAreaLeft = 2.0*CF_EndArea + gamma*CF_Wrap;
	double surfaceAreaRight = 2.0*CF_EndArea + (1.0-gamma)*CF_Wrap;
	double modFade = CF_TotalNodeObjectCosts/TotalObjectCosts;
	double fracLeft = costLeft/(costLeft+costRight);
	double fracRight = 1.0-fracLeft;
	double newCost = 1.0;
	newCost += (1.0-modFade)*(surfaceAreaLeft*costLeft + surfaceAreaRight*costRight)/CF_Area;
	newCost += modFade*((fracLeft+fracRight*CF_EndArea/surfaceAreaRight)*costLeft
						+(fracRight+fracLeft*CF_EndArea/surfaceAreaLeft)*costRight);

	if ( newCost<CF_OldCost ) {
		*retCost = newCost;
		CF_OldCost = newCost;
		return true;
	}
	else {
		return false;
	}
}

bool KdTree::CalcDoubleRecurseGS( double splitValue, double costLeft, double costRight, double* retCost )
{
	double gamma = (splitValue-CF_MinOnAxis)*CF_FirstAxisLenInv;
	double surfaceAreaLeft = 2.0*CF_EndArea + gamma*CF_Wrap;
	double surfaceAreaRight = 2.0*CF_EndArea + (1.0-gamma)*CF_Wrap;
	double A = surfaceAreaLeft/CF_Area;
	double B = surfaceAreaRight/CF_Area;
	double alpha = costLeft/CF_TotalNodeObjectCosts;
	double beta = costRight/CF_TotalNodeObjectCosts;
	if ( SplitAlgorithm==DoubleRecurseModifiedCoefs ) {
		double modFade = CF_TotalNodeObjectCosts/TotalObjectCosts;
		double fracLeft = costLeft/(costLeft+costRight);
		double fracRight = 1.0-fracLeft;
		A = Lerp(A, fracLeft+fracRight*CF_EndArea/surfaceAreaRight, modFade);
		B = Lerp(B, fracRight+fracLeft*CF_EndArea/surfaceAreaLeft, modFade);
	}

	if ( costLeft==0.0 || costRight==0.0 ) {
		double newCost;
		if ( costLeft==0.0 && B<1.0 ) {
			newCost = 1.0/(1.0-B);
		}
		else if ( costRight==0.0 && A<1.0 ) {
			newCost = 1.0/(1.0-A);
		}
		else {
			return false;
		}
		if ( newCost<CF_OldCost ) {
			*retCost = newCost;
			CF_OldCost = newCost;
			if ( CF_EndArea!=0.0 ) {
				CF_ExponentToBeat = log( (newCost-CF_D)/CF_C ) * CF_LogTNOCinv;
				assert ( 0<CF_ExponentToBeat && CF_ExponentToBeat<1.0 );
			}
			return true;
		}
		return false;
	} 

	if ( CF_EndArea==0.0 ) {
		if ( alpha!=0.0 && beta!=0.0 ) {
			double newCost = 1.0 - log(CF_TotalNodeObjectCosts)/(A*log(alpha)+B*log(beta));
			if ( newCost<CF_OldCost ) {
				*retCost = newCost;
				CF_OldCost = newCost;
				return true;
			}
		}
		return false;
	}

	double C, D;
	double newExponent;
	bool betterCost = FindDoubleRecurseSoln( A, B, alpha, beta, &C, &newExponent, &D, CF_ExponentToBeat );
	if ( betterCost ) {
		CF_ExponentToBeat = newExponent;
		assert ( 0<CF_ExponentToBeat && CF_ExponentToBeat<1.0 );
		CF_OldCost = C * pow(CF_TotalNodeObjectCosts,newExponent) + D;
		*retCost = CF_OldCost;
		return true;
	}

	return false;
}

void KdTree::MemoryError()
{
	assert(0);
	fprintf(stderr,"KdTree construction: Failed to allocate memory.\n");
	exit(0);
}

void KdTree::MemoryError2()
{
	assert(0);
	fprintf(stderr,"KdTree: Memory overflow. Need to increase storage multiplier.\n");
	exit(0);
}
