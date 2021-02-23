#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>

using namespace std;

int readFile() {

	char filename[50];
	ifstream textFile;
	cin.getline(filename, 50);
	textFile.open(filename);

	if (!textFile.is_open()) {
		exit(EXIT_FAILURE);
	}
	
	char word[50];
	textFile >> word;
	while (textFile.good()) {
		cout << word << " ";
		textFile >> word;
	}

	system("pause");
	return 0;
}