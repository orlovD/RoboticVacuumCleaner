#include "Video.h"

//constructor
Video::Video(House* house, const string& algorithm_name, const string& house_name) :
	is_folder_created(true), frame_number(0), count_failed_images(0), is_video_created(true), is_folder_removed(true) {
	this->house = house;
	this->algorithm_name = algorithm_name;
	this->house_name = house_name;
	errors_during_video_creation.clear();
}

//check folder creating for errors
void Video::checkFolderCreating(int error_number, const string& folder_name) {
	//check for errors
	if (error_number) { //will be '0' if succeeded
		//folder creating failed
		is_folder_created = false;
		//create error message
		string error_in_folder_creation =
				"Error: In the simulation " + algorithm_name + ", " + house_name + ": folder creation " + folder_name + " failed";
		//add error message to vector
		errors_during_video_creation.push_back(error_in_folder_creation);
	}
}

//check image creating for errors
void Video::checkImageCreating(int result, const string& composed_image) {
	//check if 'system' failed - exit value '-1'
	if (result == -1) {
		//creation of current frame failed - increase number of failed images
		count_failed_images++;
	} else		//if 'system' succeeded - exit value '0'
	{
		//check if image created
		if (!ReadAllFiles::isFileExists(composed_image)) {
			//creation of current frame failed - increase number of failed images
			count_failed_images++;
		}
	}
}

//check video creating for errors
void Video::checkVideoCreating(int result, const string& video_file_name) {
	//check if 'system' failed - exit value '-1'
	if (result == -1) {
		//video creating failed
		is_video_created = false;
	}
	else { //if 'system' succeeded - exit value '0'
		//check if video created
		if (!ReadAllFiles::isFileExists(video_file_name)) {
			//video creating failed
			is_video_created = false;
		}
	}
    //if video creating failed
    if(!is_video_created) {
	   	//create error message
	   	string error_in_video_creating =
	   			 "Error: In the simulation " + algorithm_name + ", " +  house_name + ": video file creation failed";
	   	//add error message to vector
		errors_during_video_creation.push_back(error_in_video_creating);
    }
}

//check folder removing for errors
void Video::checkFolderRemoving(int error_number, const string& simulation_directory) {
	//check folder removing for errors
	if (error_number) {
		//will be '0' if succeeded
		//folder removing failed
		is_folder_removed = false;
		//create error message
		string error_in_folder_removing =
				"Error: In the simulation " + algorithm_name + ", " + house_name + ": folder removing " + simulation_directory + " failed";
		//add error message to vector
		errors_during_video_creation.push_back(error_in_folder_removing);
	}
}

//create description of the house on current step
void Video::montage(const Point& robot_location) {
	//save description of each point in house
	vector<string> tiles;
	//get current frame of the house
    for (size_t row = 0; row < (size_t)house->getRows(); ++row) {
    	for (size_t column = 0; column < (size_t)house->getColumns(); ++column) {
    		//if robot is located at the point
    		if (row == (size_t)robot_location.getY() && column == (size_t)robot_location.getX()) {
    			//add picture of a robot
    			tiles.push_back("R");
    		}
    		//if robot is not located at the point
    		else {
				//if value of point in matrix is not a number from 0 to 9 and not 'W' - dust level there is 0
				if (!isdigit(house->getMatrix()[row][column]) && house->getMatrix()[row][column] != 'W' && house->getMatrix()[row][column] != 'D') {
					//add picture with 0 dust
					tiles.push_back("0");
				}
				//if value is a number from 0 to 9 or 'W'
				else {
					//add appropriate picture
					tiles.push_back(string() + house->getMatrix()[row][column]);
				}
    		}
    	}
    }
    //folder name
    string folder_name = algorithm_name + "_" + house_name;
    //directory name
    string images_directory_path = "simulations/" + folder_name;
    //create directory by given name if not exists yet
    int error_number = ReadAllFiles::createDirectoryIfNotExists(images_directory_path);
    //check folder creating for errors
	checkFolderCreating(error_number, folder_name);
    //count images on current house
    string counter_string = to_string(frame_number++);
    //set image name
    string composed_image = images_directory_path + "/image" + string(5-counter_string.length(), '0') + counter_string + ".jpg";
    //compose created images into one
    int result  = Montage::compose(tiles, house->getColumns(), house->getRows(), composed_image);
    //check image creating for errors
	checkImageCreating(result, composed_image);
}

//encode images into video
void Video::encodeImagesIntoVideo() {
	//check for errors in creation of images
	if (this->count_failed_images > 0) {
		//create error message
		string error_in_image_creation =
			"Error: In the simulation " + algorithm_name + ", " + house_name + ": the creation of " + to_string(count_failed_images) + " images was failed";
    	//add error message to vector
		errors_during_video_creation.push_back(error_in_image_creation);
	}
	//get simulation directory
	string simulation_directory =  "simulations/" + algorithm_name + "_" + house_name + "/";
	//get images names
	string images_expression = simulation_directory + "image%5d.jpg";
	string video_file_name = algorithm_name + "_" + house_name + ".mpg";
	//encoder created images into .mpg
	int result = Encoder::encode(images_expression, video_file_name);
    //check video creating for errors
	checkVideoCreating(result, video_file_name);
	//remove directory with images after creating video file
	int error_number = ReadAllFiles::removeDirectory(simulation_directory);
	//check folder removing for errors
	checkFolderRemoving(error_number, simulation_directory);
}




