#ifndef RUNRESULT_H_
#define RUNRESULT_H_

#include "Direction.h"
#include "Utils.h"

class RunResult {
	//gives friend access to Simulation
	friend class Simulation;
	int simulation_steps;
	int actual_position_in_competition;
	int position_in_competition;
	int winner_id;
	int winner_num_steps;
	int this_num_steps;
	int sum_dirt_in_house;
	int dirt_collected;
	int current_battery_capacity;
	int times_at_docking;
	int step_of_wall_encountering;
	int house_number_of_wall_encountering;
	bool previous_step_at_docking;
	bool is_back_in_docking;
	bool is_wall_encountered;
	bool is_battery_emptied;
	bool is_finished_successfully;
	bool is_still_in_competition;
	bool is_first_step;
	Direction previous_step;

public:
	//constructor
	RunResult(int winner_num_steps, int sum_dirt_in_house, int current_battery_capacity);
	//default score function
	static int calc_score(const map<string, int>& score_params);
	//score calculation
	int const calculateScore();
	//for printing result's data
	string const toString() const;
	//destructor
	~RunResult() {};
};

#endif /* RUNRESULT_H_ */
