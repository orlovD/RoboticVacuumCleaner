#include "RunResult.h"

#include "Utils.h"

//constructor
RunResult::RunResult(int winner_num_steps, int sum_dirt_in_house, int current_battery_capacity) :
	simulation_steps(0), actual_position_in_competition(0), position_in_competition(0), winner_id(-1),
	this_num_steps(0), dirt_collected(0), times_at_docking(0), step_of_wall_encountering(-1),
	house_number_of_wall_encountering(-1), previous_step_at_docking(false),
	is_back_in_docking(false), is_wall_encountered(false), is_battery_emptied(false),
	is_finished_successfully(false), is_still_in_competition(true), is_first_step(true), previous_step(Direction::Stay) {
	this->winner_num_steps = winner_num_steps;
	this->sum_dirt_in_house = sum_dirt_in_house;
	this->current_battery_capacity = current_battery_capacity;
}

//default score function
int RunResult::calc_score(const map<string, int>& score_params) {
	//initialize score to 0
	int score = 0;
	int position_in_competition = 10;
	int actual_position_in_competition = score_params.find("actual_position_in_competition")->second;
	//default score formula doesn't use simulation_step parameter
//	int simulation_steps = score_params.find("simulation_steps")->second;
	int winner_num_steps = score_params.find("winner_num_steps")->second;
	int this_num_steps = score_params.find("this_num_steps")->second;
	int sum_dirt_in_house = score_params.find("sum_dirt_in_house")->second;
	int dirt_collected = score_params.find("dirt_collected")->second;
	int is_back_in_docking = score_params.find("is_back_in_docking")->second;

	//check if algorithm finished successfully <=> all dirt collected and its back in docking
	if (dirt_collected == sum_dirt_in_house && is_back_in_docking) {
		position_in_competition = min(4, actual_position_in_competition);
	}
	//if algorithm didn't finish successfully - its place will be 10
	else {
		position_in_competition = 10;
	}
	//calculate using default score formula
	score = max(0, 2000 - (position_in_competition - 1) * 50 + (winner_num_steps - this_num_steps) * 10 - (sum_dirt_in_house - dirt_collected) * 3 + (is_back_in_docking ? 50 : -200));
	return score;
}

//score calculation
int const RunResult::calculateScore() {
	//initialize score to 0
	int score = 0;
	int docking = 0;
	//if entered wall score will be 0
	if (is_wall_encountered) {
		score = 0;
	}
	else {
		//check if returned to the docking at the end
		if (is_back_in_docking) {
			docking = 50;
		}
		else {
			docking = -200;
		}
		score = max(0, 2000 - (position_in_competition - 1) * 50 + (winner_num_steps - this_num_steps) * 10 - (sum_dirt_in_house - dirt_collected) * 3 + docking);
	}
	return score;
}

//for printing result's data
string const RunResult::toString() const {
	return "simulation_step = " + to_string(simulation_steps) +
			"\nactual_position_in_competition = " + to_string(actual_position_in_competition) +
			"\nposition_in_competition = " + to_string(position_in_competition) +
			"\nwinner_id = " + to_string(winner_id) +
			"\nwinner_num_steps = " + to_string(winner_num_steps) +
			"\nthis_num_steps = " + to_string(this_num_steps) +
			"\nsum_dirt_in_house = " + to_string(sum_dirt_in_house) +
			"\ndirt_collected = " + to_string(dirt_collected) +
			"\ncurrent_battery_capacity = " + to_string(current_battery_capacity) +
			"\ntimes_at_docking = " + to_string(times_at_docking) +
			"\nprevious_step_at_docking = " + to_string(previous_step_at_docking) +
			"\nis_back_in_docking = " + to_string(is_back_in_docking) +
			"\nwall_encountered = " + to_string(is_wall_encountered) +
			"\nbattery_emptied = " + to_string(is_battery_emptied) +
			"\nfinished_successfully = " + to_string(is_finished_successfully) +
			"\nis_still_in_competition = " + to_string(is_still_in_competition) + "\n";
}
