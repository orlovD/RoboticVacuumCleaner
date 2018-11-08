#ifndef __HOUSE__HPP_
#define __HOUSE__HPP_

#include "Point.h"
#include "ReadAllFiles.h"
#include "Montage.h"
#include "Utils.h"

class House {
private:	
	//internal description of a house, not for print with results
	string name_and_desc;
	//max steps for simulation on given house
	signed long max_steps;
	//string* matrix;
	vector<string> matrix;
	signed long rows;
	signed long columns;
	//matrix set line
	void setlineMatrix(const int i, const string& s);
	//initialize matrix
	void initMatrix(const int r);
	//cell value getter
	void setCellValue(const int i, const int j, const char c);
	//rows setter
	void setRows(const int r);
	//columns setter
	void setColumns(const int c);
	//max_steps setter
	void setMaxSteps(const int steps);

public:
	//cell value getter
	char const getCellValue(const int i, const int j) const;
	//empty constructor
//	House() : max_steps(0), matrix(NULL), R(0), C(0) {};
	House() : max_steps(0), rows(0), columns(0) {};
	//copy constructor
	House(const House& other);
	//constructor
	House(const string& file);
	//matrix getter
	vector<string> getMatrix() const {
		return this->matrix;
	}
	//rows getter
	signed long const getRows() const;
	//columns getter
	signed long const getColumns() const;
	//name and description getter
	string const getNameAndDesc() const;
	//MaxSteps getter
	int const getMaxSteps() const;
	//name_and_desc setter
	void setName(const string s);
	//description setter
	void setNameAndDesc(const string& s);
	//add missing spaces to house map if R and C is larger than 0
	void checkMissingSpaces();
	//calculate total dirt
	int const sumDirt() const;
	//find docking station
	Point const findDocking() const;
	//calculate number of docking stations
	int const calculateDocking() const;
	//found negative lines
	string const checkLines(const string& house_filename) const;
	//print house
	void print() const;
	//add walls if R and C is larger than 0
	void checkWalls();
	//decrease dirt at point
	void decreaseDirtAtLocation(const Point& location);
	//check if there is a dirt
	bool const isLocationHasDirt(const Point& location) const;
	//get dirt at point
	int const getDirtAtLocation(const Point& location) const;
	//create simple1.house
	static void createSimple1();
	//check wall at point
	bool const hasWallAt(const int y, const int x) const;

	//destructor
	~House() {};
};

#endif /* __HOUSE__HPP_ */
