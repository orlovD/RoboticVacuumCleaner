#ifndef ALGORITHMREGISTRAR_H_
#define ALGORITHMREGISTRAR_H_
#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "ReadAllFiles.h"

#include "AlgorithmRegistration.h"

#include "Utils.h"

//========================================================================
// AlgorithmRegistrar.h is part of the simulation project ONLY
// You have the freedom to implement it as you wish!
// You even don't have to have such a class!
//========================================================================
class AlgorithmRegistrar {
	//vector for handles of opened files
	vector<void*> handles;
	//changed from list to vector
	vector<std::string> algorithmNames;
	vector<std::function<unique_ptr<AbstractAlgorithm>()>> algorithmFactories;
    void registerAlgorithm(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
        instance.algorithmFactories.push_back(algorithmFactory);
    }
    void setNameForLastAlgorithm(const std::string& algorithmName) {
        assert(algorithmFactories.size()-1 == algorithmNames.size());
        algorithmNames.push_back(algorithmName);
    }
public:
    friend class AlgorithmRegistration;
    enum {ALGORITHM_REGISTERED_SUCCESSFULY = 0, FILE_CANNOT_BE_LOADED = -1, NO_ALGORITHM_REGISTERED = -2};
    int loadAlgorithm(const std::string& path, const std::string& so_file_name_without_so_suffix);
    //changed from list to vector
    vector<unique_ptr<AbstractAlgorithm>> getAlgorithms()const {
    	vector<unique_ptr<AbstractAlgorithm>> algorithms;
        for(auto algorithmFactoryFunc : algorithmFactories) {
            algorithms.push_back(algorithmFactoryFunc());
        }
        return algorithms;
    }
    //changed from list to vector
    const vector<std::string>& getAlgorithmNames()const {
        return algorithmNames;
    }
    size_t size()const {
        return algorithmFactories.size();
    }
    static AlgorithmRegistrar& getInstance() {
        return instance;
    }
    //free opened handles for loaded shared objects - return error string
    string freeHandles();
private:
    static AlgorithmRegistrar instance;
};

#endif /* ALGORITHMREGISTRAR_H_ */
