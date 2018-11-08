#ifndef _MONTAGE__H_
#define _MONTAGE__H_

#include "Utils.h"

class Montage {
public:
	//compose images of current frame points into single image
	//return '0' on success
	static int compose(const vector<string> &images, int columns, int rows, const string& composedImagePath);
};

#endif //_MONTAGE__H_
