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

// RayTraceStats.cpp
// Ray Trace Statistics

#include "RayTraceStats.h"
#include "../DataStructs/Stack.h"
#include "../DataStructs/KdTree.h"

void RayTraceStats::Init()
{
	NumberRaysTraced = 0;
	NumberReflectionRays = 0;
	NumberXmitRays = 0;
	NumberShadowFeelers = 0;
	NumberIsectTests = 0;
	NumberSuccessIsectTests = 0;

	NumberKdNodesTraversed = 0;
	NumberKdLeavesTraversed = 0;
	NumberKdObjectsInLeaves = 0;

}

void RayTraceStats::GetKdRunData( const KdTree& kdTree )
{
	kdTree.Stats_GetAll( &NumberKdNodesTraversed, &NumberKdLeavesTraversed, &NumberKdObjectsInLeaves );
}

void RayTraceStats::PrintStats( FILE* out )
{
#if TrackRaysTraced
	fprintf( out, "Run time statistics:\n");
	fprintf( out, "  Number of rays traced = %ld.\n", NumberRaysTraced );
#endif
#if TrackShadowFeelers
	fprintf( out, "  Number of shadow feelers = %ld.\n", NumberShadowFeelers );
#endif
#if TrackKdTraversal
	fprintf( out, "  KdTree: Nodes traversed, %ld.  Non-empty leaves traversed, %ld.\n", 
				NumberKdNodesTraversed, NumberKdLeavesTraversed );
	fprintf( out, "          Objects tested, %ld.  Mean number per leaf, %0.6lf.\n", 
				NumberKdObjectsInLeaves, 
				(double)NumberKdObjectsInLeaves/(double)NumberKdLeavesTraversed );
	double numRays = NumberRaysTraced;
	fprintf( out, "  Kd per ray: Nodes, %0.6lf. Leaves, %0.6lf. Objects, %lf.\n",
				(double)NumberKdNodesTraversed/numRays, 
				(double)NumberKdLeavesTraversed/numRays,
				(double)NumberKdObjectsInLeaves/numRays );
#endif
}

void RayTraceStats::PrintKdStats( const KdTree& kdTree, FILE* out )
{
#if TrackKdProperties
	long numNodes = 0;
	long maxDepth = 0;
	long numLeaves = 0;
	long numAtDepth[128];
	long sumLeafDepths = 0;
	long sumNodeDepths = 0;
	long numEmptyLeaves = 0;
	long numObjectsAtLeaves = 0;

	// Traverse the tree, gathering data
	Stack<long> treeNodeStack;
	Stack<long> levelStack;
	treeNodeStack.Push(0);
	levelStack.Push(0);
	while ( !treeNodeStack.IsEmpty() ) {
		numNodes++;
		long i = treeNodeStack.Pop();
		long level = levelStack.Pop();
		if ( level>maxDepth ) {
			maxDepth = level;
		}
		sumNodeDepths += level;
		numAtDepth[Min((long)127,level)]++;
		const KdTreeNode& thisNode = kdTree.GetNode(i);
		if ( thisNode.IsLeaf() ) {
			numLeaves++;
			sumLeafDepths += level;
			numObjectsAtLeaves += thisNode.GetNumObjects();
		}
		else {
			if ( !thisNode.RightChildEmpty() ) {
				treeNodeStack.Push( thisNode.RightChildIndex() );
				levelStack.Push( level+1 );
			}
			else {
				numEmptyLeaves++;
			}
			if ( !thisNode.LeftChildEmpty() ) {
				treeNodeStack.Push( thisNode.LeftChildIndex() );
				levelStack.Push ( level+1 );
			}
			else {
				numEmptyLeaves++;
			}
		}
	}

	fprintf( out, "KdTree statistics:\n");
	fprintf( out, "       Number of objects: %ld.\n", kdTree.NumObjects );
	fprintf( out, "       Number of nodes: %ld.\n", numNodes);
	fprintf( out, "       Number of leaves: %ld.\n", numLeaves);
	fprintf( out, "       Number of empty leaves: %ld.\n", numEmptyLeaves );
	fprintf( out, "       Objects at leaves:  Total number is %ld.  Average per leaf: %.5lf.\n",
						numObjectsAtLeaves, (double)(numObjectsAtLeaves)/(double)(numLeaves) );
	fprintf( out, "       Depth: %ld.\n", maxDepth);
	fprintf( out, "       Mean depths: All nodes, %.5lf.  Leaf nodes, %.5lf.\n", 
					(double)sumNodeDepths/(double)(numNodes), (double)sumLeafDepths/(double)(numLeaves) );
#endif

}
