#ifndef KDDATA_H
#define KDDATA_H

#include "../Graphics/ViewableBase.h"

class KdData {
public:
	KdData() 
	: isectEpsilon(1.0e-6), bestObject(-1), bestHitDistance(DBL_MAX) {}
	bool kdTraverseFeeler;
	double isectEpsilon;
	long bestObject;
	long kdTraverseAvoid;
	double bestHitDistance;
	double kdShadowDist;
	VisiblePoint tempPoint;
	VisiblePoint* bestHitPoint;
	VectorR3 kdStartPos;
	VectorR3 kdStartPosAvoid;
	VectorR3 kdTraverseDir;
};

#endif
