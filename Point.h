#ifndef __POINT__H_
#define __POINT__H_

#include "Utils.h"

class Point {
	//x coordinate
	signed long x;
	//y coordinate
	signed long y;
public:
	//constructor
	Point();
	Point(const signed long x, const signed long y);
	//== overloading
	bool operator == (const Point& B) const;
	//!= overloading
	bool operator != (const Point& B) const;
	//set X coordinate
	void setX(const signed long x);
	//set Y coordinate
	void setY(const signed long y);
	//increase X coordinate
	void increaseX();
	//increase Y coordinate
	void increaseY();
	//decrease X coordinate
	void decreaseX();
	//decrease Y coordinate
	void decreaseY();
	//get X coordinate
	signed long const getX() const;
	//get Y coordinate
	signed long const getY() const;
	//print point
	string const toString() const;
	//destructor
	~Point() {};
};

#endif /* __POINT__H_ */
