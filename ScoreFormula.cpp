#include "Utils.h"

extern "C" {
	//implements default score formula for testing
	int calc_score(const map<string, int>& score_params) {
		//initialize score to 0
		int score = 0;
		int position_in_competition = 10;
		int actual_position_in_competition = score_params.find("actual_position_in_competition")->second;
		int winner_num_steps = score_params.find("winner_num_steps")->second;
		//default score formula doesn't use simulation_step parameter
//		int simulation_steps = score_params.find("simulation_steps")->second;
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
}
