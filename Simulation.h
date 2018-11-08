#ifndef __SIMULATION__H_
#define __SIMULATION__H_

#include "Direction.h"

#include "AbstractAlgorithm.h"
#include "AbstractSensor.h"
#include "Configuration.h"
#include "Sensor.h"
#include "Point.h"
#include "House.h"
#include "RunResult.h"
#include "Video.h"
#include "Utils.h"

// AlgorithmRegistration.h is part of BOTH the simulation project
// and the Algorithm projects
#include "AlgorithmRegistration.h"
// AlgorithmRegistrar.h is part of the simulation project ONLY
#include "AlgorithmRegistrar.h"

#include "MakeUnique.h"

#include "CalcScore.h"

//for comparing pairs
struct PairComparator
{
    bool operator () (const pair<string, double> &a, const pair<string, double> &b)
    {
    	//if values are equal
    	if (a.second == b.second) {
    		//compare by keys
    		return a.first < b.first;
    	}
    	//if values are different
    	else {
    		//compare by values
    		return a.second > b.second;
    	}
    }
};

class Simulation {
	//vector of algorithms
	vector<unique_ptr<AbstractAlgorithm>> algorithms_unique;
	//vector of houses to run algorithms on them
	vector<House> houses;
	//vector of error messages of wall encountered algorithms
	vector<string> errors_during_the_simulation;
	//map for config.ini
	map<string, int> params;
	//pointer to score formula
	calc_score_function score_formula_pointer;
	//enable/disable video option
	bool is_video_enabled;

	//simulation parameters
	size_t max_steps_after_winner;
	int max_battery_capacity;
	int battery_consumption_rate;
	int battery_recharge_rate;

	//multi threaded parameters
	//index of current house to run algorithms on
	atomic_size_t house_index{0}; // atomic_size_t is a language typedef for std::atomic<size_t>
	//vector of results of runs
    vector<int> final_results;
    //vector of videos for each algorithm of each house
    vector<Video> videos;

    //set configuration
	void setParams(const Configuration& config);
	//calculate collected dirt
	static int collectDirtAtCurrentLocation(House& house, const Point& curr_point);
	//make step
	static bool performStep(const Direction& step, const SensorInformation& current_sensor_info, Point& curr_point);
	//initialize data for each algorithm on current house
	void initAlgorithmsData(const House& house, vector<unique_ptr<AbstractAlgorithm>>& algorithms_unique,
			const size_t max_steps_on_house, Point& docking, vector<RunResult>& algorithm_data,
			vector<Point>& current_points, vector<House>& houses, vector<Sensor>& sensors);
	//initialize video
	void initVideo(vector<House>& houses, const vector<string>& algorithms_file_names, const vector<string>& houses_file_names, const int index);
	//check if any algorithm still in competition
	static bool checkAlgorithms(const vector<RunResult>& algoritms_in_competition);
	//iterate over the points of finishing of each algorithm and check if back at docking
	static void checkIfBackAtDocking(const vector<Point>& current_points, const Point& docking, vector<RunResult>& algorithm_data);
	//calculate actual_position_in_copmetition for each algorithm
	void calculateActualPosition(vector<RunResult>& algorithm_data,	const size_t max_steps_on_house);
	//set actual_position_in_competition for each algorithm
	void setPosition(vector<RunResult>& algorithm_data);
	//iterate over the algorithms after simulation finishes and update necessary data
	void setPostRunData(const vector<House>& houses, vector<RunResult>& algorithm_data, const int steps_performed, const int winner_num_steps);
	//encode images into video
	int encodeImagesIntoVideo(const string& algorithm_name, const string& house_name);
	//create map for score calculation
	map<string, int> createScoreParameters(const vector<RunResult>& algorithm_data, const size_t algorithm_number);
	//calculate final results
	void calculateResults(vector<RunResult>& algorithm_data, const vector<string>& algorithms_file_names,
			const vector<string>& houses_file_names, /*vector<int>& final_results,*/ const size_t& index);
	//create final results table
	void createFinalResultsTable(const vector<string>& algorithms_file_names, const vector<string>& houses_file_names);
	//run sub-simulation for one thread
	void runSingleSubSimulationThread(const vector<string>& algorithms_file_names, const vector<string>& houses_file_names);
	//run all algorithms on current house and calculate the results
	void runAlgorithmsOnHouse(House& house, const vector<string>& algorithms_file_names, const vector<string>& houses_file_names,const size_t& index);
	//make one move of the algorithm, return 1 if wall encountered
	int const makeSingleMove(House& house, unique_ptr<AbstractAlgorithm>& algorithm, const Point& docking, Point& curr_point,
			Sensor& sensor, RunResult& algorithm_data, int& winner_num_steps, Video& video);

public:
	//initialize simulation parameters
	Simulation(const Configuration& configuration);
	//set houses from vector
	void setHouses(const vector<House>& houses);
	//set algorithms from registrar
	void setAlgorithms(const AlgorithmRegistrar& registrar);
	//set score formula
	void setScoreFormula(const calc_score_function& score_formula_pointer);
	//set video using boolean variable
	void setVideo(const bool& is_video_enabled);
	//run multithreaded and return simulation errors
	vector<string> const runMultiThreaded(size_t num_threads, const vector<string>& algorithms_file_names,
			const vector<string>& houses_file_names);
	//destructor
	~Simulation(){};
};

#endif /* __SIMULATION__H_ */
