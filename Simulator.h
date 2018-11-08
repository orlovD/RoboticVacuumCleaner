#ifndef __SIMULATOR_H_
#define __SIMULATOR_H_

#include "Direction.h"

#include "AbstractAlgorithm.h"
#include "AbstractSensor.h"

#include "Configuration.h"
#include "Simulation.h"
#include "ReadAllFiles.h"
#include "Utils.h"

// AlgorithmRegistration.h is part of BOTH the simulation project
// and the Algorithm projects
#include "AlgorithmRegistration.h"
// AlgorithmRegistrar.h is part of the simulation project ONLY
#include "AlgorithmRegistrar.h"

#include "CalcScore.h"

class Simulator {
	//vector of house errors during the run
	vector<string> house_errors_during_the_run;
	//vector of algorithms errors during the run
	vector<string> algorithms_errors_during_the_run;
	//flag enabled if no houses loaded
	bool is_no_houses_loaded;
	//flag enabled if no algorithms loaded
	bool is_no_algorithms_loaded;
	//vector of algorithm filenames
	vector<string> algorithms_file_names;
	//vector of house filenames
	vector<string> houses_file_names;
	//set houses and algorithms names
	void setNames(const vector<string>& algorithm_path_list, const vector<string>& house_path_list);
	//load algorithms from .so files
	AlgorithmRegistrar& loadAlgorithmsFromFiles(vector<string>& temp_algorithm_path_list, vector<string>& algorithm_path_list);
	//load houses from file
	void loadHousesFromFiles(vector<string>& temp_house_path_list, vector<string>& house_path_list, vector<House>& houses);

public:
	//constructor
	Simulator();
	//run simulation
	int const run(const Configuration& configuration, const calc_score_function& score_formula_pointer,
			vector<string>& algorithm_path_list, vector<string>& house_path_list, size_t& number_of_threads, const bool& is_video_enabled);
	//print all the errors after program end - return 0 if no errors found, 1 otherwise
	int const printErrors(const string& algorithm_path, const string& house_path) const;
	//close handles for shared objects
	static void closeHandles();
	//destructor
	~Simulator(){};
};

#endif /* __SIMULATOR_H_ */
