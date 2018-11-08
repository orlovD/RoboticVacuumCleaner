#include "Simulation.h"

//initialize simulation parameters - constructor
Simulation::Simulation(const Configuration& configuration) {
	setParams(configuration);
	max_steps_after_winner = params.find("MaxStepsAfterWinner")->second;
	max_battery_capacity = params.find("BatteryCapacity")->second;
	battery_consumption_rate = params.find("BatteryConsumptionRate")->second;
	battery_recharge_rate = params.find("BatteryRechargeRate")->second;
}

//set houses from vector
void Simulation::setHouses(const vector<House>& houses) {
	this->houses = houses;
}

//set algorithm from registrar
void Simulation::setAlgorithms(const AlgorithmRegistrar& registrar) {
	//initialize algorithms_unique from registrar
	this->algorithms_unique = registrar.getAlgorithms();
}

//set score formula
void Simulation::setScoreFormula(const calc_score_function& score_formula_pointer) {
	this->score_formula_pointer = score_formula_pointer;
	//set vector of final results to size of houses * algorithms so it will fit all the results of all runs
	final_results.resize(houses.size() * algorithms_unique.size());
}

//set video using boolean variable
void Simulation::setVideo(const bool& is_video_enabled) {
	this->is_video_enabled = is_video_enabled;
	//if video is enabled
	if (this->is_video_enabled) {
		//create vector of videos of appropriate size
		videos.resize(houses.size() * algorithms_unique.size());
	}
}

//set configuration
void Simulation::setParams(const Configuration& config) {
	this->params = config.getParams();
//	 for (auto& x: params) {
//	    std::cout << x.first << ": " << x.second << '\n';
//	  }
}

//calculate collected dirt
int Simulation::collectDirtAtCurrentLocation(House& house, const Point& curr_point) {
	int collected_this_step = 0;
	if (house.isLocationHasDirt(curr_point)) {
		house.decreaseDirtAtLocation(curr_point);
		collected_this_step = 1;
	}
	return collected_this_step;
}

//make step
bool Simulation::performStep(const Direction& step, const SensorInformation& current_sensor_info, Point& curr_point) {
	bool success = true;
	switch (step) {
		case Direction::East: {
			success = !current_sensor_info.isWall[(int)Direction::East];
			//update position
			curr_point.increaseX();
			break;
		}
		case Direction::West: {
			success = !current_sensor_info.isWall[(int)Direction::West];
			//update position
			curr_point.decreaseX();
			break;
		}
		case Direction::South: {
			success = !current_sensor_info.isWall[(int)Direction::South];
			//update position
			curr_point.increaseY();
			break;
		}
		case Direction::North: {
			success = !current_sensor_info.isWall[(int)Direction::North];
			//update position
			curr_point.decreaseY();
			break;
		}
		case Direction::Stay: {
			success = true;
			break;
		}
		default: {
		}
	}
	return success;
}

//initialize data for each algorithm on current house
//vector<unique_ptr<AbstractAlgorithm>>& algorithms_unique - different from algorithms_unique field of Simulation
//it a vector of algorithms run on current house by current thread
void Simulation::initAlgorithmsData(const House& house,	vector<unique_ptr<AbstractAlgorithm>>& algorithms_unique, const size_t max_steps_on_house, Point& docking,
		vector<RunResult>& algorithm_data, vector<Point>& current_points, vector<House>& houses, vector<Sensor>& sensors) {
	//starting point for algorithm
	Point curr_point;
	//find docking station
	curr_point = house.findDocking();
	//save docking in current house
	docking = curr_point;
	//initialize data for each algorithm on current house
	for (size_t i = 0; i < algorithms_unique.size(); i++) {
		//set winner_max_steps to max_steps_on_house
		RunResult temp_result = RunResult(max_steps_on_house, house.sumDirt(), max_battery_capacity);
		//insert new initialized result into the vector
		algorithm_data.push_back(temp_result);
		//set point of docking as start point of each algorithm
		current_points.push_back(docking);
		//add house into vector
		houses.push_back(house);
	}
	//set algorithm parameters and sensors
	int i = 0;
	for(auto& algorithm : algorithms_unique) {
		//set configuration for each algorithm
	    algorithm->setConfiguration(params);
		//create new instance of sensor
		Sensor temp_sensor(&houses[i]);
		//add sensor to vector
		sensors.push_back(temp_sensor);
		//increase i - go to next value in vector
	    i++;
	}
	//set i to 0 - start from first value in sensors' vector
	i = 0;
	for(auto& algorithm : algorithms_unique) {
		//pass the sensor to algorithm
		algorithm->setSensor(sensors[i]);
	    i++;
	}
}

//initialize video
void Simulation::initVideo(vector<House>& houses, const vector<string>& algorithms_file_names, const vector<string>& houses_file_names, const int index) {
	//if video is enabled
	if (is_video_enabled) {
		for (size_t i = 0; i < algorithms_unique.size(); i++) {
			//create new video object
			Video video(&houses[i], algorithms_file_names[i], houses_file_names[index]);
			//debug
//			cout << "i + index * algorithms_unique.size() is: " << i + index * algorithms_unique.size() << endl;
			//add video object into vector of videos
			videos[i + index * algorithms_unique.size()] = video;
		}
	}
}

//check if any algorithm still in competition
bool Simulation::checkAlgorithms(const vector<RunResult>& algoritms_in_competition) {
	int result = false;
	for (size_t i = 0; i < algoritms_in_competition.size(); i++) {
		if (algoritms_in_competition[i].is_still_in_competition) {
			result = true;
		}
	}
	return result;
}

//iterate over the points of finishing of each algorithm and check if back at docking
void Simulation::checkIfBackAtDocking(const vector<Point>& current_points,
		const Point& docking, vector<RunResult>& algorithm_data) {
	//iterate over the points of finishing of each algorithm and
	for (size_t i = 0; i < current_points.size(); i++) {
		//check if algorithms returned to docking
		if (current_points[i] == docking) {
			//if algorithm is at docking - set is_back_in_docking to true
			algorithm_data[i].is_back_in_docking = true;
		}
	}
}

//calculate actual_position_in_copmetition for each algorithm
void Simulation::calculateActualPosition(vector<RunResult>& algorithm_data, const size_t max_steps_on_house) {
	////new function for giving places
	int index_of_min = -1;
	int minimum = 0;
	int prev_min = 0;
	vector<int> steps;
	vector<int> actual_position;
	//set actual_position_in_competition for all algorithms finished successfully
	for (size_t i = 0; i < algorithms_unique.size(); i++) {
		if (algorithm_data[i].is_finished_successfully) {
			//add number of steps of current algorithm
			steps.push_back(algorithm_data[i].this_num_steps);
			//set position to -1
			actual_position.push_back(-1);
		}
	}
	int place = 1;
	int num_places = 1;
	int maximum = (int) (max_steps_on_house) * 2;
	if (steps.size() > 0) {
		//find index of minimum element
		index_of_min = std::distance(steps.begin(), std::min_element(steps.begin(), steps.end()));
		//find value of minimum element
		minimum = steps[index_of_min];
		actual_position[index_of_min] = place;
	}
	else {
		minimum = maximum;
	}
	//save current minimum as previous
	prev_min = minimum;
	while (minimum < maximum) {
		if (steps.size() > 0) {
			//set number of steps to 1M at current index of min
			steps[index_of_min] = maximum;
			//find index of new minimum element
			index_of_min = std::distance(steps.begin(),	std::min_element(steps.begin(), steps.end()));
			//find value of new minimum element
			minimum = steps[index_of_min];
			//check if new minimum < maximum
			if (minimum < maximum) {
				//check if the new minimum is the same as the old
				if (minimum != prev_min) {
					//increase current place by number of same places
					place += num_places;
					//new place
					num_places = 1;
				} else {
					//increase number of current places
					num_places++;
				}
				//give place to new minimum value
				actual_position[index_of_min] = place;
				//save current minimum as previous
				prev_min = minimum;
			}
		}
	}
	int j = 0;
	//set actual_position_in_competition for all algorithms finished successfully
	for (size_t i = 0; i < algorithms_unique.size(); i++) {
		if (algorithm_data[i].is_finished_successfully) {
			//set actual_position_in_competition
			algorithm_data[i].actual_position_in_competition = actual_position[j];
			//increase j to next algorithm's position
			j++;
		}
	}
}

//set position_in_competition for each algorithm
void Simulation::setPosition(vector<RunResult>& algorithm_data) {
	//set position in competition to each algorithm
	for (size_t i = 0; i < algorithms_unique.size(); i++) {
		//if algorithm finished successfully
		if (algorithm_data[i].is_finished_successfully) {
			algorithm_data[i].position_in_competition = min(4, algorithm_data[i].actual_position_in_competition);
		}
		//if algorithm didn't finish successfully - its position will be 10
		else {
			algorithm_data[i].position_in_competition = 10;
		}
	}
}

//iterate over the algorithms after simulation finishes and update necessary data
void Simulation::setPostRunData(const vector<House>& houses, vector<RunResult>& algorithm_data, const int steps_performed, const int winner_num_steps) {
	//iterate over the algorithms after simulation finishes and update necessary data
	for (size_t i = 0; i < algorithms_unique.size(); i++) {
		//print houses after simulation ends
		//debug
//		houses[i].print();
		//set actual simulation steps for each algorithm result
		algorithm_data[i].simulation_steps = steps_performed;
		//set winner_num_step for each algorithm
		algorithm_data[i].winner_num_steps = winner_num_steps;
		//set this_num_steps for each algorithm that finished with empty battery
		if (algorithm_data[i].is_battery_emptied) {
			//for such algorithms this_num_steps equals to actual simulation_steps
			algorithm_data[i].this_num_steps = steps_performed;
		}
		//set winner_num_steps if there is no winner - winner_num_steps remained 0
		if (!winner_num_steps) {
			//in this case winner_num_steps equals to actual simulation_steps
			algorithm_data[i].winner_num_steps = steps_performed;
		}
	}
}

//create map for score calculation
map<string, int> Simulation::createScoreParameters(const vector<RunResult>& algorithm_data, const size_t algorithm_number) {
	//map of score parameters
	map<string, int> score_params;
	//create map
	score_params.emplace("actual_position_in_competition", algorithm_data[algorithm_number].actual_position_in_competition);
	score_params.emplace("simulation_steps", algorithm_data[algorithm_number].simulation_steps);
	score_params.emplace("winner_num_steps", algorithm_data[algorithm_number].winner_num_steps);
	score_params.emplace("this_num_steps", algorithm_data[algorithm_number].this_num_steps);
	score_params.emplace("sum_dirt_in_house", algorithm_data[algorithm_number].sum_dirt_in_house);
	score_params.emplace("dirt_collected", algorithm_data[algorithm_number].dirt_collected);
	score_params.emplace("is_back_in_docking", algorithm_data[algorithm_number].is_back_in_docking);
	return score_params;
}

//calculate final results
void Simulation::calculateResults(vector<RunResult>& algorithm_data, const vector<string>& algorithms_file_names,
		const vector<string>& houses_file_names, const size_t& index) {
	string wall_entered_error = "";
	//calculate results for each algorithm on current house
	for (size_t i = 0; i < algorithms_unique.size(); i++) {
		//if wall encountered
		if (algorithm_data[i].is_wall_encountered) {
			//score of such algorithm is 0
			//final_results.push_back(0);
			//set score on appropriate place
			final_results[i + index * algorithms_unique.size()] = 0;
			wall_entered_error = "Algorithm " + algorithms_file_names[i] + " when running on House "
					+ houses_file_names[algorithm_data[i].house_number_of_wall_encountering]
					+ " went on a wall in step " + to_string(algorithm_data[i].step_of_wall_encountering);
			//add error to vector of wall encountering errors
			errors_during_the_simulation.push_back(wall_entered_error);
		}
		//wall is not encountered
		else {
			//create map of score parameters
			map<string, int> score_params = createScoreParameters(algorithm_data, i);
			//set for -1 - will remain unchanged if calculation failed
			int score = -1;
			//calculate score
			score = score_formula_pointer(score_params);
			//add score to vector of results
			//set score on appropriate place
			final_results[i + index * algorithms_unique.size()] = score;
		}
		//if video is enabled - create video file from images
		if (is_video_enabled) {
			//if image folder was created
			if (videos[i + index * algorithms_unique.size()].is_folder_created) {
				//create movie from images taken
				videos[i + index * algorithms_unique.size()].encodeImagesIntoVideo();
			}
		}
	}
	//debug
//	for (size_t i = 0; i < algorithms_unique.size(); i++) {
//		cout << algorithm_data[i].toString() << endl;
//	}
}

//create final results table
void Simulation::createFinalResultsTable(const vector<string>& algorithms_file_names, const vector<string>& houses_file_names) {
	vector<string> final_results_formatted;
	vector<double> average_for_algorithm;
	ostringstream res;
	ostringstream tmp_res;
	ostringstream dashes;
	//store algorithm names
	vector<string> algorithm_names;
	//store algorithm name and average sorted by value and then by key
	set<pair<string, double>, PairComparator> algorithm_with_average;
	//create first column
	res << "|" << setfill(' ') << setw(13 + 1);
	//fill in house names
	for (size_t i = 0; i < houses.size(); i++) {
		//create house names
		res << "|" << setw(10) << left << houses_file_names[i].substr(0, 9);
	}
	//add AVG tab
	res << "|" << setw(10) << left << "AVG";
	//calculate averages
	for (size_t i = 0; i < algorithms_unique.size(); i++) {
		double sum = 0;
		for (size_t j = 0; j < houses.size(); j++) {
			int k = i + algorithms_unique.size() * j;
			sum += final_results[k];
			//debug
//			cout << "k=" << k << endl;
		}
		double average = sum / houses.size();
		//add results to vector
		average_for_algorithm.push_back(sum / houses.size());
		string algorithm_name = algorithms_file_names[i].substr(0, 12);
		//add algorithm name to vector of names
		algorithm_names.push_back(algorithm_name);
		//add algorithm name with average to set
		algorithm_with_average.emplace(algorithms_file_names[i], average);
	}
	res << "|" << endl;
	string s = res.str();
	int length = s.length();
	//create dashed line for separating lines in table
	dashes << setfill('-') << setw(length - 1) << "-" << endl;
	/* swap 2 streams */
	tmp_res << dashes.str() << res.str();
	//empty stream
	res.str("");
	//reset errors
	res.clear();
	//swap streams
	res << tmp_res.str();

	//create iterator for set of pairs
	set<pair<string, double>>::iterator it;
	//get current algorithm index - algorithm with maximal AVG
	for (it = algorithm_with_average.begin(); it != algorithm_with_average.end(); ++it) {
		size_t index = find(algorithms_file_names.begin(), algorithms_file_names.end(), (*it).first) - algorithms_file_names.begin();
		//create dashed line for separating lines in table
		res << setfill('-') << setw(length - 1) << "-" << endl;
		//set algorithm name
		res << "|" << setfill(' ') << setw(13) << left << algorithms_file_names[index].substr(0, 12);
		//create results on current house
		for (size_t j = 0; j < houses.size(); j++) {
			int k = index + algorithms_unique.size() * j;
			//create results of current algorithm and add results to stream
			res << "|" << setw(10) << right << to_string(final_results[k]).substr(0, 9);
		}
		//add average
		res << "|" << setw(10) << right << setprecision(2) << fixed << average_for_algorithm[index] << "|" << endl;
	}
	//close the table from the bottom
	res << setfill('-') << setw(length - 1) << "-";// << endl;
	//print final result table
	cout << res.str() << endl;
	//check if there were score calculation errors
	for (size_t i = 0; i < final_results.size(); i++) {
		//if there are any results equal to -1
		if (final_results[i] == -1) {
			//create error string
			string score_error = "Score formula could not calculate some scores, see -1 in the results table";
			//add error message to be printed after all errors
			errors_during_the_simulation.push_back(score_error);
			//add this error only once
			break;
		}
	}
	//check if there were video creation errors
	for (size_t i = 0; i < videos.size(); i++) {
		//if there are any errors in video
		if (videos[i].errors_during_video_creation.size()) {
			for (size_t j = 0; j < videos[i].errors_during_video_creation.size(); j++) {
				//add error message to be printed after all errors
				errors_during_the_simulation.push_back(videos[i].errors_during_video_creation[j]);
			}
		}
	}
}

//run multithreaded and return simulation errors
vector<string> const Simulation::runMultiThreaded(size_t num_threads, const vector<string>& algorithms_file_names,
		const vector<string>& houses_file_names) {
	//create threads if more than 1 thread needed
	//debug
//	cout << "num_threads is: " << num_threads << endl;
	if (num_threads > 1) {
		//debug
//		cout << "Multi threaded run" << endl;
		// ===> create the threads as vector of pointers to threads (unique_ptr)
		vector<unique_ptr<thread>> threads(num_threads);
		for(auto& thread_ptr : threads) {
			// ===> actually create the threads and run them
			thread_ptr = make_unique<thread>(&Simulation::runSingleSubSimulationThread, this,
					ref(algorithms_file_names), ref(houses_file_names)); // create and run the thread
		}
		// ===> join all the threads to finish nicely (i.e. without crashing / terminating threads)
		for(auto& thread_ptr : threads) {
			thread_ptr->join();
		}
	}
	//use main thread inly if 1 thread is needed
	else {
		//debug
//		cout << "Single threaded run" << endl;
		runSingleSubSimulationThread(algorithms_file_names, houses_file_names);
	}
    //after all threads are finished - create table of final results
    createFinalResultsTable(algorithms_file_names, houses_file_names);
    //return vector of errors to caller
    return errors_during_the_simulation;
}

//run sub-simulation for one thread
void Simulation::runSingleSubSimulationThread(const vector<string>& algorithms_file_names, const vector<string>& houses_file_names) {
	for (size_t i = house_index++; i < houses.size(); i = house_index++) {
		//receive results of different runs into final_results vector
		runAlgorithmsOnHouse(houses[i], algorithms_file_names, houses_file_names, i);
	}
}

//run algorithms - receive current house and algorithms to run on this house
//calculates score for each algorithm on current house
void Simulation::runAlgorithmsOnHouse(House& house, const vector<string>& algorithms_file_names, const vector<string>& houses_file_names, const size_t& index) {
	//get instance of registrar
    AlgorithmRegistrar& registrar = AlgorithmRegistrar::getInstance();
    //get algorithms for current thread - different from algorithms_unique field of Simulation
    auto algorithms_unique = registrar.getAlgorithms();
//	house.print();
	//get max_steps from the current house object
	size_t max_steps_on_house = house.getMaxSteps();
	//simulation steps on current house
	int steps_performed = 0;
	//save winner_num_steps on current run
	int winner_num_steps = 0;
	//flag to know if aboutToFinish() already has been called
	bool is_aboutToFinish_called = false;
	//vector of sensors for each algorithm
	vector<Sensor> sensors;
	//vector of locations of each algorithm inside the house
	vector<Point> current_points;
	//vector of houses for each algorithm
	vector<House> houses;
	//vector of RunResults - save data of current algorithm in current run
	vector<RunResult> algorithm_data;
	//docking station coordinates
	Point docking;
	//initialize vector of algorithm_data
	initAlgorithmsData(house, algorithms_unique, max_steps_on_house, docking, algorithm_data, current_points, houses, sensors);
	//initialize video objects
	initVideo(houses, algorithms_file_names, houses_file_names, index);
	//iterate over each algorithm and make one step in RoundRobin fashion
	for (size_t t = 0; t < max_steps_on_house; t++) {
		//if not all algorithms finished - continue
		if (checkAlgorithms(algorithm_data)) {
			//when simulation is about to finish
			//update all the algorithms still in competition with max_steps_after_winner
			if (t == (max_steps_on_house - max_steps_after_winner)) {
				//if there is still no winner
				if (!winner_num_steps) {
					//index of algorithm in competition
					int k = 0;
					//iterate over unique_ptr algorithms
					for(auto& algorithm : algorithms_unique) {
						//call only algorithms still in competition
						if (algorithm_data[k].is_still_in_competition) {
							algorithm->aboutToFinish(max_steps_after_winner);
						}
						//go to next algorithm
						k++;
					}
				}
			}
			int i = 0;
			//iterate over unique_ptr algorithms
			for(auto& algorithm : algorithms_unique) {
				//create frame of house on current step
//				house.montage(algorithms_file_names[i], houses_file_names[index], current_points[i], algorithm_data[i].this_num_steps);
				//only algorithms that are still in competition will continue to run
				if (algorithm_data[i].is_still_in_competition) {
					//update sensor data
					sensors[i].setLocation(current_points[i]);
					//run algorithm number i for one step on current house
					if (makeSingleMove(houses[i], algorithm, docking, current_points[i], sensors[i], algorithm_data[i], winner_num_steps,
							videos[i + index * algorithms_unique.size()])) {
//							algorithms_file_names[i], houses_file_names[index])) {
						//if wall encountered
						algorithm_data[i].is_wall_encountered = true;
						//set current step as step of encountering the wall
						algorithm_data[i].step_of_wall_encountering = algorithm_data[i].this_num_steps;
						//set current house index in houses_file_names as house of encountering the wall
						algorithm_data[i].house_number_of_wall_encountering = index;
						//algorithm entered the wall finishes its run
						algorithm_data[i].is_still_in_competition = false;
					}
//					cout << "position of algorithm #" << i << " is: " << current_points[i].toString() << endl;
					//check if winner just found but not updated
					//when winner_num_steps changed after current step it means current algorithm (algorithm #i) is the winner
					if (winner_num_steps != 0 && algorithm_data[i].winner_id == -1) {
						//iterate over algorithm_data values
						int j = 0;
						//iterate over unique_ptr algorithms
						//set winner_id and winner_num_step for each algorithm
						int l = 0; //index algorithms to call aboutToFinish()
						for(auto& algorithm : algorithms_unique) {
							algorithm_data[j].winner_id = i;
							algorithm_data[j].winner_num_steps = winner_num_steps;
							//when winner is found - update all the algorithms with max_steps_after_winner if it wasn't done yet
							if (!is_aboutToFinish_called) {
								//call only algorithms still in competition
								if (algorithm_data[l].is_still_in_competition) {
									//winner is algorithm #i, so all algorithms before it already done its steps at this round
									//they should receive min(max_steps_after_winner, max_steps_on_house - t) steps till finish
									if (l < i) {
										//send to each algorithm steps left till simulation ends
										algorithm->aboutToFinish(min(max_steps_after_winner, max_steps_on_house - t));
									}
									//winner is algorithm #i, so all algorithms after it didn't do its steps at this round
									//they should receive additional step till finish for compensation
									if (l > i) {
										//send to each algorithm steps left till simulation ends
										algorithm->aboutToFinish(min(max_steps_after_winner, max_steps_on_house - t) + 1);
									}
								}
								//go to next algorithm
								l++;
							}
							//increase j - go to next algorithm_data value
							j++;
						}
						//winner finishes its run
						algorithm_data[i].is_still_in_competition = false;
					}
					//check if battery level dropped to 0 after this step and robot is not in docking
					if (algorithm_data[i].current_battery_capacity <= 0 && current_points[i] != docking) {
						//algorithm with empty battery finishes its run
						algorithm_data[i].is_still_in_competition = false;
						//flag of empty battery is set
						algorithm_data[i].is_battery_emptied = true;
					}
					//check if algorithm finished cleaning
					if (algorithm_data[i].is_finished_successfully) {
						//algorithm cleaned all dust and returned to docking finishes its run
						algorithm_data[i].is_still_in_competition = false;
					}
				}
				//increase i
				i++;
			}
			//increase number of steps performed by simulation
			steps_performed++;
		}
		//no algorithms left in competition - simulation ends
		else {
			break;
		}
	}
	//iterate over the points of finishing of each algorithm and check if back at docking
	checkIfBackAtDocking(current_points, docking, algorithm_data);
	//iterate over the algorithms after simulation finishes and update necessary data
	setPostRunData(houses, algorithm_data, steps_performed, winner_num_steps);
	//get actual_position_in_copmetition of each algorithm
	calculateActualPosition(algorithm_data, max_steps_on_house);
	//set actual_position_in_competition for each algorithm
	setPosition(algorithm_data);
	//calculate final results
	calculateResults(algorithm_data, algorithms_file_names, houses_file_names, index);
//	house.print();
}

//make one move of the algorithm, return 1 if wall encountered
int const Simulation::makeSingleMove(House& house, unique_ptr<AbstractAlgorithm>& algorithm, const Point& docking, Point& curr_point, Sensor& sensor,
		RunResult& algorithm_data, int& winner_num_steps, Video& video) {
	//if video is enabled - create current frame
	if (is_video_enabled) {
		//if folder creation not failed
		if (video.is_folder_created) {
			//create current frame
			video.montage(curr_point);
		}
	}
	//debug
	//print each step
//	house.print();
	//debug
//	cout << "thread #" << std::this_thread::get_id() << " in makeSingleMove() algorithm address is: " << &algorithm << endl;
//	usleep(100000);
//	house.print();
	//check battery capacity
	if (algorithm_data.current_battery_capacity > 0) {
		//increase total number of steps for current algorithm
		algorithm_data.this_num_steps++;
		int collected_this_step = collectDirtAtCurrentLocation(house, curr_point);
		algorithm_data.dirt_collected += collected_this_step;
		SensorInformation current_sensor_info = sensor.sense();
		//variable for next step
		Direction step;
		//if it's first step of the algorithm - send it 'Stay' as previous step
		if (algorithm_data.is_first_step) {
			//get next step
			step = algorithm->step(Direction::Stay);
			//set previous step of algorithm to value returned by it's function step()
			algorithm_data.previous_step = step;
			//set step to next step
			algorithm_data.is_first_step = false;
		}
		//if it's first step of the algorithm - send it actual step taken by simulation as previous step
		else {
			//get next step
			step = algorithm->step(algorithm_data.previous_step);
			//set previous step of algorithm to value returned by it's function step()
			algorithm_data.previous_step = step;
		}
		bool success = performStep(step, current_sensor_info, curr_point);
		//wall encountered
		if (!success) {
			return 1;
		}
//		cout << curr_point.toString() << endl;
		//update sensor with current coordinates on map
		sensor.setLocation(curr_point);

		//check if cleaning is finished and robot is back at docking
		if (curr_point == docking && house.sumDirt() == 0) {
			//if there is no winner yet - winner_num_steps still 0
			if (!winner_num_steps) {
				//this is the winner
				algorithm_data.winner_num_steps = algorithm_data.this_num_steps;
				winner_num_steps = algorithm_data.this_num_steps;
			}
			//algorithm at docking
			algorithm_data.is_back_in_docking = true;
			//algorithm finished successfully its run
			algorithm_data.is_finished_successfully = true;
			//if video is enabled - create current frame
			if (is_video_enabled) {
				//if folder creation not failed
				if (video.is_folder_created) {
					//create last frame of returning into docking
					video.montage(curr_point);
				}
			}
		}
		else {
		}
		//if robot not at docking
		if (curr_point != docking) {
			algorithm_data.previous_step_at_docking = false;
			//reduce battery level by battery_consumption_rate
			algorithm_data.current_battery_capacity = algorithm_data.current_battery_capacity - battery_consumption_rate;
		}
		//if robot at docking station
		else {
			if (algorithm_data.current_battery_capacity < max_battery_capacity) {
				//if previous step was at docking
				if (algorithm_data.previous_step_at_docking) {
					//charge the battery
					algorithm_data.current_battery_capacity = min(algorithm_data.current_battery_capacity + battery_recharge_rate + battery_consumption_rate, max_battery_capacity);
				}
				//if previous step wasn't at docking
				else {
					algorithm_data.current_battery_capacity = min(algorithm_data.current_battery_capacity + battery_recharge_rate, max_battery_capacity);
				}
			}
			algorithm_data.previous_step_at_docking = true;
			//increase number of times robot was at docking station
			algorithm_data.times_at_docking++;
		}
	}
	//simulation of the step finished successful
	return 0;
}
