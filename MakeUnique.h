#ifndef MAKEUNIQUE_H_
#define MAKEUNIQUE_H_

#include <iostream>
#include <memory>

//make_unique is present
#ifdef __cpp_lib_make_unique
//make_unique is not present
#else
    //implementation of make_unique for solving problems with unique_ptr at compilation time in older versions of g++
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
    	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
#endif

#endif /* MAKEUNIQUE_H_ */
