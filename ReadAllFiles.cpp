#include "ReadAllFiles.h"

//all thrown exceptions will be caught and handled at ReadAllFiles::checkInput

//create list of files by path and extension
ReadAllFiles::ReadAllFiles(const string& path, const string& extension) {
	this->list_of_filenames = getAllFiles(path, extension);
}

//list_of_filenames getter
vector<string> const ReadAllFiles::getListOfFilenames() {
	return this->list_of_filenames;
}

//get filenames from full paths with filename
vector<string> ReadAllFiles::getNamesFromFullPath(const vector<string>& path_list) {
	//create vector of names
	vector<string> name_list;
	for (size_t i = 0; i < path_list.size(); i++) {
		int last_s = path_list[i].find_last_of("/");
		int last_p = path_list[i].find_last_of(".");
		int len = last_p - last_s;
		string filename_i = path_list[i].substr(last_s + 1, len - 1);
		name_list.push_back(filename_i);
	}
	return name_list;
}

//check if path has backslash at the end, add it if not
void ReadAllFiles::addBackslashAtEnd(const string& path, string& file_name) {
	//check if ends with "/" and add it
	if (path[path.length() - 1] != '/') {
		file_name.append("/");
	}
}

//check if path has backslash at the end, remove it if needed
void ReadAllFiles::removeBackslashFromEnd(const string& path, string& file_name) {
	//check if ends with "/" and remove it
	if (path[path.length() - 1] == '/') {
		file_name = path.substr(0, path.size() - 1);
	}
	else {
		file_name = path;
	}
}

//create directory by given path return status of shell process or '-1' on error of creation shell process
//mkdir returns '-1' on failure and '0' on success
int ReadAllFiles::createDirectoryIfNotExists(const string& directory_path) {
	string cmd = "mkdir -p " + directory_path;
	return system(cmd.c_str());
}

//remove directory by given path return status of shell process or '-1' on error of creation shell process
//rm returns '-1' on failure and '0' on success
int ReadAllFiles::removeDirectory(const string& directory_path) {
	string cmd = "rm -rf " + directory_path;
	return system(cmd.c_str());
}

//check if file exists by path
bool ReadAllFiles::isFileExists(const string& path) {
	ifstream infile(path);
	return infile.good();
}

//get full path to file with filename appended by path and filename without extension
string ReadAllFiles::getFullPathWithFileName(const std::string& path, const std::string& so_file_name_without_so_suffix, const string& extension) {
	//copy full path with filename to algorithm
	string file_name = path;
	//check if ends with backslash
	addBackslashAtEnd(path, file_name);
	file_name.append(so_file_name_without_so_suffix);
	//append extension
	file_name.append(extension);
	return file_name;
}

//creates vector of files in given folder with given extension
vector<string> ReadAllFiles::createListOfFileNames(const string& path, const string& extension) {
	string str;
	vector<string> files;
	DIR *dir = NULL;
	dir = opendir(path.c_str());
	//check for errors in opening directory
	if (errno) {
		//debug
//		cerr << "Directory opening failed: " << strerror(errno) << endl;
		throw 1; //directory problem
	}
	struct dirent *ent;
	if(dir == NULL) {
		//debug
//      cout << "ERROR! dir could not be initialized correctly" << endl;
        throw 1; //directory problem
	}
	while((ent = readdir(dir))) {
		//check for errors in reading directory
		if (errno) {
			//debug
//			cerr << "Directory read failed: " << strerror(errno) << endl;
	        throw 1; //directory problem
		}
		if (ent == NULL) {
			//debug
//			cout << "ERROR! ent could not be initialized correctly" << endl;
	        throw 1; //directory problem
		}
		//debug
//		cout << ent->d_name << endl;
		size_t len = strlen (ent->d_name);
		if (len > extension.length()) {
			str  = string(ent->d_name);
			//check if current file has given extension
			if (strcmp(str.substr(str.length() - extension.length(), str.length()).c_str(), extension.c_str()) == 0) {
				files.push_back(str);
				//debug
//				cout << str << endl;
			}
		}
	}
	closedir (dir);
	//check for errors in closing directory
	if (errno) {
		//debug
//		cerr << "Directory closing failed: " << strerror(errno) << endl;
        throw 1; //directory problem
	}
	return files;
}

//get full path by absolute path provided
string ReadAllFiles::getAbsoluteFullPath(const string& absolute_path) {
	string absolute_full_path = "";
	string path = absolute_path;//(absolute_path.length() + 1, ' ');
	//get absolute path
	char* ptr;
	char actualpath[PATH_MAX + 1];
	ptr = realpath(path.c_str(), actualpath);
	//check for errors in getting absolute path
	if (errno) {
		//debug
		//  cerr << "Getting full path failed: " << strerror(errno) << endl;
		exception e;
		throw e;
	}
	//debug
	//	cout << "config_path_with_filename is " << config_path_with_filename << endl;
	//	cout << "full path is " << ptr << endl;
	absolute_full_path.append(ptr);
	//check if ends with backslash
	addBackslashAtEnd(absolute_full_path, absolute_full_path);
	return absolute_full_path;
}

//get full path with filename for config.ini file
string ReadAllFiles::getFullPathToConfigFile(const string& config_path) {
	//get absolute full path
	string config_path_with_filename = ReadAllFiles::getAbsoluteFullPath(config_path);
	//append name of configuration file
	config_path_with_filename.append("config.ini");
	return config_path_with_filename;
}

//get full path with filename
string ReadAllFiles::getFullPathToFile(const string& file_path, const string& file_name) {
	//get absolute full path
	string file_path_with_filename = ReadAllFiles::getAbsoluteFullPath(file_path);
	//append name of configuration file
	file_path_with_filename.append(file_name);
	return file_path_with_filename;
}

//get full path from relative provided
string ReadAllFiles::getFullPath(const string& path_provided) {
	//get current working directory path
	char current_working_directory [PATH_MAX + 1];
	if (getcwd(current_working_directory, sizeof(current_working_directory)) == NULL) {
		perror("getcwd() error");
	}
	//get current work directory
	string full_path = current_working_directory;
	//if relative path provided
	if (path_provided.at(0) != '/') {
		//add backslash to current working directory path and insert it into full_path
		ReadAllFiles::addBackslashAtEnd(full_path, full_path);
		//add path provided to full_path
		full_path.append(path_provided);
	}
	//if absolute path provided
	else {
		try {
			full_path = getAbsoluteFullPath(path_provided);
			//remove backslash
			ReadAllFiles::removeBackslashFromEnd(full_path, full_path);
		}
		//path doesn't exist
		catch (exception &e) {
			//set full path to provided path
			full_path = path_provided;
		}
	}
	//return resulted path
	return full_path;
}

//creates vector of files with full path by given extension and path
vector<string> ReadAllFiles::getAllFiles(const string& files_path, const string& extension) {
	vector<string> houses;
	vector<string> files_paths;
	string path = files_path;
	//get absolute path
	char *ptr;
	char actualpath [PATH_MAX + 1];
	ptr = realpath(path.c_str(), actualpath);
	//check for errors in getting absolute path
	if (errno) {
//		cerr << "Getting full path failed: " << strerror(errno) << endl;
		throw errno;
	}
	houses = ReadAllFiles::createListOfFileNames(ptr, extension);
	for (size_t i = 0; i < houses.size(); i++) {
		string file_path = ptr;
		//check if ends with backslash
		addBackslashAtEnd(file_path, file_path);
		file_path.append(houses[i]);
		files_paths.push_back(file_path);
	}
	return files_paths;
}

//iterate over the argv[] array and calculate number of each parameter provided
//return error number if any, return 0 otherwise
int ReadAllFiles::checkParameters(const int argc, char* argv[], string& config_path, string& score_formula_path,
		string& house_path, string& algorithm_path,	signed long& number_of_threads, bool& is_video_enabled) {
	//check that no more than one parameter of each type provided
	int count_config = 0;
	int count_house = 0;
	int count_algorithm = 0;
	int count_score_formula = 0;
	int count_threads = 0;
	int count_video = 0;
	//iterate over the argv[] array
	for (int i = 0; i < argc; i++) {
//		//supplied parameter only valid if supplied on odd place in the array argv[]
//		if ((string(argv[i]) == "-config") && (i % 2)) {
		//only defined parameters are took into account, not relevant parameters are not counted as errors
		if ((string(argv[i]) == "-config")) {
			//set config_paths to empty string
			config_path = "";
			//if next argument provided
			if (argc > i + 1) {
				//get next argument as parameter
				config_path = argv[i + 1];
			}
			count_config++;
		}
//		if ((string(argv[i]) == "-algorithm_path") && (i % 2)) {
		if ((string(argv[i]) == "-algorithm_path")) {
			//set algorithm_paths to empty string
			algorithm_path = "";
			//if next argument provided
			if (argc > i + 1) {
			//get next argument as parameter
				algorithm_path = argv[i + 1];
			}
			count_algorithm++;
		}
//		if ((string(argv[i]) == "-house_path") && (i % 2)) {
		if ((string(argv[i]) == "-house_path")) {
			//set house_paths to empty string
			house_path = "";
			//if next argument provided
			if (argc > i + 1) {
				//get next argument as parameter
				house_path = argv[i + 1];
			}
			count_house++;
		}
		if ((string(argv[i]) == "-score_formula")) {
			//set score_formula_path to empty string
			score_formula_path = "";
			//if next argument provided
			if (argc > i + 1) {
				//get next argument as parameter
				score_formula_path = argv[i + 1];
			}
			count_score_formula++;
		}
		if ((string(argv[i]) == "-threads")) {
			//set threads to -1
			number_of_threads = -1;
			//if next argument provided
			if (argc > i + 1) {
				//get next argument as parameter
				try {
					number_of_threads = stoll(argv[i + 1]);
				}
				//invalid value provided - cannot convert to number
				catch (exception& e) {
					//set number of threads to be 1 w/o error messages
					number_of_threads = 1;
				}
				//check if invalid value is provided (negative or zero)
				if (number_of_threads < 1) {
					//set number of threads to be 1 w/o error messages
					number_of_threads = 1;
				}
			}
			count_threads++;
		}
		if ((string(argv[i]) == "-video")) {
			//set is_video_enabled to true
			is_video_enabled = true;
			count_video++;
		}
	}
	//if any parameter provided more than once
	if (count_config > 1 || count_house > 1 || count_algorithm > 1 || count_score_formula > 1 || count_threads > 1 || count_video > 1) {
		return 11; //some parameter provided more than once
	}
	//if any of paths is not provided return error number
	if (!config_path.compare("")) {
		return 20; //no config path provided
	}
	if (!score_formula_path.compare("") && count_score_formula == 1) {
		return 30; //no score formula path provided
	}
	if (!algorithm_path.compare("")) {
		return 40; //no algorithm path provided
	}
	if (!house_path.compare("")) {
		return 50; //no house path provided
	}
	if (number_of_threads == -1 && count_threads == 1) {
		return 60; //if its value still -1 - parameter w/o value provided
	}
	//if no errors found
	return 0;
}

//print message to user
void ReadAllFiles::printMessage(const int choise, const string path = "") {
	string path_wo_backslash = "";
	string usage_warning =
			"Usage: simulator [-config <config path>] [-house_path <house path>] [-algorithm_path <algorithm path>] [-score_formula <score .so path>] [-threads <num threads>]";
	//if path provided - remove backslash
	if (path.compare("")) { //returns 0 if equal to ""
		removeBackslashFromEnd(path, path_wo_backslash);
	}
	if (choise == USAGE) {
		cout << usage_warning << endl;
	}
	if (choise == CONFIG_IS_MISSING) {
		cout << usage_warning << endl;
		cout << "cannot find config.ini file in '" << ReadAllFiles::getFullPath(path_wo_backslash) << "'" << endl;
	}
	if (choise == CONFIG_CANNOT_BE_OPENED) {
		cout << "config.ini exists in '" << ReadAllFiles::getFullPath(path_wo_backslash) << "' but cannot be opened" << endl;
	}
	if (choise == ALGORITHMS_ARE_MISSING) {
		cout << usage_warning << endl;
		cout << "cannot find algorithm files in '" << ReadAllFiles::getFullPath(path_wo_backslash) << "'" << endl;
	}
	if (choise == HOUSES_ARE_MISSING) {
		cout << usage_warning << endl;
		cout << "cannot find house files in '" << ReadAllFiles::getFullPath(path_wo_backslash) << "'" << endl;
	}
	if (choise == SCORE_FORMULA_IS_MISSING) {
		cout << usage_warning << endl;
		cout << "cannot find score_formula.so file in '" << ReadAllFiles::getFullPath(path_wo_backslash) << "'" << endl;
	}
	if (choise == SCORE_FORMULA_CANNOT_BE_OPENED) {
//		cout << usage_warning << endl;
		cout << "score_formula.so exists in '" << ReadAllFiles::getFullPath(path_wo_backslash) << "' but cannot be opened or is not a valid .so" << endl;
	}
	if (choise == SCORE_FORMULA_IS_NOT_IMPLEMENTED) {
//		cout << usage_warning << endl;
		cout << "score_formula.so is a valid .so but it does not have a valid score formula" << endl;
	}
}

//check input
//return 0 if succeed
//return error number otherwise
int ReadAllFiles::checkInput(const int argc, char* argv[], string& config_path, string& config_path_with_filename, Configuration& configuration,
		string& score_formula_path, string& score_path_with_filename, void*& score_file_handle,
		calc_score_function& score_formula_pointer, string& algorithm_path, vector<string>& algorithm_path_list,
		string& house_path, vector<string>& house_path_list, signed long& number_of_threads, bool& is_video_enabled) {
	//get error number of parameters errors
	int errors_in_parameters = ReadAllFiles::checkParameters(argc, argv, config_path, score_formula_path,
			house_path, algorithm_path, number_of_threads, is_video_enabled);
	//if no errors in parameters
		if (errors_in_parameters) {
			//if any parameter provided more than once or missing - print usage message and return
			printMessage(USAGE);
			return errors_in_parameters; //return error number
		}

	//try to get filenames
	//get path to configuration file
	try {
		config_path_with_filename = ReadAllFiles::getFullPathToFile(config_path, "config.ini");
	}
	catch (exception& e) {
		printMessage(CONFIG_IS_MISSING, config_path);
		return 21; //configuration path does not exist
	}
	//check configuration file for validity
	string config_folder = config_path_with_filename.substr(0, config_path_with_filename.length() - strlen("/config.ini"));
	try {
		Configuration configuration_temp(config_path_with_filename);
		configuration = configuration_temp;
	}
	//check if there are problems with configuration file
	catch (int e) {
		//file is not exist
		if (e == ENOENT) {
			printMessage(CONFIG_IS_MISSING, config_folder);
			return 22; //path exists but configuration file does not exist
		}
		//file exists but can not be opened
		if (e == 1) {
			printMessage(CONFIG_CANNOT_BE_OPENED, config_folder);
			return 23; //configuration file exists but cannot be opened
		}
	}
	//in case of other errors in system functions
	catch (exception& e) {
		printMessage(CONFIG_CANNOT_BE_OPENED, config_folder);
		return 24; //configuration file exists but cannot be opened because of other system function errors
	}

	//check configuration file data for bad parameters
	string bad_params = configuration.checkBadParameters();
	if (configuration.checkMissingData().length() > 0 || bad_params.length() > 0) {
		//if there is any missing data
		if (configuration.checkMissingData().length() > 0) {
			cout << configuration.checkMissingData() << endl; //print list of missing parameters with names
	//		return 25; //configuration file has missing lines
		}
		//if there is any bad parameters values
		if (bad_params.length() > 0) {
			cout << bad_params << endl; //print list of bad parameters
	//		return 26; //configuration file has bad parameters
		}
		return 27; //configuration file has missing lines and/or bad parameters
	}

	//if any score formula path provided
	if (score_formula_path.compare("")) { //if score_formula_path equals to "default", compare returns 0
		//get path to score formula file
		try {
			score_path_with_filename = ReadAllFiles::getFullPathToFile(score_formula_path, "score_formula.so");
		}
		catch (exception& e) {
			printMessage(SCORE_FORMULA_IS_MISSING, score_formula_path);
			return 31; //score formula path does not exist
		}
		//check if score_formula.so exists
		if (access(score_path_with_filename.c_str(), F_OK) == -1) {
			printMessage(SCORE_FORMULA_IS_MISSING, score_formula_path);
			return 32; //score formula file does not exist
		}
		//clear any existing error
		dlerror();
		//try to open score_formula.so and save the handle into the appropriate variable
		score_file_handle = dlopen(score_path_with_filename.c_str(), RTLD_NOW);
		//if dlopen() fails it returns 0 so handle will be 0
		if (!score_file_handle) {
			//clear any existing error
			dlerror();
			printMessage(SCORE_FORMULA_CANNOT_BE_OPENED, score_formula_path);
			return 33; //score formula cannot be opened or is not valid
		}
		//clear any existing error - next subsequent call will return NULL
		dlerror();
		//pointer to function
		calc_score_function function_from_file;
		//try to get pointer to function from provided file
		void* function = dlsym(score_file_handle, "calc_score");
		//if dlsym() failed - dlerror() will be != NULL
		if (dlerror() != NULL) {
			//clear any existing error
			dlerror();
			printMessage(SCORE_FORMULA_IS_NOT_IMPLEMENTED, score_formula_path);
			return 34; //score formula cannot be found
		}
		//check if there is enough space to copy pointers
		static_assert(sizeof(void*) == sizeof(function_from_file), "Error: sizeof(void*) is greater than sizeof(function_from_file)!");
		//copy pointers
		memcpy(&function_from_file, &function, sizeof function);
		//set score function to function from .so file
		score_formula_pointer = function_from_file;
	}

	//get path to algorithms
	try {
		ReadAllFiles algorithms(algorithm_path, "_.so");
		algorithm_path_list = algorithms.getListOfFilenames();
	}
	catch (int e) {
		if (e == ENOENT) {
			printMessage(ALGORITHMS_ARE_MISSING, algorithm_path);
			return 41; //path to algorithms does not exists
		}
	}
	//in case of other errors in system functions
	catch (exception& e) {
		printMessage(ALGORITHMS_ARE_MISSING, algorithm_path);
		return 42; //algorithms cannot be opened because of system function errors
	}
	//check if any .so files found in the provided directory
	if (!algorithm_path_list.size()) {
		printMessage(ALGORITHMS_ARE_MISSING, algorithm_path);
		return 43; //no algorithms found at given path
	}

	//get path to houses
	try {
		ReadAllFiles houses(house_path, ".house");
		house_path_list = houses.getListOfFilenames();
	}
	catch (int e) {
		if (e == ENOENT) {
			printMessage(HOUSES_ARE_MISSING, house_path);
			return 51; //path to houses does not exists
		}
	}
	//in case of other errors in system functions
	catch (exception& e) {
		printMessage(HOUSES_ARE_MISSING, house_path);
		return 52; //houses cannot be opened because of system function errors
	}
	//check if any .house files found in the provided directory
	if (!house_path_list.size()) {
		printMessage(HOUSES_ARE_MISSING, house_path);
		return 53; //no houses found at given path
	}

	//return 0 if no problems found
	return 0;
}
