#ifndef _321892366_B_H
#define _321892366_B_H
#include "Direction.h"

#include "SmartAlgorithm.h"
#include "AbstractSensor.h"

#include "Utils.h"

#include "Point.h"

#include <stack>

// AlgorithmRegistration.h is part of BOTH the simulation project
// and the Algorithm projects
#include "AlgorithmRegistration.h"

#include "MakeUnique.h"

class _321892366_B: public SmartAlgorithm {
	public:
	//constructor
	_321892366_B();

	// step is called by the simulation for each time unit
	virtual Direction step(Direction prevStep) override;

	//destructor
	virtual ~_321892366_B() {};
};

#endif /* __321892366_B_H_ */
