#include "Configuration.h"

//all thrown exceptions will be caught and handled at main

//split by delimiter
vector<string> Configuration::split(const string &s, const char delim) {
	vector<string> elems;
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

//trim string
string Configuration::trim(string& str) {
	str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
	str.erase(str.find_last_not_of(' ')+1);         //suffixing spaces
	return str;
}

//add line to parameters map
void Configuration::processLine(string& line) {
	bool is_numeric = true;
	bool is_added_to_vector = false;
	vector<string> tokens = split(line, '=');
	string trimmed_parameter = "";
	//if any symbols are present on current line
	if (tokens.size() > 0) {
		//try to trim first token
		try {
			trimmed_parameter = trim(tokens[0]);
		}
		//if exception thrown - handle
		catch (exception& e) {
			//if there was an exception, it's error in trim - continue to next line
			return;
		}
	}
	else {
		//continue to next line
		return;
	}
	if (tokens.size() != 2) {
		//if parameter is not known - discard this parameter
		if (!(trimmed_parameter == "MaxStepsAfterWinner" || trimmed_parameter == "BatteryRechargeRate" || trimmed_parameter == "BatteryConsumptionRate" || trimmed_parameter == "BatteryCapacity")) {
			//line cannot be split in two parts as it should
			return;
		}
		//parameter is known but no value provided
		else {
			//add bad parameter to vector
			bad_parameters.push_back(trimmed_parameter);
			return;
		}
	}
	//check only parameters that should be in config.ini
	if (trimmed_parameter == "MaxStepsAfterWinner" || trimmed_parameter == "BatteryRechargeRate" || trimmed_parameter == "BatteryConsumptionRate" || trimmed_parameter == "BatteryCapacity") {
		//if value is not provided
		if (tokens[1].size() == 0) {
			//this is not a number
			is_numeric = false;
		}
		//check if the value is a number
		for (size_t i = 0; i < tokens[1].size(); i++) {
			if (!isdigit(tokens[1].at(i)) && !isspace(tokens[1].at(i))) {
				//this is not a number
				is_numeric = false;
			}
		}
		//if its not a number
		if (!is_numeric) {
			//add bad parameter to vector
			bad_parameters.push_back(trimmed_parameter);
			//set flag
			is_added_to_vector = true;
		}
		//check for other fails
		try {
			this->parameters[trimmed_parameter] = stoi(tokens[1]);
		}
		//if exception thrown - handle
		catch (exception& e) {
			//if not added before - add it now
			if (!is_added_to_vector) {
				//add bad parameter to vector
				bad_parameters.push_back(trimmed_parameter);
			}
		}
	}
	//if parameter is not known - continue to next line
	else {
		return;
	}
}

///create configuration object by path
Configuration::Configuration(const string& iniPath) {
	this->loadFromFile(iniPath);
}

//load configuration from file
void Configuration::loadFromFile(const string& iniPath) {
	this->parameters.clear();
	ifstream fin(iniPath);
	//check for errors in opening file
	if ((fin.rdstate() & ifstream::badbit) != 0) {
		throw 1;
	}
	//if file not found
	if (errno) {
		//throw error number as exception
		throw errno;
	}
	string line;
	while (getline(fin, line)) {
		this->processLine(line);
	}
	fin.close();
}

//for printing
string const Configuration::toString() const {
   string out;
   for (map<string, int>::const_iterator itr = this->parameters.begin(); itr != this->parameters.end(); ++itr) {
	   if (itr !=  this->parameters.begin()) {
		   out += "\n";
	   }
	   out += "parameters[" + itr->first + "] = " + to_string(itr->second);
   }
   return out;
}
//parameters file getter
map<string, int> const Configuration::getParams() const {
	return this->parameters;
}

//check configuration file data for missing lines
string const Configuration::checkMissingData() {
	string missing_data = "";
	int num_of_params = 0;
	if (!this->parameters.count("MaxStepsAfterWinner")) {
		num_of_params++;
		missing_data.append("MaxStepsAfterWinner, ");
	}
	if (!this->parameters.count("BatteryCapacity")) {
		num_of_params++;
		missing_data.append("BatteryCapacity, ");
	}
	if (!this->parameters.count("BatteryConsumptionRate")) {
		num_of_params++;
		missing_data.append("BatteryConsumptionRate, ");
	}
	if (!this->parameters.count("BatteryRechargeRate")) {
		num_of_params++;
		missing_data.append("BatteryRechargeRate, ");
	}
	if (num_of_params > 0) {
		//remove ',' from the end of string
		missing_data = missing_data.substr(0, missing_data.length() - 2);
		//string result = result.append()
		return "config.ini missing " + to_string(num_of_params) + " parameter(s): " + missing_data;
	}
	return missing_data;
}

//check configuration file data for negative values
string const Configuration::checkBadParameters() {
	string bad_values = "";
	//if there are any bad parameters
	if (bad_parameters.size() > 0) {
		bad_values.append("config.ini having bad values for ");
		//add number of parameters to error string
		bad_values.append(to_string(bad_parameters.size()));
		bad_values.append(" parameter(s): ");
		//add parameters to error string
		for (size_t i = 0; i < bad_parameters.size(); i++) {
			bad_values.append(bad_parameters[i]);
			bad_values.append(", ");
		}
		//remove ',' from the end of string
		bad_values = bad_values.substr(0, bad_values.length() - 2);
	}
	return bad_values;
}

//create configuration file
void Configuration::writeConfigFile(const string &iniPath) {
	ofstream fout(iniPath.c_str());
	fout << "BatteryConsumptionRate  =    1" << endl;
	fout << "MaxSteps   = 1200" << endl;
	fout << "MaxStepsAfterWinner=200" << endl;
	fout << "BatteryCapacity=400" << endl;
	fout << "BatteryRechargeRate=20" << endl;
	fout.close();
	ifstream fin(iniPath.c_str());
	string line;
}
