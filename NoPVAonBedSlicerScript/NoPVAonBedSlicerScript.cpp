#include <iostream>
#include <string>
#include <fstream>

using namespace std;

#define ADJUSTEDSTRING "_NoSolubleFirstLayer"
#define EXTENSION ".gcode"
#define LINEBUFFERMAX 1024
#define NEWPLATOOL "T0"
#define SOLUTOOL "T4"
#define MSETTEMP "M104 S"
#define MWAITTEMP "M109 S"
#define PLATEMP "215"
#define TOOLCHANGECHANGES 2
#define LINEFEED '\n'

void removeNewLineChars(string& str) {
	str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
	str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
}

int main(int argc, char* argv[]) {

	string buf = argv[1];
	string filename = "";
	int backslashindex = buf.length();
	for (; backslashindex > 0; backslashindex--) {
		if (buf[backslashindex] == '\\')
			break;
	}
	for (int k = backslashindex + 1; k < buf.length(); k++) {
		filename += buf[k];
	}
	cout << "File is named ' " << filename << "'" << endl;

	fstream originalgcode(filename);
	fstream newgcode(filename + ADJUSTEDSTRING + EXTENSION, std::ios_base::binary | std::ios::out);
	if (!originalgcode.is_open() || !newgcode.is_open())
		cerr << "Unable to open file!" << endl;

	int foundToolLines = 0;
	bool foundTempLine1 = false;
	bool foundTempLine2 = false;
	string linebuffer("", LINEBUFFERMAX);
	originalgcode.seekg(0, ios_base::beg);
	newgcode.seekg(0, ios_base::beg);
	
	while (!originalgcode.eof()) {
		bool needDefault = true;
		std::getline(originalgcode, linebuffer);
		removeNewLineChars(linebuffer);
		
		if (foundToolLines < TOOLCHANGECHANGES && linebuffer.substr(0, strlen(SOLUTOOL)) == SOLUTOOL) {
			newgcode << NEWPLATOOL << LINEFEED;
			foundToolLines++;
			needDefault = false;
		}
		if (!foundTempLine1 && linebuffer.substr(0, strlen(MSETTEMP)) == MSETTEMP) {
			newgcode << MSETTEMP << PLATEMP << LINEFEED;
			foundTempLine1 = true;
			needDefault = false;
		}
		if (!foundTempLine2 && linebuffer.substr(0, strlen(MWAITTEMP)) == MWAITTEMP) {
			newgcode << MWAITTEMP << PLATEMP << LINEFEED;
			foundTempLine2 = true;
			needDefault = false;
		}
		
		if(needDefault)
			newgcode << linebuffer << LINEFEED;
	}

	if (foundToolLines == TOOLCHANGECHANGES && foundTempLine1 && foundTempLine2)
		cout << "All Success!" << endl;
	else cerr << "Warning: Issues: " << foundToolLines << foundTempLine1 << foundTempLine2 << endl;
	originalgcode.close();
	newgcode.close();
	while (true);
	return 0;
}
