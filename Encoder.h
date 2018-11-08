#ifndef _ENCODER__H_
#define _ENCODER__H_

#include "Utils.h"

class Encoder
{
public:
	//compose images of current frame points into single image
	//return '0' on success
    static int encode(const string& imagesString, const string& videoOutput);
};

#endif //_ENCODER__H_
