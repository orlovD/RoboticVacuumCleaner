#include "SmartAlgorithm.h"

// this should be a global line in SmartAlgorithm.cpp
//REGISTER_ALGORITHM (SmartAlgorithm)

//constructor - initialize starting point to (0,0)
SmartAlgorithm::SmartAlgorithm() : sensor(NULL), curr_point(0,0), choice(Direction::Stay),
last_choice(Direction::Stay), mode(EXPLORING), starting_mode(mode), current_battery(0), steps_to_docking(0), current_step(0),
is_returning_to_docking(false), steps_till_finishing(-1), prev_step(Direction::Stay),
steps_from_docking(0), algorithm_step(Direction::Stay), is_back_on_route(true) {
	//set docking placement
	docking = Point(0,0);
}

// setSensor is called once when the Algorithm is initialized
//initialize all required data for current run
void SmartAlgorithm::setSensor(const AbstractSensor& sensor) {
	this->sensor = &sensor;
	//set docking placement
	docking = Point(0,0);
	//reset point on each new house
	curr_point = Point(0,0);
	//default step is Stay
	choice = Direction::Stay;
	//last_choise step is Stay
	last_choice = Direction::Stay;
	//reset list of clean points
	clean_points.clear();
	//reset map of adjacent points
	adjacent.clear();
	//reset mode to starting mode
	mode = EXPLORING;
	//set starting mode to mode
	starting_mode = mode;
	//get current battery level
	current_battery = this->configuration["BatteryCapacity"];
	//reset number of steps for current run
	current_step = 0;
	//set steps till end of simulation
	steps_till_finishing = -1;
	//initialize previous step
	prev_step = Direction::Stay;
	//initialize algorithm step
	algorithm_step = Direction::Stay;
	//initialize was_step_performed to true
	is_back_on_route = true;
	//empty stacks
	while (!unexplored_points.empty()) {
		unexplored_points.pop();
	}
	while (!route.empty()) {
		route.pop();
	}
	while (!route_to_docking.empty()) {
		route_to_docking.pop();
	}
	while (!route_from_docking.empty()) {
		route_from_docking.pop();
	}
	while (!simulation_interference.empty()) {
		simulation_interference.pop();
	}
	//empty maps
	if (adjacent.size()) {
		adjacent.clear();
	}
	if (points_with_path_to_docking.size()) {
		points_with_path_to_docking.clear();
	}
	//empty vector
	if (clean_points.size()) {
		clean_points.clear();
	}
}

// setConfiguration is called once when the Algorithm is initialized - see below
void SmartAlgorithm::setConfiguration(map<string, int> config) {
	this->configuration = config;
}

//revert the direction
void SmartAlgorithm::revertDirection(Direction current_direction) {
	if (current_direction == Direction::East) {
		choice = Direction::West;
	}
	if (current_direction == Direction::West) {
		choice = Direction::East;
	}
	if (current_direction == Direction::North) {
		choice = Direction::South;
	}
	if (current_direction == Direction::South) {
		choice = Direction::North;
	}
	if (current_direction == Direction::Stay) {
		choice = Direction::Stay;
	}
}

//revert the direction
void SmartAlgorithm::revertDirection() {
	switch (choice) {
		case Direction::East: {
			choice = Direction::West;
			break;
		}
		case Direction::West: {
			choice = Direction::East;
			break;
		}
		case Direction::North: {
			choice = Direction::South;
			break;
		}
		case Direction::South: {
			choice = Direction::North;
			break;
		}
		case Direction::Stay: {
			choice = Direction::Stay;
			break;
		}
		default: {
		}
	}
}

//update current point accordingly to chosen direction
void SmartAlgorithm::updateCurrentPoint(Direction chosen_direction) {
	//if East chosen
	if (chosen_direction == Direction::East) {
		//x increases by 1, y doesn't change
		curr_point.increaseX();
	}
	//if West chosen
	if (chosen_direction == Direction::West) {
		//x decreases by 1, y doesn't change
		curr_point.decreaseX();
	}
	//if South chosen
	if (chosen_direction == Direction::South) {
		//y increases by 1, x doesn't change
		curr_point.increaseY();
	}
	//if North chosen
	if (chosen_direction == Direction::North) {
		//y decreases by 1, x doesn't change
		curr_point.decreaseY();
	}
}

//decrease current battery level by battery consumption
void SmartAlgorithm::decreaseBatteryLevel() {
	//decrease current battery level by battery consumption
	current_battery -= configuration["BatteryConsumptionRate"];
}

//if its time to finish - decrease steps_till_finishing
void SmartAlgorithm::decreaseStepsTillFinishing() {
	//if its time to finish
	if (steps_till_finishing > 0) {
		//decrease steps_till_finishing by 1 on every step
		steps_till_finishing--;
	}
}

//get return_direction_to_docking from stack
Direction SmartAlgorithm::getReturnToDockingDirection(Direction return_direction_to_docking) {
	//if size is not zero
	if (!route_to_docking.empty()) {
		//get direction from stack of returning to docking
		return_direction_to_docking = route_to_docking.top();
		//remove direction from stack
		route_to_docking.pop();
	} else {
		//if stack is empty - stay
		return_direction_to_docking = Direction::Stay;
	}
	return return_direction_to_docking;
}

//get return_direction_from_docking from stack
Direction SmartAlgorithm::getReturnFromDockingDirection(Direction return_direction_from_docking) {
	//if size is not zero
	if (!route_from_docking.empty()) {
		//get direction from stack of returning from docking
		return_direction_from_docking = route_from_docking.top();
		//remove direction from stack
		route_from_docking.pop();
	} else {
		//if stack is empty - stay
		return_direction_from_docking = Direction::Stay;
	}
	return return_direction_from_docking;
}

//get return_direction_to_unexplored from stack
Direction SmartAlgorithm::getReturnToUnexploredDirection(Direction return_direction_to_unexplored) {
	//if size is not zero
	if (!route.empty()) {
		//get direction from stack of returning from docking
		return_direction_to_unexplored = route.top();
		//remove direction from stack
		route.pop();
	} else {
		//if stack is empty - stay
		return_direction_to_unexplored = Direction::Stay;
	}
	return return_direction_to_unexplored;
}

//get return_direction_to_unexplored from stack
void SmartAlgorithm::getReturnToUnexploredDirection() {
	//if size is not zero
	if (!route.empty()) {
		//get direction from stack of returning from docking
		choice = route.top();
		//remove direction from stack
		route.pop();
	} else {
		//if stack is empty - stay
		choice = Direction::Stay;
	}
}

//check surrounding walls and adjacent points
void SmartAlgorithm::checkWalls(SensorInformation sensor_information, vector<Direction>& direction_vector, vector<Point>& adjacent_points) {
	//size of array sensor_information.isWall is 4
	for (int i = 0; i < 4; i++) {
		//no walls in given direction
		if (!sensor_information.isWall[i]) {
			switch (i) {
			case 0: {
				//add current direction into direction_vector
				direction_vector.push_back(Direction::East);
				//x increases by 1, y doesn't change
				Point east_point(curr_point.getX() + 1, curr_point.getY());
				//add point in this direction to vector of adjacent points
				adjacent_points.push_back(east_point);

				//add East point to map of points with routes to docking
				if (curr_point == docking) {
					stack<Direction> temp_stack;
					//add direction from current point to adjacent point into the stack
					temp_stack.push(Direction::East);
					//add new entry into the points_with_path_to_docking map
					points_with_path_to_docking.emplace(east_point, temp_stack);
				}
				else {
					stack<Direction> temp_stack = points_with_path_to_docking.find(curr_point)->second;
					//check length of the current path
					int new_length = temp_stack.size() + 1;
					//check length of the old path
					int old_length;
					//iterator for searching map
					std::map<Point, stack<Direction>>::iterator place_of_point;
					//check if East point is already added into points_with_path_to_docking map
					place_of_point = points_with_path_to_docking.find(east_point);
					//if point is found in a map
					if (place_of_point != points_with_path_to_docking.end()) {
						//get route from East point to docking
						stack<Direction> old_stack = points_with_path_to_docking.find(east_point)->second;
						old_length = old_stack.size();
						//only if new length is shorter - update the path
						if (old_length >  new_length) {
							//add direction from current point to adjacent point into the stack
							temp_stack.push(Direction::East);
							//remove old entry from map
							points_with_path_to_docking.erase(east_point);
							//add new entry into the points_with_path_to_docking map
							points_with_path_to_docking.emplace(east_point, temp_stack);
						}
					}
					//if point is not found in a map
					else {
						//add direction from current point to adjacent point into the stack
						temp_stack.push(Direction::East);
						//add new entry into the points_with_path_to_docking map
						points_with_path_to_docking.emplace(east_point, temp_stack);
					}
				}
				break;
			}
			case 1: {
				//add current direction into direction_vector
				direction_vector.push_back(Direction::West);
				//x decreases by 1, y doesn't change
				Point west_point(curr_point.getX() - 1, curr_point.getY());
				//add point in this direction to vector of adjacent points
				adjacent_points.push_back(west_point);

				//add West point to map of points with routes to docking
				if (curr_point == docking) {
					stack<Direction> temp_stack;
					//add direction from current point to adjacent point into the stack
					temp_stack.push(Direction::West);
					//add new entry into the points_with_path_to_docking map
					points_with_path_to_docking.emplace(west_point, temp_stack);
				}
				else {
					stack<Direction> temp_stack = points_with_path_to_docking.find(curr_point)->second;
					//check length of the current path
					int new_length = temp_stack.size() + 1;
					//check length of the old path
					int old_length;
					//iterator for searching map
					std::map<Point, stack<Direction>>::iterator place_of_point;
					//check if East point is already added into points_with_path_to_docking map
					place_of_point = points_with_path_to_docking.find(west_point);
					//if point is found in a map
					if (place_of_point != points_with_path_to_docking.end()) {
						//get route from West point to docking
						stack<Direction> old_stack = points_with_path_to_docking.find(west_point)->second;
						old_length = old_stack.size();
						//only if new length is shorter - update the path
						if (old_length >  new_length) {
							//add direction from current point to adjacent point into the stack
							temp_stack.push(Direction::West);
							//remove old entry from map
							points_with_path_to_docking.erase(west_point);
							//add new entry into the points_with_path_to_docking map
							points_with_path_to_docking.emplace(west_point, temp_stack);
						}
					}
					//if point is not found in a map
					else {
						//add direction from current point to adjacent point into the stack
						temp_stack.push(Direction::West);
						//add new entry into the points_with_path_to_docking map
						points_with_path_to_docking.emplace(west_point, temp_stack);
					}
				}
				break;
			}
			case 2: {
				//add current direction into direction_vector
				direction_vector.push_back(Direction::South);
				//y increases by 1, x doesn't change
				Point south_point(curr_point.getX(), curr_point.getY() + 1);
				//add point in this direction to vector of adjacent points
				adjacent_points.push_back(south_point);

				//add South point to map of points with routes to docking
				if (curr_point == docking) {
					stack<Direction> temp_stack;
					//add direction from current point to adjacent point into the stack
					temp_stack.push(Direction::South);
					//add new entry into the points_with_path_to_docking map
					points_with_path_to_docking.emplace(south_point, temp_stack);
				}
				else {
					stack<Direction> temp_stack = points_with_path_to_docking.find(curr_point)->second;
					//check length of the current path
					int new_length = temp_stack.size() + 1;
					//check length of the old path
					int old_length;
					//iterator for searching map
					std::map<Point, stack<Direction>>::iterator place_of_point;
					//check if South point is already added into points_with_path_to_docking map
					place_of_point = points_with_path_to_docking.find(south_point);
					//if point is found in a map
					if (place_of_point != points_with_path_to_docking.end()) {
						//get route from West point to docking
						stack<Direction> old_stack = points_with_path_to_docking.find(south_point)->second;
						old_length = old_stack.size();
						//only if new length is shorter - update the path
						if (old_length >  new_length) {
							//add direction from current point to adjacent point into the stack
							temp_stack.push(Direction::South);
							//remove old entry from map
							points_with_path_to_docking.erase(south_point);
							//add new entry into the points_with_path_to_docking map
							points_with_path_to_docking.emplace(south_point, temp_stack);
						}
					}
					//if point is not found in a map
					else {
						//add direction from current point to adjacent point into the stack
						temp_stack.push(Direction::South);
						//add new entry into the points_with_path_to_docking map
						points_with_path_to_docking.emplace(south_point, temp_stack);
					}
				}
				break;
			}
			case 3: {
				//add current direction into direction_vector
				direction_vector.push_back(Direction::North);
				//y decreases by 1, x doesn't change
				Point north_point(curr_point.getX(), curr_point.getY() - 1);
				//add point in this direction to vector of adjacent points
				adjacent_points.push_back(north_point);

				//add North point to map of points with routes to docking
				if (curr_point == docking) {
					stack<Direction> temp_stack;
					//add direction from current point to adjacent point into the stack
					temp_stack.push(Direction::North);
					//add new entry into the points_with_path_to_docking map
					points_with_path_to_docking.emplace(north_point, temp_stack);
				}
				else {
					stack<Direction> temp_stack = points_with_path_to_docking.find(curr_point)->second;
					//check length of the current path
					int new_length = temp_stack.size() + 1;
					//check length of the old path
					int old_length;
					//iterator for searching map
					std::map<Point, stack<Direction>>::iterator place_of_point;
					//check if North point is already added into points_with_path_to_docking map
					place_of_point = points_with_path_to_docking.find(north_point);
					//if point is found in a map
					if (place_of_point != points_with_path_to_docking.end()) {
						//get route from North point to docking
						stack<Direction> old_stack = points_with_path_to_docking.find(north_point)->second;
						old_length = old_stack.size();
						//only if new length is shorter - update the path
						if (old_length >  new_length) {
							//add direction from current point to adjacent point into the stack
							temp_stack.push(Direction::North);
							//remove old entry from map
							points_with_path_to_docking.erase(north_point);
							//add new entry into the points_with_path_to_docking map
							points_with_path_to_docking.emplace(north_point, temp_stack);
						}
					}
					//if point is not found in a map
					else {
						//add direction from current point to adjacent point into the stack
						temp_stack.push(Direction::North);
						//add new entry into the points_with_path_to_docking map
						points_with_path_to_docking.emplace(north_point, temp_stack);
					}
				}
				break;
			}
			default: {
			}
			}
		}
	}
	//Stay can be always a choice - added into direction_vector
	direction_vector.push_back(Direction::Stay);
}

//get new direction from current point in order of priorities
void SmartAlgorithm::getNewDirection(SensorInformation sensor_information, vector<Direction>& available_directions) {
	//default direction is Stay
	choice = Direction::Stay;
	std::vector<Point>::iterator it; //iterator for checking clean points
	//EAST=>NORTH=>WEST=>SOUTH
	//check if appropriate direction is available
	//check from lowest priority to highest - chosen direction will be with highest priority
	//check there is no wall in desired direction and dirt level at current point is 0
	//no wall at South
	if (!sensor_information.isWall[2]) {
		//check if point to step in is not visited already
		//y increases by 1, x doesn't change
		Point south_point(curr_point.getX(), curr_point.getY() + 1);
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), south_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::South;
			//add current direction to list of available from current point
			available_directions.push_back(Direction::South);
		}
	}
	//no wall at West
	if (!sensor_information.isWall[1]) {
		//check if point to step in is not visited already
		//x decreases by 1, y doesn't change
		Point west_point(curr_point.getX() - 1, curr_point.getY());
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), west_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::West;
			//add current direction to list of available from current point
			available_directions.push_back(Direction::West);
		}
	}
	//no wall at North
	if (!sensor_information.isWall[3]) {
		//check if point to step in is not visited already
		//y decreases by 1, x doesn't change
		Point north_point(curr_point.getX(), curr_point.getY() - 1);
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), north_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::North;
			//add current direction to list of available from current point
			available_directions.push_back(Direction::North);
		}
	}
	if (!sensor_information.isWall[0]) {
		//check if point to step in is not visited already
		//x increases by 1, y doesn't change
		Point east_point(curr_point.getX() + 1, curr_point.getY());
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), east_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::East;
			//add current direction to list of available from current point
			available_directions.push_back(Direction::East);
		}
	}
	//stay till dust level is not 0
	if (sensor_information.dirtLevel != 0) {
		choice = Direction::Stay;
	}
}

//get new direction from current point in order of priorities
void SmartAlgorithm::getNewDirection2(SensorInformation sensor_information, vector<Direction>& available_unvisited_directions) {
	//default direction
	choice = Direction::Stay;
	//iterator for checking clean points
	std::vector<Point>::iterator it;
	//EAST=>WEST=>NORTH=>SOUTH
	//check if appropriate direction is available
	//check from lowest priority to highest - chosen direction will be with highest priority
	//check there is no wall in desired direction and dirt level at current point is 0
	//no wall at South
	if (!sensor_information.isWall[2]) {
		//check if point to step in is not visited already
		//y increases by 1, x doesn't change
		Point south_point(curr_point.getX(), curr_point.getY() + 1);
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), south_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::South;
			//add current direction to list of available from current point
			available_unvisited_directions.push_back(Direction::South);
		}
	}
	//no wall at North
	if (!sensor_information.isWall[3]) {
		//check if point to step in is not visited already
		//y decreases by 1, x doesn't change
		Point north_point(curr_point.getX(), curr_point.getY() - 1);
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), north_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::North;
			//add current direction to list of available from current point
			available_unvisited_directions.push_back(Direction::North);
		}
	}
	//no wall at West
	if (!sensor_information.isWall[1]) {
		//check if point to step in is not visited already
		//x decreases by 1, y doesn't change
		Point west_point(curr_point.getX() - 1, curr_point.getY());
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), west_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::West;
			//add current direction to list of available from current point
			available_unvisited_directions.push_back(Direction::West);
		}
	}
	//no wall at East
	if (!sensor_information.isWall[0]) {
		//check if point to step in is not visited already
		//x increases by 1, y doesn't change
		Point east_point(curr_point.getX() + 1, curr_point.getY());
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), east_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::East;
			//add current direction to list of available from current point
			available_unvisited_directions.push_back(Direction::East);
		}
	}
	//stay till dust level is not 0
	if (sensor_information.dirtLevel != 0) {
		choice = Direction::Stay;
	}
}

//get new direction from current point in order of priorities
void SmartAlgorithm::getNewDirection3(SensorInformation sensor_information, vector<Direction>& available_unvisited_directions) {
	//default direction
	choice = Direction::Stay;
	//iterator for checking clean points
	std::vector<Point>::iterator it;
	//NORTH=>EAST=>WEST=>SOUTH
	//check if appropriate direction is available
	//check from lowest priority to highest - chosen direction will be with highest priority
	//check there is no wall in desired direction and dirt level at current point is 0
	//no wall at South
	if (!sensor_information.isWall[2]) {
		//check if point to step in is not visited already
		//y increases by 1, x doesn't change
		Point south_point(curr_point.getX(), curr_point.getY() + 1);
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), south_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::South;
			//add current direction to list of available from current point
			available_unvisited_directions.push_back(Direction::South);
		}
	}
	//no wall at West
	if (!sensor_information.isWall[1]) {
		//check if point to step in is not visited already
		//x decreases by 1, y doesn't change
		Point west_point(curr_point.getX() - 1, curr_point.getY());
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), west_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::West;
			//add current direction to list of available from current point
			available_unvisited_directions.push_back(Direction::West);
		}
	}
	//no wall at East
	if (!sensor_information.isWall[0]) {
		//check if point to step in is not visited already
		//x increases by 1, y doesn't change
		Point east_point(curr_point.getX() + 1, curr_point.getY());
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), east_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::East;
			//add current direction to list of available from current point
			available_unvisited_directions.push_back(Direction::East);
		}
	}
	//no wall at North
	if (!sensor_information.isWall[3]) {
		//check if point to step in is not visited already
		//y decreases by 1, x doesn't change
		Point north_point(curr_point.getX(), curr_point.getY() - 1);
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), north_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::North;
			//add current direction to list of available from current point
			available_unvisited_directions.push_back(Direction::North);
		}
	}
	//stay till dust level is not 0
	if (sensor_information.dirtLevel != 0) {
		choice = Direction::Stay;
	}
}

//get new direction from current point in order of priorities
void SmartAlgorithm::getNewDirection4(SensorInformation sensor_information, vector<Direction>& available_unvisited_directions) {
	//default direction
	choice = Direction::Stay;
	//iterator for checking clean points
	std::vector<Point>::iterator it;
	//NORTH=>SOUTH=>EAST=>WEST
	//check if appropriate direction is available
	//check from lowest priority to highest - chosen direction will be with highest priority
	//check there is no wall in desired direction and dirt level at current point is 0
	//no wall at South
	if (!sensor_information.isWall[2]) {
		//check if point to step in is not visited already
		//y increases by 1, x doesn't change
		Point south_point(curr_point.getX(), curr_point.getY() + 1);
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), south_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::South;
			//add current direction to list of available from current point
			available_unvisited_directions.push_back(Direction::South);
		}
	}
	//no wall at West
	if (!sensor_information.isWall[1]) {
		//check if point to step in is not visited already
		//x decreases by 1, y doesn't change
		Point west_point(curr_point.getX() - 1, curr_point.getY());
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), west_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::West;
			//add current direction to list of available from current point
			available_unvisited_directions.push_back(Direction::West);
		}
	}
	//no wall at East
	if (!sensor_information.isWall[0]) {
		//check if point to step in is not visited already
		//x increases by 1, y doesn't change
		Point east_point(curr_point.getX() + 1, curr_point.getY());
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), east_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::East;
			//add current direction to list of available from current point
			available_unvisited_directions.push_back(Direction::East);
		}
	}
	//no wall at South
	if (!sensor_information.isWall[2]) {
		//check if point to step in is not visited already
		//y increases by 1, x doesn't change
		Point south_point(curr_point.getX(), curr_point.getY() + 1);
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), south_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::South;
			//add current direction to list of available from current point
			available_unvisited_directions.push_back(Direction::South);
		}
	}
	//no wall at North
	if (!sensor_information.isWall[3]) {
		//check if point to step in is not visited already
		//y decreases by 1, x doesn't change
		Point north_point(curr_point.getX(), curr_point.getY() - 1);
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), north_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::North;
			//add current direction to list of available from current point
			available_unvisited_directions.push_back(Direction::North);
		}
	}
	//stay till dust level is not 0
	if (sensor_information.dirtLevel != 0) {
		choice = Direction::Stay;
	}
}

//add current point and directions from it to stack of unexplored points and directions
void SmartAlgorithm::addPointToUnexplored(Direction dir, const vector<Direction>& available_directions) {
	//if East chosen
	if (dir == Direction::East) {
		//if there is any additional directions except chosen
		if (available_directions.size() > 1) {
			//add current point and chosen direction to stack of unexplored points
			unexplored_points.emplace(curr_point, Direction::East);
		}
	}
	//if West chosen
	if (dir == Direction::West) {
		//if there is any additional directions except chosen
		if (available_directions.size() > 1) {
			//add current point and chosen direction to stack of unexplored points
			unexplored_points.emplace(curr_point, Direction::West);
		}
	}
	//if South chosen
	if (dir == Direction::South) {
		//if there is any additional directions except chosen
		if (available_directions.size() > 1) {
			//add current point and chosen direction to stack of unexplored points
			//			unexplored_points.emplace(curr_point, Direction::South);
		}
	}
	//if North chosen
	if (dir == Direction::North) {
		//if there is any additional directions except chosen
		if (available_directions.size() > 1) {
			//add current point and chosen direction to stack of unexplored points
			unexplored_points.emplace(curr_point, Direction::North);
		}
	}
}

//update stacks
void SmartAlgorithm::updateStacks(Direction prevStep, Direction dir) {
	//rearrange stack accordingly to actual previous step taken by simulation
	//if previous step was taken - no rearrangement is needed
	if (prev_step == prevStep) {
		//add chosen direction to stack route
		route.push(dir);
		//add chosen direction to stack route_to_docking
		route_to_docking.push(dir);
	}
	//otherwise - remove previous step from stack and insert actually taken step
	else {
		//remove previous step
		route.pop();
		//insert previous step taken by simulation
		route.push(prevStep);
		//insert current step taken by algorithm
		route.push(dir);
	}
}

//chose next unexplored direction
void SmartAlgorithm::getNewUnexploredDirection(SensorInformation sensor_information, Direction prevStep, vector<Direction>& available_directions) {
	std::vector<Point>::iterator it;
	//EAST=>WEST=>NORTH=>SOUTH
	//chose next unchosen direction
	//select next direction
	if (last_choice == Direction::East && !sensor_information.isWall[1]) {
		//check if point to step in is not visited already
		//x decreases by 1, y doesn't change
		Point west_point(curr_point.getX() - 1, curr_point.getY());
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), west_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::West;
			//add current direction to list of available from current point
			available_directions.push_back(Direction::West);
		}
	}
	if (last_choice == Direction::West && !sensor_information.isWall[3]) {
		//check if point to step in is not visited already
		//y decreases by 1, x doesn't change
		Point north_point(curr_point.getX(), curr_point.getY() - 1);
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), north_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::North;
			//add current direction to list of available from current point
			available_directions.push_back(Direction::North);
		}
	}
	if (last_choice == Direction::North && !sensor_information.isWall[2]) {
		//check if point to step in is not visited already
		//y increases by 1, x doesn't change
		Point south_point(curr_point.getX(), curr_point.getY() + 1);
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), south_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::South;
			//add current direction to list of available from current point
			available_directions.push_back(Direction::South);
		}
	}
	if (choice != Direction::Stay) {
		//rearrange stack accordingly to actual previous step taken by simulation
		//if previous step was taken - no rearrangement is needed
		updateStacks(prevStep, choice);
		//update distance to docking
		steps_to_docking++;
	}
	//if last choice chosen - remove from unexplored points stack
	if (choice == Direction::South || available_directions.size() == 1) {
		//remove it from stack
		unexplored_points.pop();
	}
}

//chose next unexplored direction
void SmartAlgorithm::getNewUnexploredDirection2(SensorInformation sensor_information, Direction prevStep, vector<Direction>& available_directions) {
	std::vector<Point>::iterator it; //iterator for checking clean points
	//	choice = Direction::Stay;
	//EAST=>NORTH=>WEST=>SOUTH
	//chose next unchosen direction
	//select next direction
	if (last_choice == Direction::East && !sensor_information.isWall[3]) {
		//check if point to step in is not visited already
		//y decreases by 1, x doesn't change
		Point north_point(curr_point.getX(), curr_point.getY() - 1);
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), north_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::North;
			//add current direction to list of available from current point
			available_directions.push_back(Direction::North);
		}
	}
	if (last_choice == Direction::North && !sensor_information.isWall[1]) {
		//check if point to step in is not visited already
		//x decreases by 1, y doesn't change
		Point west_point(curr_point.getX() - 1, curr_point.getY());
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), west_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::West;
			//add current direction to list of available from current point
			available_directions.push_back(Direction::West);
		}
	}
	if (last_choice == Direction::West && !sensor_information.isWall[2]) {
		//check if point to step in is not visited already
		//y increases by 1, x doesn't change
		Point south_point(curr_point.getX(), curr_point.getY() + 1);
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), south_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::South;
			//add current direction to list of available from current point
			available_directions.push_back(Direction::South);
		}
	}
	if (choice != Direction::Stay) {
		//rearrange stack accordingly to actual previous step taken by simulation
		//if previous step was taken - no rearrangement is needed
		updateStacks(prevStep, choice);
		//update distance to docking
		steps_to_docking++;
	}
	//if last choice chosen - remove from unexplored points stack
	if (choice == Direction::South || available_directions.size() == 1) {
		//remove it from stack
		unexplored_points.pop();
	}
}

void SmartAlgorithm::getNewUnexploredDirection3(SensorInformation sensor_information, Direction prevStep, vector<Direction>& available_directions) {
	std::vector<Point>::iterator it; //iterator for checking clean points
	//	choice = Direction::Stay;
	//NORTH=>EAST=>WEST=>SOUTH
	//chose next unchosen direction
	//select next direction
	if (last_choice == Direction::North && !sensor_information.isWall[0]) {
		//check if point to step in is not visited already
		//x increases by 1, y doesn't change
		Point east_point(curr_point.getX() + 1, curr_point.getY());
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), east_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::East;
			//add current direction to list of available from current point
			available_directions.push_back(Direction::East);
		}
	}
	if (last_choice == Direction::East && !sensor_information.isWall[1]) {
		//check if point to step in is not visited already
		//x decreases by 1, y doesn't change
		Point west_point(curr_point.getX() - 1, curr_point.getY());
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), west_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::West;
			//add current direction to list of available from current point
			available_directions.push_back(Direction::West);
		}
	}
	if (last_choice == Direction::West && !sensor_information.isWall[2]) {
		//check if point to step in is not visited already
		//y increases by 1, x doesn't change
		Point south_point(curr_point.getX(), curr_point.getY() + 1);
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), south_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::South;
			//add current direction to list of available from current point
			available_directions.push_back(Direction::South);
		}
	}
	if (choice != Direction::Stay) {
		//rearrange stack accordingly to actual previous step taken by simulation
		//if previous step was taken - no rearrangement is needed
		updateStacks(prevStep, choice);
		//update distance to docking
		steps_to_docking++;
	}
	//if last choice chosen - remove from unexplored points stack
	if (choice == Direction::South || available_directions.size() == 1) {
		//remove it from stack
		unexplored_points.pop();
	}
}

//chose next unexplored direction
void SmartAlgorithm::getNewUnexploredDirection4(SensorInformation sensor_information, Direction prevStep, vector<Direction>& available_directions) {
	std::vector<Point>::iterator it;
	//WEST=>EAST=>NORTH=>SOUTH
	//chose next unchosen direction
	//select next direction
	if (last_choice == Direction::West && !sensor_information.isWall[1]) {
		//check if point to step in is not visited already
		//x increases by 1, y doesn't change
		Point east_point(curr_point.getX() + 1, curr_point.getY());
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), east_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::East;
			//add current direction to list of available from current point
			available_directions.push_back(Direction::East);
		}
	}
	if (last_choice == Direction::East && !sensor_information.isWall[3]) {
		//check if point to step in is not visited already
		//y decreases by 1, x doesn't change
		Point north_point(curr_point.getX(), curr_point.getY() - 1);
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), north_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::North;
			//add current direction to list of available from current point
			available_directions.push_back(Direction::North);
		}
	}
	if (last_choice == Direction::North && !sensor_information.isWall[2]) {
		//check if point to step in is not visited already
		//y increases by 1, x doesn't change
		Point south_point(curr_point.getX(), curr_point.getY() + 1);
		//try to find next point in vector of visited
		it = find(clean_points.begin(), clean_points.end(), south_point);
		//point NOT found - step into it
		if (it == clean_points.end()) {
			choice = Direction::South;
			//add current direction to list of available from current point
			available_directions.push_back(Direction::South);
		}
	}
	if (choice != Direction::Stay) {
		//rearrange stack accordingly to actual previous step taken by simulation
		//if previous step was taken - no rearrangement is needed
		updateStacks(prevStep, choice);
		//update distance to docking
		steps_to_docking++;
	}
	//if last choice chosen - remove from unexplored points stack
	if (choice == Direction::South || available_directions.size() == 1) {
		//remove it from stack
		unexplored_points.pop();
	}
}

bool SmartAlgorithm::updatePathToDocking(vector<Point> adjacent_points) {
	bool is_path_updated = false;
	//direction_vector now holds all possible direction - directions without walls
	//add pair of current point with its adjacent points into map
	adjacent.emplace(curr_point, adjacent_points);
	//check if there is an adjacent point to current point with route to docking shorter than from current point
	//length of current route to docking
//	cout << "Length of current route to docking is: " << route.size() << endl;
	int current_distance_to_docking;
	if (route_to_docking.empty()) {
		current_distance_to_docking = route.size();
	} else {
		current_distance_to_docking = route_to_docking.size(); //route.size();
	}
	//update route to docking from current point
	if (curr_point != docking) {
		//find shortest route from adjacent of current point to docking and update by adding additional step from current point to adjacent
		Point temp_point;
		//iterator for iteration through the points_with_path_to_docking map
		std::map<Point, stack<Direction> >::iterator place_of_point;
		//stack for route
		stack<Direction> route_stack;
		//size of route_stack
		int route_to_docking_length;
		//check East direction
		temp_point = curr_point;
		//in East direction X is increasing
		temp_point.increaseX();
		//check if East point is already added into points_with_path_to_docking map
		place_of_point = points_with_path_to_docking.find(temp_point);
		//if point is found in a map
		if (place_of_point != points_with_path_to_docking.end()) {
			//get route from East point to docking
			route_stack = points_with_path_to_docking.find(temp_point)->second;
			//get size of stack
			route_to_docking_length = route_stack.size();
			//check if route from adjacent point is shorter than from current point
			if (route_to_docking_length + 1 < current_distance_to_docking) {
				//add East direction to route_stack
				route_stack.push(Direction::West);
				//erase old route from current point to docking
				points_with_path_to_docking.erase(curr_point);
				//update route from current point to docking
				points_with_path_to_docking.emplace(curr_point, route_stack);
				//set route to docking be through adjacent point
				route_to_docking = route_stack;
				is_path_updated = true;
			}
		}
		if (route_to_docking.empty()) {
			current_distance_to_docking = route.size();
		} else {
			current_distance_to_docking = route_to_docking.size(); //route.size();
		}
		//check West direction
		temp_point = curr_point;
		//in West direction X is decreasing
		temp_point.decreaseX();
		//check if West point is already added into points_with_path_to_docking map
		place_of_point = points_with_path_to_docking.find(temp_point);
		//if point is found in a map
		if (place_of_point != points_with_path_to_docking.end()) {
			//get route from West point to docking
			route_stack = points_with_path_to_docking.find(temp_point)->second;
			//get size of stack
			route_to_docking_length = route_stack.size();
			//check if route from adjacent point is shorter than from current point
			if (route_to_docking_length + 1 < current_distance_to_docking) {
				//add West direction to route_stack
				route_stack.push(Direction::East);
				//erase old route from current point to docking
				points_with_path_to_docking.erase(curr_point);
				//update route from current point to docking
				points_with_path_to_docking.emplace(curr_point, route_stack);
				//set route to docking be through adjacent point
				route_to_docking = route_stack;
				is_path_updated = true;
			}
		}
		if (route_to_docking.empty()) {
			current_distance_to_docking = route.size();
		} else {
			current_distance_to_docking = route_to_docking.size(); //route.size();
		}
		//check South direction
		temp_point = curr_point;
		//in South direction Y is increasing
		temp_point.increaseY();
		//check if South point is already added into points_with_path_to_docking map
		place_of_point = points_with_path_to_docking.find(temp_point);
		//if point is found in a map
		if (place_of_point != points_with_path_to_docking.end()) {
			//get route from South point to docking
			route_stack = points_with_path_to_docking.find(temp_point)->second;
			//get size of stack
			route_to_docking_length = route_stack.size();
			//check if route from adjacent point is shorter than from current point
			if (route_to_docking_length + 1 < current_distance_to_docking) {
				//add South direction to route_stack
				route_stack.push(Direction::North);
				//erase old route from current point to docking
				points_with_path_to_docking.erase(curr_point);
				//update route from current point to docking
				points_with_path_to_docking.emplace(curr_point, route_stack);
				//set route to docking be through adjacent point
				route_to_docking = route_stack;
				is_path_updated = true;
			}
		}
		if (route_to_docking.empty()) {
			current_distance_to_docking = route.size();
		} else {
			current_distance_to_docking = route_to_docking.size();
		}
		//check North direction
		temp_point = curr_point;
		//in North direction Y is decreasing
		temp_point.decreaseY();
		//check if North point is already added into points_with_path_to_docking map
		place_of_point = points_with_path_to_docking.find(temp_point);
		//if point is found in a map
		if (place_of_point != points_with_path_to_docking.end()) {
			//get route from North point to docking
			route_stack = points_with_path_to_docking.find(temp_point)->second;
			//get size of stack
			route_to_docking_length = route_stack.size();
			//check if route from adjacent point is shorter than from current point
			if (route_to_docking_length + 1 < current_distance_to_docking) {
				//add North direction to route_stack
				route_stack.push(Direction::South);
				//erase old route from current point to docking
				points_with_path_to_docking.erase(curr_point);
				//update route from current point to docking
				points_with_path_to_docking.emplace(curr_point, route_stack);
				//set route to docking be through adjacent point
				route_to_docking = route_stack;
				is_path_updated = true;
			}
		}
		if (route_to_docking.empty()) {
			current_distance_to_docking = route.size();
		} else {
			current_distance_to_docking = route_to_docking.size(); //route.size();
		}
		/*DEBUG
		cout << "Length of NEW current_distance_to_docking is: " << current_distance_to_docking << endl;
		cout << "Current point is: " << curr_point.toString() << endl;
		if (!route_to_docking.empty()) {
			cout << "Route to docking is:" << endl;
			stack<Direction> temp = route_to_docking;
			int i = 0;
			while (!temp.empty()) {
				cout << "step #" << i << " direction is: " << (int) (temp.top()) << endl;
				temp.pop();
				i++;
			}
		}
		*/
	}
	return is_path_updated;
}

//fix Simulation steps
Direction SmartAlgorithm::returnToRoute(Direction prevStep) {
	//step will remain unchanged if prev_step of an algorithm was taken by Simulation
	Direction step = Direction::Stay;
	//if Simulation performed different step - return to previous point
	if (prevStep != prev_step) {
		//if Simulation step was Stay - algorithm is on its route and there is no need to return to it
		if (prevStep == Direction::Stay) {
			//set prev_step to algorithm_step it was supposed to perform before interference started
			prev_step = algorithm_step;
			//take again the step that was chosen by algorithm before Simulation's interruption
			step = algorithm_step;
		}
		//if Simulation step wasn't Stay - algorithm is off its route and there is a need to return to it
		else {
			//add actual step performed by Simulation to stack
			simulation_interference.push(prevStep);
		}
	}
	//no interfered steps remained in the stack
	if (simulation_interference.empty()) {
		//algorithm is on its route
		is_back_on_route = true;
	}
	//algorithm still off the route
	else {
		//algorithm is not on its route
		is_back_on_route = false;
	}
	//if previous step wasn't accepted - algorithm is not on its route - try to return there
	if (!is_back_on_route) {
		//decrease current battery level by battery consumption
		decreaseBatteryLevel();
		//if its time to finish - decrease steps_till_finishing
		decreaseStepsTillFinishing();

		//previous step of returning to route was accepted by Simulation
		if (prevStep == prev_step) {
			//remove top element from stack - this step is performed already
			simulation_interference.pop();

			//if there are any steps remained in the simulation_interference stack
			if (!simulation_interference.empty()) {
				//get next step from stack of steps performed by Simulation
				step = simulation_interference.top();
				//revert current direction
				revertDirection(step);

				//set prev_step to choice
				prev_step = choice;
				//try to go back
				step = choice;
			}
			//if no steps remained in the simulation_interference stack - algorithm is back on the route
			else {
				//algorithm returned to its route
				is_back_on_route = true;
				//set prev_step to algorithm_step it was supposed to perform before interference started
				prev_step = algorithm_step;
				//take again the step that was chosen by algorithm before Simulation's interruption
				step = algorithm_step;
			}
		}
		//previous step of returning to route wasn't accepted by Simulation
		else {
			//get last step performed by Simulation from stack
			step = simulation_interference.top();
			//revert received direction
			revertDirection(step);

			//set prev_step to choice - save current algorithm's step
			prev_step = choice;
			//try to return to route
			step = choice;
		}
	}
	//function will return Stay in case of no Simulation interference
	//it will try to return to route in case of Simulation interference
	return step;
}

// this method is called by the simulation either when there is a winner or
// when steps == MaxSteps - MaxStepsAfterWinner
// parameter stepsTillFinishing == MaxStepsAfterWinner
void SmartAlgorithm::aboutToFinish(int stepsTillFinishing) {
	//set steps_till_finishing to be number of steps received from simulation when winner is detected
	steps_till_finishing = stepsTillFinishing;
}
