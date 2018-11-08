#include "House.h"

//all thrown exceptions will be caught and handled at Simulator::run

//constructor
House::House(const string& file) : rows(0), columns(0) {
	string tmp = "";
	signed long t;
	struct stat sb;
	//if any problem in stat()
    if (stat(file.c_str(), &sb) == -1) {
    	throw 1;
	}
    //if its not a file
    if ((sb.st_mode & S_IFMT) != S_IFREG) {
    	throw 1;
    }
	ifstream fin(file);
	//check for errors in opening file
	if ((fin.rdstate() & std::ifstream::badbit) != 0) {
		throw 1;
	}
	//throw exception if failed
	if (fin.fail() || !fin.is_open() || !fin.good()) {
		throw 1;
	}
	//if the file has no lines
	if (!getline(fin, tmp)) {
		throw 1;
	}

	setNameAndDesc(tmp);
	fin >> t;
	setMaxSteps(t);
	fin >> t;
	setRows(t);
	fin >> t;
	setColumns(t);
	fin.ignore(); //skip newline and go to the beginning of matrix
	if (this->getRows() > 0) {
		initMatrix(getRows());
		for (int i =0; i < getRows(); ++i) {
			std::getline(fin, tmp);
			setlineMatrix(i, tmp);
		}
	}
	fin.close();
}
//copy constructor
//House::House(const House& other) : matrix(NULL) {
//	this->name_and_desc = other.name_and_desc;
//	this->max_steps = other.max_steps;
//	this->C = other.C;
//	this->R = other.R;
//	this->initMatrix(other.R);
//	for (int i = 0; i < this->R; i++) {
//		string blank(C, ' ');
//		setlineMatrix(i, blank);
//		for (int j = 0; j < this->C; j++) {
//		this->matrix[i][j] = other.matrix[i][j];
//		}
//	}
//}

//copy constructor
House::House(const House& other) {
	this->name_and_desc = other.name_and_desc;
	this->max_steps = other.max_steps;
	this->columns = other.columns;
	this->rows = other.rows;
	this->columns = other.columns;
	this->rows = other.rows;
//	this->initMatrix(other.R);
//	for (int i = 0; i < this->R; i++) {
	for (int i = 0; i < this->rows; i++) {
		string blank(columns, ' ');
//		setlineMatrix(i, blank);
		matrix.push_back(blank);
		for (int j = 0; j < this->columns; j++) {
		this->matrix[i][j] = other.matrix[i][j];
		}
	}
}

//name and description getter
string const House::getNameAndDesc() const {
	return this->name_and_desc;
}
//MaxSteps getter
int const House::getMaxSteps() const {
	return this->max_steps;
}
//rows getter
signed long const House::getRows() const {
	return this->rows;
}
//columns getter
signed long const House::getColumns() const {
	return this->columns;
}
//init matrix
void House::initMatrix(const int r) {
	for (int i = 0; i < r; i++) {
		this->matrix.push_back("");
	}
//	this->matrix = new string[r];
}
//matrix set line
void House::setlineMatrix(const int i, const string& s) {
//	cout << "In setLine1" << endl;
//	cout << "string is: " << s << endl;
	this->matrix[i] = s;
//	cout << "In setLine2" << endl;
}
//cell value getter
char const House::getCellValue(const int y, const int x) const {
//	cout << this->matrix[y][x] << endl;
	return this->matrix[y][x];
}
//cell value setter
void House::setCellValue(const int y, const int x, const char c) {
	this->matrix[y][x] = c;
}
//name setter
void House::setName(const string s) {
	this->name_and_desc = s;
}
//desc setter
void House::setNameAndDesc(const string& s) {
	this->name_and_desc = s;
}
//R setter
void House::setRows(const int r) {
	this->rows = r;
}
//C setter
void House::setColumns(const int c) {
	this->columns = c;
}
//MaxSteps setter
void House::setMaxSteps(const int steps) {
	this->max_steps = steps;
}
//add missing spaces to house map if R and C is larger than 0
void House::checkMissingSpaces() {
	if ((this->getRows() > 0) && (this->getColumns() > 0)) {
		for (int i = 0; i < this->getRows(); ++i) {
			if((this->matrix[i]).length() < (size_t)this->getColumns()) {
				string blank(columns - (this->matrix[i]).length(), ' ');
				string new_line = this->matrix[i] + blank;
				setlineMatrix(i, new_line);
				for (int j = (this->matrix[i]).length(); j < this->getColumns(); ++j) {
					this->setCellValue(i, j, ' ');
				}
			}
		}
	}
}
//calculate total dirt
int const House::sumDirt() const {
	int sum_dirt_in_house = 0;
	for (int i = 0; i < this->getRows(); i++) {
		for (int j = 0; j < this->getColumns(); j++) {
			if ((int)(this->getCellValue(i,j) - '0') >= 0 && (int)(this->getCellValue(i,j) - '0') <= 9) {
				sum_dirt_in_house = sum_dirt_in_house + (int)(this->getCellValue(i,j) - '0');
			}
		}
	}
	return sum_dirt_in_house;
}
//find docking station
Point const House::findDocking() const {
	Point docking;
	for (int i = 0; i < this->getRows(); i++) {
		for (int j = 0; j < this->getColumns(); j++) {
			if (this->getCellValue(i,j) == 'D') {
				docking = Point(j, i);
				break;
			}
		}
	}
	return docking;
}
//calculate number of docking stations
int const House::calculateDocking() const {
	int num_of_docking = 0;
	//if number of rows and columns is positive
	if (this->getRows() > 0 && this->getRows() > 0) {
		//check number of docking stations
		for (int i = 0; i < this->getRows(); i++) {
			for (int j = 0; j < this->getColumns(); j++) {
				if (this->getCellValue(i,j) == 'D') {
					num_of_docking++;
				}
			}
		}
	}
	//if number of rows is not positive
	else {
		//number of docking stations cannot be checked
		num_of_docking = -1;
	}
	return num_of_docking;
}
//found negative lines
string const House::checkLines(const string& house_filename) const {
	string result = "";
	if (this->getMaxSteps() <= 0) {
		result += house_filename + ": line number 2 in house file shall be a positive number, found: " + to_string(this->getMaxSteps()) + '\n';
	}
	if (this->getRows() <= 0) {
		result += house_filename +": line number 3 in house file shall be a positive number, found: " + to_string(this->getRows()) + '\n';
	}
	if (this->getColumns() <= 0) {
		result += house_filename + ": line number 4 in house file shall be a positive number, found: " + to_string(this->getColumns()) + '\n';
	}
	return result.substr(0, result.length() - 1);
}
//print house
void House::print() const {
	cout << /* "House name and description: " << */ this->getNameAndDesc() << endl;
	cout << /* "MaxSteps: " << */ this->getMaxSteps() << endl;
	cout << this->getRows() << endl;
	cout << this->getColumns() << endl;
	for (int i = 0; i < this->getRows(); ++i) {
		for (int j =0; j < this->getColumns(); ++j) {
			cout << this->getCellValue(i,j);
		}
		cout << endl;
	}
}
//add walls if R and C is larger than 0
void House::checkWalls() {
	if ((this->getRows() > 0) && (this->getColumns() > 0)) {
		for (int i = 0; i < this->getRows(); i++) {
			if (this->getCellValue(i, 0) != 'W') {
				this->setCellValue(i, 0, 'W');
			}
			if (this->getCellValue(i, this->getColumns() - 1) != 'W') {
				this->setCellValue(i, this->getColumns() - 1, 'W');
			}
		}
		for (int i = 0; i < this->getColumns(); i++) {
			if (this->getCellValue(0, i) != 'W') {
				this->setCellValue(0, i, 'W');
			}
			if (this->getCellValue(this->getRows() - 1, i) != 'W') {
				this->setCellValue(this->getRows() - 1, i, 'W');
			}
		}
	}
}
//decrease dirt at point
void House::decreaseDirtAtLocation(const Point& location) {
	char current = getCellValue(location.getY(), location.getX());
	setCellValue(location.getY(), location.getX(), current - 1);
}
//check if there is a dirt
bool const House::isLocationHasDirt(const Point& location) const {
	int y = location.getY();	int x = location.getX();	char cellValue = getCellValue(y, x);
	return (cellValue > '0') && (cellValue <= '9');
}
//get dirt at point
int const House::getDirtAtLocation(const Point& location) const {
	if (!isLocationHasDirt(location)) {
		return 0;
	}
	return getCellValue(location.getY(),location.getX()) - '0';
}
//check wall at point
bool const House::hasWallAt(const int y, const int x) const {
	return getCellValue(y, x) == 'W';
}
//create simple1.house
void House::createSimple1() {
	cout << "Creating file: simple1.house" << endl;
	ofstream fout("simple1.house");
	fout << "Simple1" << endl;
	fout << "2 Bedrooms + Kitchen Isle" << endl;
	fout << 8 << endl;
	fout << 10 << endl;
	fout << "WWWWWWWWWW" << endl;
	fout << "W22  DW59W" << endl;
	fout << "W  W 1199W" << endl;
	fout << "W WWW3WW W" << endl;
	fout << "W6   3W  W" << endl;
	fout << "W78WTEWSTW" << endl;
	fout << "W99W 8W6 W" << endl;
	fout << "WWWWWWWWWW" << endl;
	fout.close();
}
