#include "Simulator.h"

Simulator::Simulator() : is_no_houses_loaded(0), is_no_algorithms_loaded(0) {
}

//set houses and algorithms names
void Simulator::setNames(const vector<string>& algorithm_path_list, const vector<string>& house_path_list) {
	//get algorithms names
	algorithms_file_names = ReadAllFiles::getNamesFromFullPath(algorithm_path_list);
	//get houses names
	houses_file_names = ReadAllFiles::getNamesFromFullPath(house_path_list);
}

//load algorithms from .so files
AlgorithmRegistrar& Simulator::loadAlgorithmsFromFiles(vector<string>& temp_algorithm_path_list, vector<string>& algorithm_path_list) {
	//get instance of AlgorithmRegistrar
	AlgorithmRegistrar& registrar = AlgorithmRegistrar::getInstance();
	//store succeeded algorithms_file_names
	vector<string> temp_algorithm_names;
	//set algorithms from list
	if (algorithm_path_list.size() > 0) {
		//get folder name
		string algorithms_folder = algorithm_path_list[0].substr(0, algorithm_path_list[0].find_last_of("."));
		algorithms_folder = algorithms_folder.substr(0, algorithm_path_list[0].find_last_of("/") + 1);
		//variable for current position in vector
		int i = 0;

		for (const auto& algorithmSoFileName : algorithms_file_names) {
			//try to register algorithm
			int result = registrar.loadAlgorithm(algorithms_folder,	algorithmSoFileName);
			//if algorithm didn't register successfully
			if (result != AlgorithmRegistrar::ALGORITHM_REGISTERED_SUCCESSFULY) {
				if (result == AlgorithmRegistrar::FILE_CANNOT_BE_LOADED) {
					//create error string
					string error = algorithmSoFileName + ".so: file cannot be loaded or is not a valid .so";
					//add error to the vector of errors
					algorithms_errors_during_the_run.push_back(error);
				}
				if (result == AlgorithmRegistrar::NO_ALGORITHM_REGISTERED) {
					//create error string
					string error = algorithmSoFileName + ".so: valid .so but no algorithm was registered after loading it";
					//add error to the vector of errors
					algorithms_errors_during_the_run.push_back(error);
				}
			}
			else { //algorithm registered successfully
				//add name to temp_names
				temp_algorithm_path_list.push_back(algorithm_path_list[i]);
			}
			//increase i to next item
			i++;
		}
	}
	//return resulted instance of registrar
	return registrar;
}

//load houses from file
void Simulator::loadHousesFromFiles(vector<string>& temp_house_path_list,	vector<string>& house_path_list, vector<House>& houses) {
	//use files' paths to get houses from files
	for (size_t i = 0; i < house_path_list.size(); i++) {
		//get house filename
		string filename = house_path_list[i].substr(house_path_list[i].find_last_of("/") + 1, house_path_list[i].size());
		try {
			//try to create new house from given path
			House house_temp(house_path_list[i]);
			//check lines in house file
			string negative_lines_error = house_temp.checkLines(filename);
			//if negative lines found - add error and don't create house
			if (negative_lines_error.compare("")) {
				//add error to the vector of errors
				house_errors_during_the_run.push_back(negative_lines_error);
			}
			else { //if no negative lines found
				//check missing spaces
				house_temp.checkMissingSpaces();
				//check walls
				house_temp.checkWalls();
				//calculate number of docking stations
				int num_of_docking = house_temp.calculateDocking();
				//if no docking stations found
				if (!num_of_docking) {
					//create error string
					string error = filename	+ ": missing docking station (no D in house)";
					house_errors_during_the_run.push_back(error);
				}
				//if more than one docking stations found
				if (num_of_docking > 1) {
					//create error string
					string error = filename + ": too many docking stations (more than one D in house)";
					//add error to the vector of errors
					house_errors_during_the_run.push_back(error);
				}
				//map of the house after adding spaces and walls
//				house_temp.print();
				//if house is valid - add it to vector
				if (num_of_docking == 1 && negative_lines_error == "") {
					//add house to vector of houses
					houses.push_back(house_temp);
					//house checked and no errors found - add its path to paths vector
					temp_house_path_list.push_back(house_path_list[i]);
				}
			}
		}
		catch (int e) {
			//different problem during the load
			if (e == 1) {
				//create error string
				string error = filename + ": cannot open file";
				//add error to the vector of errors
				house_errors_during_the_run.push_back(error);
			}
		}
		catch (exception& e) {
			//create error string
			string error = filename + ": cannot open file";
			//add error to the vector of errors
			house_errors_during_the_run.push_back(error);
		}
	}
}

//run simulation
int const Simulator::run(const Configuration& configuration, const calc_score_function& score_formula_pointer,
		vector<string>& algorithm_path_list, vector<string>& house_path_list, size_t& number_of_threads, const bool& is_video_enabled) {
	//fill vector of houses
	vector<House> houses;
	//store succeeded house_file_names
	vector<string> temp_house_path_list;
	//store succeeded algorithm_file_names
	vector<string> temp_algorithm_path_list;

	//sort house_path_list
	sort(house_path_list.begin(), house_path_list.end());
	//algorithm_path_list
	sort(algorithm_path_list.begin(), algorithm_path_list.end());

	//get ALL(not only legal) houses and algorithms names using provided paths
	setNames(algorithm_path_list, house_path_list);

	//get instance of AlgorithmRegistrar
	AlgorithmRegistrar& registrar = loadAlgorithmsFromFiles(temp_algorithm_path_list, algorithm_path_list);

	//remove problematic algorithm paths from vector of house_path_list
	algorithm_path_list.clear();
	//add loaded algorithm paths to algorithm_path_list vector
	algorithm_path_list = temp_algorithm_path_list;

	//check if any algorithms loaded
	if (!registrar.size()) {
		//if registrar.size() is 0 - no algorithms loaded
		is_no_algorithms_loaded = true;
		//no need to continue to check houses
		return 1;
	}

	//use files' paths to get houses from files
	loadHousesFromFiles(temp_house_path_list, house_path_list, houses);
	//remove problematic house paths from vector of house_path_list
	house_path_list.clear();
	//add loaded houses paths to house_path_list vector
	house_path_list = temp_house_path_list;
	//if no houses loaded
	if (!house_path_list.size()) {
		is_no_houses_loaded = true;
	}

	//get LEGAL houses and algorithms names using provided paths
	setNames(algorithm_path_list, house_path_list);

	//if no houses found - return
	if (!houses.size()) {
		//end program
		return 1;
	}

	//create new simulation
	Simulation simulation_instance(configuration);
	//set algorithms for simulation
	simulation_instance.setAlgorithms(registrar);
	//set houses for simulation
	simulation_instance.setHouses(houses);
	//set score formula for simulation
	simulation_instance.setScoreFormula(score_formula_pointer);
	//set number_of_threads be minimum between number of houses and number_of_threads provided
	number_of_threads = min(number_of_threads, houses_file_names.size());
	//set video using boolean variable
	simulation_instance.setVideo(is_video_enabled);
	//run simulation in number_of_threads multithreaded and get simulation errors
	vector<string> simulation_errors = simulation_instance.runMultiThreaded(number_of_threads, algorithms_file_names, houses_file_names);

	//if any errors during simulation - add them to vector
	if (simulation_errors.size()) {
		for (size_t i = 0; i < simulation_errors.size(); i++) {
			//add errors to vector of errors
			algorithms_errors_during_the_run.push_back(simulation_errors[i]);
		}
	}

	//end of Simulator::run
	return 0;
}

//print all the errors after program end - return 0 if no errors found, 1 otherwise
int const Simulator::printErrors(const string& algorithm_path, const string& house_path) const {
	string full_algorithm_path_no_backslash = "";
	string full_house_path_no_backslash = "";
	//remove backslash from end
	ReadAllFiles::removeBackslashFromEnd(algorithm_path, full_algorithm_path_no_backslash);
	ReadAllFiles::removeBackslashFromEnd(house_path, full_house_path_no_backslash);
	int res;
	//check if there are any errors in the vectors and print header
	if (house_errors_during_the_run.size() > 0 || algorithms_errors_during_the_run.size() > 0) {
		if (!is_no_houses_loaded && !is_no_algorithms_loaded) {
			cout << "\nErrors:" << endl;
		}
		res = 1;
	}
	//if there were no errors
	else {
		res = 0;
	}
	if (is_no_houses_loaded) {
		cout << "All house files in target folder '" << ReadAllFiles::getFullPath(full_house_path_no_backslash) << "' cannot be opened or are invalid:" << endl;
		//print all house errors
		for (size_t i = 0; i < house_errors_during_the_run.size(); i++) {
			cout << house_errors_during_the_run[i] << endl;
		}
		return res;
	}
	//print all house errors
	for (size_t i = 0; i < house_errors_during_the_run.size(); i++) {
		cout << house_errors_during_the_run[i] << endl;
	}
	if (is_no_algorithms_loaded) {
		cout << "All algorithm files in target folder '" << ReadAllFiles::getFullPath(full_algorithm_path_no_backslash) << "' cannot be opened or are invalid:" << endl;
		//print all algorithm errors
		for (size_t i = 0; i < algorithms_errors_during_the_run.size(); i++) {
			cout << algorithms_errors_during_the_run[i] << endl;
		}
		return res;
	}
	//print all algorithm errors
	for (size_t i = 0; i < algorithms_errors_during_the_run.size(); i++) {
		cout << algorithms_errors_during_the_run[i] << endl;
	}
	return res;
}

//close handles for shared objects
void Simulator::closeHandles() {
	//close handles for shared objects
	//get instance of AlgorithmRegistrar
	AlgorithmRegistrar& registrar = AlgorithmRegistrar::getInstance();
	//free handles of loaded algorithms
	string dlclose_errors = registrar.freeHandles();
	//check for errors in dlclose()
	if (dlclose_errors.compare("")) {
		//print errors if found
		cout << dlclose_errors << endl;
	}
}

//run simulator
int main(int argc, char* argv[]) {
	//name for config file
	string config_path_with_filename = "/config.ini";
	//name for score file
	string score_path_with_filename = "/score_formula.so";
	//paths for house files
	vector<string> house_path_list;
	//paths for algorithm files
	vector<string> algorithm_path_list;
	//object of configuration
	Configuration configuration;


	//get current working directory path
	char current_working_directory [PATH_MAX + 1];
	if (getcwd(current_working_directory, sizeof(current_working_directory)) == NULL) {
		cout << "Error in getcwd(): " << errno << endl;
		//exit program if current directory cannot be acquired
		//this case is undefined
		return 1;
	}
	//default values for paths
	string config_path;
	string algorithm_path;
	string house_path;
	config_path = algorithm_path = house_path = current_working_directory;
	string score_formula_path = ""; //if it stays an empty string - default formula will be used
	void* score_file_handle = NULL; //handle to score_formula.so
	//assign pointer to default function
	calc_score_function default_calc_score = RunResult::calc_score;
	//set score function to default
	calc_score_function score_formula_pointer = default_calc_score;
	signed long provided_number_of_threads = -1; //set to -1 for checking if parameter w/o value provided
	size_t number_of_threads = 0;
	bool is_video_enabled = false; //optional parameter for video creating
	//if no input provided - look in current work directory
	//otherwise - check input for validity
	int check_input = ReadAllFiles::checkInput(argc, argv, config_path, config_path_with_filename, configuration,
			score_formula_path, score_path_with_filename, score_file_handle, score_formula_pointer, algorithm_path,
			algorithm_path_list, house_path, house_path_list, provided_number_of_threads, is_video_enabled);
	//if there are any errors
	if (check_input) {
		//check if there are a handle to score_formula
		if (score_file_handle != NULL) {
			//close the handle for formula
			if (dlclose(score_file_handle)) { //returns 0 on success
				cout << "Error in dlclose(): " << dlerror() << endl;
			}
		}
		//return error number
		return check_input;
	}

	//check if parameter '-threads' wasn't provided - set number_of_threads to 1
	if (provided_number_of_threads == -1) {
		number_of_threads = 1;
	}
	else {
		number_of_threads = provided_number_of_threads;
	}

	//create new instance of simulator
	Simulator simulator;
	//run simulator with given parameters
	simulator.run(configuration, score_formula_pointer, algorithm_path_list, house_path_list, number_of_threads, is_video_enabled);

	//close handles for shared objects
	Simulator::closeHandles();

	//print errors from the current run
	//return 1 in case of any errors in processing algorithms and house files
	//return 0 otherwise
	return simulator.printErrors(algorithm_path, house_path);
}
