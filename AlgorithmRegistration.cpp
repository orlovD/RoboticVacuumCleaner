// AlgorithmRegistration.h is part of BOTH the simulation project
// and the Algorithm projects
#include "Direction.h"
#include "AbstractAlgorithm.h"

#include "AlgorithmRegistration.h"
#include "AlgorithmRegistrar.h"

#include "Utils.h"

//========================================================================
// AlgorithmRegistration.cpp is part of the simulation project ONLY
// You MUST implement ONE and ONLY ONE method, with this exact signature.
// But you have the freedom to implement it as you wish!
//========================================================================
AlgorithmRegistration::AlgorithmRegistration(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
    AlgorithmRegistrar::getInstance().registerAlgorithm(algorithmFactory);
}
