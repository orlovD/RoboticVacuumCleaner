#include "Montage.h"

//return
int Montage::compose(const vector<string> &images, int columns, int rows, const string& composed_image_path) {
	string montage_cmd = "montage -geometry 60x60 -tile " + to_string(columns) + "x" + to_string(rows) + " ";
	for (auto &path : images) {
		montage_cmd += path + " ";
	}
	montage_cmd += composed_image_path;
	//montage returns '0' on success and '1' and error description on failure
	//system returns exit status of command on success and '-1' on failure
	return system(montage_cmd.c_str());
}
