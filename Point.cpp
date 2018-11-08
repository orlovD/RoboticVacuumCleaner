#include "Point.h"

#include "Utils.h"

//constructor
Point::Point() : x(-1), y(-1) {}
Point::Point(const signed long x, const signed long y) {
	this->x = x;
	this->y = y;
}
//== overloading
bool Point::operator== (const Point& B) const{
	return getX() == B.getX() && getY() == B.getY();
}
//!= overloading
bool Point::operator!= (const Point& B) const {
	return !(*this == B);
}
//set X coordinate
void Point::setX(const signed long x) {
	this->x = x;
}
//set Y coordinate
void Point::setY(const signed long y) {
	this->y = y;
}
//increase X coordinate
void Point::increaseX() {
	this->x++;
}
//increase Y coordinate
void Point::increaseY() {
	this->y++;
}
//decrease X coordinate
void Point::decreaseX() {
	this->x--;
}
//decrease Y coordinate
void Point::decreaseY() {
	this->y--;
}
//get X coordinate
signed long const Point::getX() const {
	return this->x;
}
//get Y coordinate
signed long const Point::getY() const {
	return this->y;
}
//print point
string const Point::toString() const {
	return "X=" + to_string(this->x) + " " + "Y=" + to_string(this->y);
}

