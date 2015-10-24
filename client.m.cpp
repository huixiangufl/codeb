/*
* File:   main.cpp
* Author: atamarkin2
*
* Created on June 26, 2014, 5:11 PM
*/

#include <string>
#include "galik_socketstream.h"
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;

using namespace galik;
using namespace galik::net;



/*
*
*/
/*
int main(int argc, char** argv) {
	if (argc < 1) {
		cout << "args: <command>" << endl;
		cout << argc;
		exit(1);
	}

	//string name(argv[1]);
	string name("MXA");
	//string password(argv[2]);
	string password("mxa12345");

	socketstream ss;
	//ss.open(argv[3], atoi(argv[4]));
	ss.open("codebb.cloudapp.net", 17429);

	string command("");
	for (int i = 1; i < argc; i++) {
		command += argv[i];
		command += " ";
	}
	command;

	ss << name << " " << password << "\n" << command << "\nCLOSE_CONNECTION" << endl;

	while (ss.good() && !ss.eof()) {
		string line;
		getline(ss, line);
		cout << line << endl;
	}
	return 0;
}
*/
