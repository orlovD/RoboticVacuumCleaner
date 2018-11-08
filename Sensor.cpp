#include "Sensor.h"

//constructor
Sensor::Sensor(House* house) {
	this->house = house;
}
//get sensor data
SensorInformation Sensor::sense() const {
	return this->sensInfo;
}

//sets sensor location and updates its sensor data
void Sensor::setLocation(const Point& location) {
	sensInfo.dirtLevel = house->getDirtAtLocation(location);
	//check walls around the current location
	sensInfo.isWall[(int)Direction::East] = house->hasWallAt(location.getY(), location.getX() + 1);
	sensInfo.isWall[(int)Direction::West] = house->hasWallAt(location.getY() , location.getX() - 1);
	sensInfo.isWall[(int)Direction::South] = house->hasWallAt(location.getY() + 1, location.getX());
	sensInfo.isWall[(int)Direction::North] = house->hasWallAt(location.getY() - 1, location.getX());
}

