#include "_321892366_C.h"

// this should be a global line in _321892366_C.cpp
REGISTER_ALGORITHM (_321892366_C)

//constructor
_321892366_C::_321892366_C() : SmartAlgorithm(){}

// step is called by the simulation for each time unit
Direction _321892366_C::step(Direction prevStep) { //decide where to go depending on the sensor's info
	setStartingMode(getMode());
	//increase current step
	increaseCurrentStep();

	//if Simulation performed different step - return to previous point
	Direction check_prev_step = returnToRoute(prevStep);
	if (check_prev_step != Direction::Stay) {
		//return returning to route direction
		return check_prev_step;
	}

	//vector with available directions - directions without walls
	vector<Direction> direction_vector;
	//vector of available directions from current point - excluding visited points
	vector<Direction> available_directions;
	//vector of adjacent points to current point
	vector<Point> adjacent_points;
	//save information from sensor
	SensorInformation sensor_information;
	//get sensor data
	sensor_information = getSensorInformation();

	//get new direction from current point in order of priorities
	getNewDirection3(sensor_information, available_directions);

	//check if it's time to return to docking
	//calculate remained steps on current battery level
	int number_of_steps_remained = getCurrentBattery() / getBatteryConsumptionRate();
	//if there is a remained
	if (getCurrentBattery() % getBatteryConsumptionRate() != 0) {
		//add one more steps - battery will drop below zero at docking
		number_of_steps_remained++;
	}
	//if its time to start finishing process
	if (getStepsTillFinishing() >= 0) {
		//robot will decide when to return to docking based on this information
		number_of_steps_remained = min(number_of_steps_remained, getStepsTillFinishing());
	}

	//if algorithm is NOT returning from docking
//	if (getMode() != RETURN_FROM_DOCKING) {
		//distance to docking is equal to route_to_docking stack's size
		setStepsToDocking(getRouteToDockingSize());
//	}

	//if robot cannot make additional move away from docking
	//add 10% for possible steps not accepted by Simulation
	if ((number_of_steps_remained < (double)(getStepsToDocking() + 2 * getBatteryConsumptionRate()) * 1.1) && !getIsReturningToDocking()) {
		//returning to docking started
		setIsReturningToDocking(true);
		//change mode to return to docking
		setMode(RETURN_TO_DOCKING);
		//try to find shorter path to docking
		if (!updatePathToDocking(adjacent_points)) {
			//if shorter path to is not found - add previous step to route_to_docking stack
			routeToDockingPush(getPrevStep());
		}
		//save current point as point to return after charging
		setPointToReturn(getCurrPoint());
	}

	//start returning to docking process
	if (getMode() == RETURN_TO_DOCKING) {
		//check surrounding walls and adjacent points
		checkWalls(sensor_information, direction_vector, adjacent_points);
		//now direction_vector holds all possible direction - directions without walls
		//adjacent_points holds all points adjacent to current

		//add pair of current point with its adjacent points into map
		if (!updatePathToDocking(adjacent_points)) {
			//decrease number of steps to docking
			decreaseStepsToDocking();
		}
		//if path to docking was updated
		else {
			//distance to docking is equal to route_to_docking stack's size
			setStepsToDocking(getRouteToDockingSize());
		}
		//if arrived to docking
		if (getCurrPoint() == getDocking()) {
			//charge till  battery is full
			if (getCurrentBattery() < getBatteryCapacity()) {
				//cannot charge more than a maximum capacity
				setCurrentBattery(min(getCurrentBattery() + getBatteryRechargeRate(), getBatteryCapacity()));
				//stay at docking while charging
				Direction charging = Direction::Stay;
				//if its time to finish - decrease steps_till_finishing
				decreaseStepsTillFinishing();
				//set prev_step to current direction chosen
				setPrevStep(charging);
				//set step chosen by algorithm
				setAlgorithmStep(charging);
				//return Stay
				return charging;
			}
			//battery is full - return to last point
			else {
				setMode(RETURN_FROM_DOCKING);
				//returning to docking is finished
				setIsReturningToDocking(false);
				//now robot is at docking and will return to point from where it started to return to docking
				setStepsToDocking(0);
			}
		}
		//continue returning to docking
		else {
			//set return_direction_to_docking to Stay - will be updated later
			Direction return_direction_to_docking = Direction::Stay;
			//get return direction from stack
			return_direction_to_docking = getReturnToDockingDirection(return_direction_to_docking);
			//revert current direction
			revertDirection(return_direction_to_docking);
			//get return direction
			return_direction_to_docking = getChoice();
			//decrease current battery level by battery consumption
			decreaseBatteryLevel();
			//update current point accordingly to chosen direction
			updateCurrentPoint(return_direction_to_docking);
			//decrease steps to docking
//			decreaseStepsToDocking();
			//if its time to finish - decrease steps_till_finishing
			decreaseStepsTillFinishing();
			//set prev_step to current direction chosen
			setPrevStep(return_direction_to_docking);
			//add new direction to route_from_docking
			routeFromDockingPush(return_direction_to_docking);
			//set step chosen by algorithm
			setAlgorithmStep(getPrevStep());
			//return chosen direction
			return return_direction_to_docking;
		}
	}
	else {
		setIsReturningToDocking(false);
	}

	//start returning process to last visited point continue cleaning
	if (getMode() == RETURN_FROM_DOCKING) {
		//if returned to saved point
		if (getCurrPoint() == getPointToReturn()) {
			//continue to clean the house
			setMode(EXPLORING);
		}
		//if not returned to saved point - continue to return
		else {
			Direction return_direction_from_docking = Direction::Stay;
			//get return_direction_from_docking from stack
			return_direction_from_docking = getReturnFromDockingDirection(return_direction_from_docking);
			//revert current direction
			revertDirection(return_direction_from_docking);
			//get return direction
			return_direction_from_docking = getChoice();

			//decrease current battery level by battery consumption
			decreaseBatteryLevel();
			//update current point accordingly to chosen direction
			updateCurrentPoint(return_direction_from_docking);
			//increase steps to docking
//			increaseStepsToDocking();
			//if its time to finish - decrease steps_till_finishing
			decreaseStepsTillFinishing();
			//set prev_step to current direction chosen
			setPrevStep(return_direction_from_docking);

			//update path to docking using paths of adjacent points
			if (!updatePathToDocking(adjacent_points)) {
				if (getChoice() != Direction::Stay) {
					//if path cannot be updated - add to current choice return_direction_from_docking stack
					routeToDockingPush(return_direction_from_docking);
					//increase number of steps to docking
					increaseStepsToDocking();
				}
			}
			//if path to docking was updated
			else {
				//distance to docking is equal to route_to_docking stack's size
				setStepsToDocking(getRouteToDockingSize());
			}
			//set save chosen by algorithm
			setAlgorithmStep(return_direction_from_docking);
				//return chosen direction
				return return_direction_from_docking;
			}
		}

	if (getMode() == EXPLORING) {
		//check surrounding walls and adjacent points
		checkWalls(sensor_information, direction_vector, adjacent_points);
		//now direction_vector holds all possible direction - directions without walls
		//adjacent_points holds all points adjacent to current

		//update path to docking using paths of adjacent points
		if (!updatePathToDocking(adjacent_points)) {
			//if any step except Stay was chosen previously
			if( getPrevStep() != Direction::Stay) {
				//add previous step to route_to_docking
				routeToDockingPush(getPrevStep());
			}
		}

		//add pair of current point with its adjacent points vector into map
		adjacentEmplace(getCurrPoint(), adjacent_points);

		//if some move performed
		if (getChoice() != Direction::Stay) {
			//add chosen direction to stack of steps
			routePush(getChoice());
			//update distance to docking
			increaseStepsToDocking();
		}

		//if there are no available directions from current point and cleaning is finished there
		if (available_directions.size() == 0 && sensor_information.dirtLevel == 0) {
			//set mode to returning mode
			setMode(RETURN_TO_UNEXPLORED);
		}
		//try to add current point to vector of unexplored directions
		addPointToUnexplored(getChoice(), available_directions);
	}

	//returning to unexplored point mode
	if (getMode() == RETURN_TO_UNEXPLORED) {
		//check surrounding walls and adjacent points
		checkWalls(sensor_information, direction_vector, adjacent_points);
		//now direction_vector holds all possible direction - directions without walls
		//adjacent_points holds all points adjacent to current

		//if path to docking didn't update
		if (!updatePathToDocking(adjacent_points)) {
			//if any step except Stay is chosen
			if (getPrevStep() != Direction::Stay) {
				//add previous step to route_to_docking stack
				routeToDockingPush(getPrevStep());
			}
		}

		//get closest unexplored point if there is any
		if (unexploredPointsEmpty()) {
			//add docking as closest unexplored point
			unexploredPointsEmplace(Point(0,0), Direction::Stay);
		}
		else {
			//get destination point
			setDestPoint(unexploredPointsTop().first);
			//get last direction chosen from this point
			setLastChoice(unexploredPointsTop().second);
		}
		//if still not arrived to closest unexplored point
		if (getCurrPoint() != getDestPoint()) {
			//get direction from stack - will be saved into variable choice
			getReturnToUnexploredDirection();
			//revert current direction
			revertDirection();
		}
		//if arrived to closest unexplored point
		else {
			//change mode to exploring mode
			setMode(EXPLORING);
			//clear the list of directions
			available_directions.clear();
			//chose next unexplored direction//EAST=>NORTH=>WEST=>SOUTH
			getNewUnexploredDirection(sensor_information, prevStep, available_directions);
		}
		//check if Stay is chosen on unexplored point arrived to -> need to continue to new unexplored point!
		if (getChoice() == Direction::Stay) {
			//arrived onto clean point
			if (sensor_information.dirtLevel == 0) {
				//remove last unexplored point from stack
				unexploredPointsPop();
				//set mode to RETURN_TO_UNEXPLORED - return to another unexplored point
				setMode(RETURN_TO_UNEXPLORED);
			}
		}
	}

	//check that point was cleaned to 0 dirt
	if (sensor_information.dirtLevel == 0) {
		//current point was cleaned to 0 dirt
		cleanPointsPushBack(getCurrPoint());
	}

	//update current point using direction chosen
	updateCurrentPoint(getChoice());
	//decrease current battery level by battery consumption
	decreaseBatteryLevel();
	//if its time to finish - decrease steps_till_finishing
	decreaseStepsTillFinishing();

	//set prev_step to current direction chosen
	setPrevStep(getChoice());
	//set step chosen by algorithm
	setAlgorithmStep(getChoice());
	//return chosen direction
	return getChoice();
}
