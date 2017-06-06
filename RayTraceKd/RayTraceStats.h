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


// RayTraceStats.h
//  A class for maintaining statistics about ray tracing


#include <stdio.h>
class KdTree;

#define TrackRaysTraced 1
#define TrackReflectionRays 1
#define TrackXmitRays 1
#define TrackShadowFeelers 1
#define TrackIsectTests 1
#define TrackSuccessIsectTests 1
#define TrackKdProperties 1
#define TrackKdTraversal 1

class RayTraceStats
{
public:
	RayTraceStats() { Init(); }
	~RayTraceStats() {}

	void Init();

	void PrintStats( FILE* out = stdout );

	void AddRayTraced();
	void AddReflectionRay();
	void AddXmitRay();
	void AddShadowFeeler();
	void AddIsectTest();
	void AddSuccessIsectTest();
	
	void AddKdNodeTraversed();
	void AddKdLeavesTraversed();
	void AddKdObjectsInLeavesTraversed( int numObjects = 1 );

public:
	void GetKdRunData( const KdTree& kdTree );
	static void PrintKdStats( const KdTree& kdTree, FILE* out = stdout );

private:
	long NumberRaysTraced;
	long NumberReflectionRays;
	long NumberXmitRays;
	long NumberShadowFeelers;
	long NumberIsectTests;
	long NumberSuccessIsectTests;

	// KdTree operations
	long NumberKdNodesTraversed;
	long NumberKdLeavesTraversed;
	long NumberKdObjectsInLeaves;

};

inline void RayTraceStats::AddRayTraced()
{
#if TrackRaysTraced
	NumberRaysTraced++;
#endif
}

inline void RayTraceStats::AddReflectionRay()
{
#if TrackReflectionRays
	NumberReflectionRays++;
#endif
}

inline void RayTraceStats::AddXmitRay()
{
#if TrackXmitRays
	NumberXmitRays++;
#endif
}

inline void RayTraceStats::AddShadowFeeler()
{
#if TrackShadowFeelers
	NumberShadowFeelers++;
#endif
}

inline void RayTraceStats::AddIsectTest()
{
#if TrackIsectTests
	NumberIsectTests++;
#endif
}

inline void RayTraceStats::AddSuccessIsectTest()
{
#if TrackSuccessIsectTests
	NumberSuccessIsectTests++;
#endif
}

inline void RayTraceStats::AddKdNodeTraversed()
{
#if TrackKdTraversals
	NumberKdNodesTraversed++;
#endif
}

inline void RayTraceStats::AddKdLeavesTraversed()
{
#if TrackKdTraversals
	NumberKdNodesTraversed++;
#endif
}

inline void RayTraceStats::AddKdObjectsInLeavesTraversed( int numObjects )
{
#if TrackKdTraversals
	NumberKdNodesTraversed += numObjects;
#endif
}
