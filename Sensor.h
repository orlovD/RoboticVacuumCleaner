#ifndef __SENSOR__H_
#define __SENSOR__H_

#include "Direction.h"

//#include "AbstractAlgorithm.h"
#include "AbstractSensor.h"

#include "Point.h"
#include "House.h"

class Sensor: public AbstractSensor {
	//sensor data
	SensorInformation sensInfo;
	//pointer to house
	House* house;

public:
	//constructor
	Sensor(House *house);
	//get sensor data
	virtual SensorInformation sense() const override;
	//update sensor data based on house map
	void setLocation(const Point& location);
	//destructor
	virtual ~Sensor() override {};
};

#endif /* __SENSOR__H_ */
