#include "AlgorithmRegistrar.h"

//========================================================================
// AlgorithmRegistrar.cpp is part of the simulation project ONLY
// You have the freedom to implement it as you wish!
// You even don't have to have such a class!
//========================================================================
// create the static field
AlgorithmRegistrar AlgorithmRegistrar::instance;

//----------------------------------------------------------------------------
// AlgorithmRegistrar::loadAlgorithm is a SUGGESTION only.
// You can implement something else instead.
// returns:  0 for success
//          -1 if file cannot be loaded
//          -2 if file doesn't register any algorithm
// (To be in AlgorithmRegistrar.cpp which is part of the simulation project)
//----------------------------------------------------------------------------
int AlgorithmRegistrar::loadAlgorithm(const std::string& path, const std::string& so_file_name_without_so_suffix) {
	size_t size = instance.size();
	//get full path with filename to algorithm
	string file_name = ReadAllFiles::getFullPathWithFileName(path, so_file_name_without_so_suffix, ".so");
	//create handle for current shared object
    void* handle;
   	//try to load shared object
    handle = dlopen(file_name.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    //if dlopen() failed
    if (!handle) {
    	//clear any existing error
    	dlerror();
    	return FILE_CANNOT_BE_LOADED;
    }
    //dlopen() succeed - add handle to vector
    else {
    	handles.push_back(handle);
    }
    if(instance.size() == size) {
    	//no algorithm registered
        return NO_ALGORITHM_REGISTERED;
    }
    //give new loaded algorithm a name
    instance.setNameForLastAlgorithm(so_file_name_without_so_suffix);
    return ALGORITHM_REGISTERED_SUCCESSFULY;
}

//free opened handles for loaded shared objects - return error string
string AlgorithmRegistrar::freeHandles() {
	//store possible errors
	string errors = "";
	//empty vector of factories
	algorithmFactories.clear();
	for (size_t i = 0; i < handles.size(); i++) {
		int result = dlclose(handles[i]);
		//check for errors - if no errors, result == 0
		if (result) {
			//add error to string of errors
			errors += "error in closing handle #" + to_string(i) + ": " + string(dlerror()) + '\n';
		}
	}
	//if any errors collected
	if (errors.compare("")) {
		//remove last '\n'
		errors = errors.substr(0, errors.length() - 1);
	}
	//return stored errors
	return errors;
}
