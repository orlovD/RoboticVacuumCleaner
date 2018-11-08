#ifndef READALLFILES_H_
#define READALLFILES_H_

#include "Configuration.h"

#include "Utils.h"

#include "CalcScore.h"

class ReadAllFiles {
	//store list of file names
	vector<string> list_of_filenames;
	//print message to user
	static void printMessage(const int choise, const string path);
	//enum for printing messages
	enum {USAGE, CONFIG_IS_MISSING, CONFIG_CANNOT_BE_OPENED, ALGORITHMS_ARE_MISSING, HOUSES_ARE_MISSING,
		SCORE_FORMULA_IS_MISSING, SCORE_FORMULA_CANNOT_BE_OPENED, SCORE_FORMULA_IS_NOT_IMPLEMENTED};
public:
	//constructors
	ReadAllFiles() {}
	ReadAllFiles(const string& current_directory);
	//create list of files by path and extension
	ReadAllFiles(const string& path, const string& extension);
	//list_of_filenames getter
	const vector<string> getListOfFilenames();
	//get filenames from full paths with filename
	static vector<string> getNamesFromFullPath(const vector<string>& path_list);
	//check if path has backslash at the end, add it if not
	static void addBackslashAtEnd(const string& path, string& file_name);
	//check if path has backslash at the end, remove it if needed
	static void removeBackslashFromEnd(const string& path, string& file_name);
	//create directory by given path return status of shell process or -1 on error
	static int createDirectoryIfNotExists(const string& directory_path);
	//remove directory by given path return status of shell process or -1 on error
	static int removeDirectory(const string& directory_path);
	//check if file exists by path
	static bool isFileExists(const string& path);
	//get full path by absolute path provided
	static string getAbsoluteFullPath(const string& absolute_path);
	//get full path to file with filename appended by path and filename without extension
	static string getFullPathWithFileName(const std::string& path, const std::string& so_file_name_without_so_suffix, const string& extension);
	//creates vector of files in given folder with given extension
	static vector<string> createListOfFileNames(const string& path, const string& extension);
	//get full path with filename for config.ini file
	static string getFullPathToConfigFile(const string& config_path);
	//get full path with filename
	static string getFullPathToFile(const string& file_path, const string& file_name);
	//get full path from relative provided
	static string getFullPath(const string& path_provided);
	//creates vector of files with full path by given extension and path
	static vector<string> getAllFiles(const string& files_path, const string& extension);
	//iterate over the argv[] array and calculate number of each parameter provided
	//return error number if any, return 0 otherwise
	static int checkParameters(const int argc, char* argv[], string& config_path, string& house_path, string& algorithm_path,
				string& score_formula_path, signed long& number_of_threads, bool& is_video_enabled);
	//check input
	//return 0 if succeed
	//return error number otherwise
	static int checkInput(const int argc, char* argv[], string& config_path, string& config_path_with_filename, Configuration& configuration,
			string& score_formula_path, string& score_path_with_filename, void*& score_file_handle,
			calc_score_function& score_formula_pointer, string& algorithm_path, vector<string>& algorithm_path_list,
			string& house_path, vector<string>& house_path_list, signed long& number_of_threads, bool& is_video_enabled);
	//destructor
	~ReadAllFiles(){};
};

#endif /* READALLFILES_H_ */
