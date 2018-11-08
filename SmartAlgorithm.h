#ifndef SmartAlgorithm_H
#define SmartAlgorithm_H
#include "Direction.h"

#include "AbstractAlgorithm.h"
#include "AbstractSensor.h"

#include "Utils.h"

#include "Point.h"

#include <stack>

// AlgorithmRegistration.h is part of BOTH the simulation project
// and the Algorithm projects
//#include "AlgorithmRegistration.h"

//for comparing points
struct PointComparator
{
    bool operator () (const Point &a, const Point &b)
    {
    	//if values are equal
    	if (a.getX() == b.getX()) {
    		//compare by keys
    		return a.getY() > b.getY();
    	}
    	//if values are different
    	else {
    		//compare by values
    		return a.getX() > b.getX();
    	}
    }
};


class SmartAlgorithm: public AbstractAlgorithm {
	AbstractSensor const *sensor;
	//map of parameters from configuration file
	map<string, int> configuration;
	//map of adjacent points:
	//key - point
	//value - vector of points that are adjacent to point
	map<Point, vector<Point>, PointComparator> adjacent;
	//map of adjacent points:
	//key - current point
	//value - stack of directions from the route to docking
	map<Point, stack<Direction>, PointComparator> points_with_path_to_docking;
	//save docking coordinates
	Point docking;
	//vector of clean points in house - already visited
	vector<Point> clean_points;
	//stack of points from where exist possibility of additional directions to continue
	//store point and last direction chosen
	//when returning to this point - chose next available direction
	//until all direction are explored
	//when chosen direction is 'South' - the point is explored
	stack<pair<Point, Direction>> unexplored_points;
	//route - stack of points from docking to current point
	stack<Direction> route;
	//current location of a robot
	Point curr_point;
	//point to return to
	Point dest_point;
	//step of a robot
	Direction choice;
	//last chosen direction from unexplored point
	Direction last_choice;
	//for modes of cleaning: 0 - exploring, 1 - returning to closest unexplored point
	int mode;
	//previous mode
	int starting_mode;
	//route to docking - stack of points from docking to current point
	//should be negated to return to docking
	stack<Direction> route_to_docking;
	//route from docking - stack of points from docking to current point
	//no negation needed to return to current point
	stack<Direction> route_from_docking;
	//no negation needed to return to current point if simulation is taking different steps
	stack<Direction> simulation_interference;
	//battery level
	int current_battery;
	//distance from current point to docking
	int steps_to_docking;
	//point from which robot is returning to docking
	Point point_to_return;
	//number of algorithm's step
	int current_step;
	//is returning to docking mode started?
	bool is_returning_to_docking;
	//will be not -1 when there is a winner
	int steps_till_finishing;
	//save previous step of algorithm
	Direction prev_step;
	//distance from docking
	int steps_from_docking;
	//step chosen by algorithm before and wasn't performed by Simulation
	Direction algorithm_step;
	//check if chosen step was performed
	bool is_back_on_route;


public:
	//enum for modes
	enum {EXPLORING, RETURN_TO_UNEXPLORED, RETURN_TO_DOCKING, RETURN_FROM_DOCKING};

	//setters and getters
	SensorInformation getSensorInformation() const {
		return this->sensor->sense();
	}

	map<string, int> getConfiguration() const {
		return this->configuration;
	}

	map<Point, vector<Point>, PointComparator> getAdjacent() const {
		return this->adjacent;
	}
	void setAdjacent(map<Point, vector<Point>, PointComparator>& adjacent) {
		this->adjacent = adjacent;
	}
	void adjacentEmplace(Point point, vector<Point>& points) {
		this->adjacent.emplace(point, points);
	}

	map<Point, stack<Direction>, PointComparator> getPointsWithPathToDocking() const {
		return this->points_with_path_to_docking;
	}
	void setPointsWithPathToDocking(map<Point, stack<Direction>, PointComparator>& points_with_path_to_docking) {
		this->points_with_path_to_docking = points_with_path_to_docking;
	}

	Point getDocking() const {
		return this->docking;
	}
	void setDocking(Point& docking) {
		this->docking = docking;
	}

	vector<Point> getCleanPoints() const {
		return this->clean_points;
	}
	void setCleanPoints(vector<Point>& clean_points) {
		this->clean_points = clean_points;
	}
	void cleanPointsPushBack(Point clean_point) {
		this->clean_points.push_back(clean_point);
	}

	stack<pair<Point, Direction>> getUnexploredPoints() const {
		return this->unexplored_points;
	}
	void setUnexploredPoints(stack<pair<Point, Direction>>& unexplored_points) {
		this->unexplored_points = unexplored_points;
	}
	pair<Point, Direction> unexploredPointsTop() const {
		return this->unexplored_points.top();
	}
	void unexploredPointsPop() {
		this->unexplored_points.pop();
	}
	bool unexploredPointsEmpty() {
		return this->unexplored_points.empty();
	}
	void unexploredPointsEmplace(Point point, Direction direction) {
		unexplored_points.emplace(point, direction);
	}


	stack<Direction> getRoute() const {
		return this->route;
	}
	void setRoute(stack<Direction>& route){
		this->route = route;
	}
	void routePush(Direction direction) {
		this->route.push(direction);
	}

	Point getCurrPoint() const {
		return this->curr_point;
	}
	void setCurrPoint(Point& curr_point) {
		this->curr_point = curr_point;
	}

	Point getDestPoint() const {
		return this->dest_point;
	}
	void setDestPoint(Point dest_point) {
		this->dest_point = dest_point;
	}

	Direction getChoice() const {
		return this->choice;
	}
	void setChoice(Direction& choice) {
		this->choice = choice;
	}

	Direction getLastChoice() const {
		return this->last_choice;
	}
	void setLastChoice(Direction last_choice) {
		this->last_choice = last_choice;
	}

	int getMode() const {
		return this->mode;
	}
	void setMode(int mode) {
		this->mode = mode;
	}

	int getStartingMode() const {
		return this->mode;
	}
	void setStartingMode(int mode) {
		this->mode = mode;
	}

	stack<Direction> getRouteToDocking() const {
		return this->route_to_docking;
	}
	void setRouteToDocking(stack<Direction>& route_to_docking){
		this->route_to_docking = route_to_docking;
	}
	int getRouteToDockingSize() const {
		return this->route_to_docking.size();
	}
	void routeToDockingPush(Direction direction) {
		this->route_to_docking.push(direction);
	}

	stack<Direction> getRouteFromDocking() const {
		return this->route_from_docking;
	}
	void setRouteFromDocking(stack<Direction>& route_from_docking){
		this->route_from_docking = route_from_docking;
	}
	void routeFromDockingPush(Direction direction) {
		this->route_from_docking.push(direction);
	}

	stack<Direction> getSimulationInterference() const {
		return this->simulation_interference;
	}
	void setSimulationInterference(stack<Direction>& simulation_interference){
		this->simulation_interference = simulation_interference;
	}

	int getCurrentBattery() const {
		return this->current_battery;
	}
	void setCurrentBattery(int current_battery) {
		this->current_battery = current_battery;
	}

	int getStepsToDocking() const {
		return this->steps_to_docking;
	}
	void setStepsToDocking(int steps_to_docking) {
		this->steps_to_docking = steps_to_docking;
	}
	void increaseStepsToDocking() {
		this->steps_to_docking++;
	}
	void decreaseStepsToDocking() {
		this->steps_to_docking--;
	}

	Point getPointToReturn() const {
		return this->point_to_return;
	}
	void setPointToReturn(Point point_to_return) {
		this->point_to_return = point_to_return;
	}

	int getCurrentStep() const {
		return this->current_step;
	}
	void setCurrentStep(int current_step) {
		this->current_step = current_step;
	}

	void increaseCurrentStep() {
		this->current_step++;
	}

	bool getIsReturningToDocking() const {
		return this->is_returning_to_docking;
	}
	void setIsReturningToDocking(bool is_returning_to_docking) {
		this->is_returning_to_docking = is_returning_to_docking;
	}

	int getStepsTillFinishing() const {
		return this->steps_till_finishing;
	}
	void setStepsTillFinishing(int steps_till_finishing) {
		this->steps_till_finishing = steps_till_finishing;
	}

	Direction getPrevStep() const {
		return this->prev_step;
	}
	void setPrevStep(Direction prev_step) {
		this->prev_step = prev_step;
	}

	int getStepsFromDocking() const {
		return this->steps_from_docking;
	}
	void setStepsFromDocking(int steps_from_docking) {
		this->steps_from_docking = steps_from_docking;
	}

	Direction getAlgorithmStep() const {
		return this->algorithm_step;
	}
	void setAlgorithmStep(Direction algorithm_step) {
		this->algorithm_step = algorithm_step;
	}

	bool getIsBackOnRoute() const {
		return this->is_back_on_route;
	}
	void setIsBackOnRoute(bool is_back_on_route) {
		this->is_back_on_route = is_back_on_route;
	}

	int getBatteryCapacity() {
		return this->configuration["BatteryCapacity"];
	}
	int getBatteryRechargeRate() {
		return this->configuration["BatteryRechargeRate"];
	}
	int getBatteryConsumptionRate() {
		return this->configuration["BatteryConsumptionRate"];
	}


	//revert current direction
	void revertDirection(Direction current_direction);
	void revertDirection();

	//update current point accordingly to chosen direction
	void updateCurrentPoint(Direction chosen_direction);

	//update current point accordingly to actually taken step by Simulation
	void fixCurrentPoint(Direction prevStep);

	//decrease current battery level by battery consumption
	void decreaseBatteryLevel();

	//if its time to finish - decrease steps_till_finishing
	void decreaseStepsTillFinishing();

	//get return_direction_to_docking from stack
	Direction getReturnToDockingDirection(Direction return_direction_to_docking);

	//get return_direction_from_docking from stack
	Direction getReturnFromDockingDirection(Direction return_direction_from_docking);

	//get return_direction_to_unexplored from stack
	Direction getReturnToUnexploredDirection(Direction return_direction_to_unexplored);
	void getReturnToUnexploredDirection();

	//check surrounding walls and adjacent points
	void checkWalls(SensorInformation sensor_information, vector<Direction>& direction_vector, vector<Point>& adjacent_points);

	//get new direction from current point in order of priorities
	void getNewDirection(SensorInformation sensor_information, vector<Direction>& available_directions);
	void getNewDirection2(SensorInformation sensor_information, vector<Direction>& available_directions);
	void getNewDirection3(SensorInformation sensor_information, vector<Direction>& available_unvisited_directions);
	void getNewDirection4(SensorInformation sensor_information, vector<Direction>& available_unvisited_directions);

	//add current point and directions from it to stack of unexplored points and directions
	void addPointToUnexplored(Direction dir, const vector<Direction>& available_directions);

	//update stacks
	void updateStacks(Direction prevStep, Direction dir);

	//chose next unexplored direction
	void getNewUnexploredDirection(SensorInformation sensor_information, Direction prevStep, vector<Direction>& available_directions);
	void getNewUnexploredDirection2(SensorInformation sensor_information, Direction prevStep, vector<Direction>& available_directions);
	void getNewUnexploredDirection3(SensorInformation sensor_information, Direction prevStep, vector<Direction>& available_directions);
	void getNewUnexploredDirection4(SensorInformation sensor_information, Direction prevStep, vector<Direction>& available_directions);


	//try to find shorter path to docking
	bool updatePathToDocking(vector<Point> adjacent_points);

	//fix Simulation steps
	Direction returnToRoute(Direction prevStep);

	//constructor
	SmartAlgorithm();
	// setSensor is called once when the Algorithm is initialized
	virtual void setSensor(const AbstractSensor& sensor) override;
	// setConfiguration is called once when the Algorithm is initialized - see below
	virtual void setConfiguration(map<string, int> config) override;
	// this method is called by the simulation either when there is a winner or
	// when steps == MaxSteps - MaxStepsAfterWinner
	// parameter stepsTillFinishing == MaxStepsAfterWinner
	virtual void aboutToFinish(int stepsTillFinishing) override;
	//destructor
	virtual ~SmartAlgorithm() override {};
};

#endif /* _SmartAlgorithm_H_ */
