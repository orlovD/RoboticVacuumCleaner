#ifndef SRC_VIDEO_H_
#define SRC_VIDEO_H_

#include "House.h"
#include "Point.h"
#include "ReadAllFiles.h"
#include "Montage.h"
#include "Encoder.h"
#include "Utils.h"

class Video {
	//gives friend access to Simulation
	friend class Simulation;
private:
	//pointer to current house
	House* house;
	//save algorithm name
	string algorithm_name;
	//save house name
	string house_name;
	//will be false if folder creation failed
	bool is_folder_created;
	//count frames
	size_t frame_number;
	//count failed images
	size_t count_failed_images;
	//will be false if video creation failed
	bool is_video_created;
	//will be false if folder removing failed
	bool is_folder_removed;
	//save errors during creation of video
	vector<string> errors_during_video_creation;
	//check folder creating for errors
	void checkFolderCreating(int error_number, const string& folder_name);
	//check image creating for errors
	void checkImageCreating(int result, const string& composed_image);
	//check video creating for errors
	void checkVideoCreating(int result, const string& video_file_name);
	//check folder removing for errors
	void checkFolderRemoving(int error_number, const string& simulation_directory);
public:
	//constructor
	Video() : house(NULL), is_folder_created(true), frame_number(0), count_failed_images(0), is_video_created(true), is_folder_removed(true) {};
	Video(House* house, const string& algorithm_name, const string& house_name);
	//create description of the house on current step
	void montage(const Point& robot_location);
	//encode images into video
	void encodeImagesIntoVideo();
	//destructor
	~Video() {};
};

#endif /* SRC_VIDEO_H_ */
