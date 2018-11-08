#include "Encoder.h"

#include "Utils.h"

//compose images of current frame points into single image
//return negative on failure
int Encoder::encode(const string& images_path, const string& video_output) {
	//create command string
//	string ffmpegCmd = "ffmpeg -y -loglevel fatal -i " + images_path + " " + video_output;
	string ffmpegCmd = "ffmpeg -y -i " + images_path + " " + video_output;
	//ffmpeg returns negative value on failure
	//system returns exit status of command on success and '-1' on failure
	return system(ffmpegCmd.c_str());
}
