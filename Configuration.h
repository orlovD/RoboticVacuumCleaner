#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "Configuration.h"

#include "Utils.h"

class Configuration {
	map<string, int> parameters;
	//for missing parameters
	vector<string> missing_parameters;
	//for bad parameters
	vector<string> bad_parameters;
	//split by delimiter
	static vector<string> split(const string &s, const char delim);
	//trim string
	static string trim(string& str);
	//add line to parameters map
	void processLine(string& line);

public:
	//constructors
	Configuration() {}
	//create configuration object by path
	Configuration(const string& iniPath);
	//load configuration from file
	void loadFromFile(const string& iniPath);
	//for printing
	string const toString() const;
	//parameters file getter
	map<string, int> const getParams() const;
	//check configuration file data for missing lines
	string const checkMissingData();
	//check configuration file data for bad values
	string const checkBadParameters();
	//create configuration file
	static void writeConfigFile(const string& iniPath);
	//destructor
	~Configuration(){}
};

#endif /* CONFIGURATION_H_ */
