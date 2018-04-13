/* myclient.cc: sample client program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
#include "messagehandler.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <algorithm>

using namespace std;

enum commands {

};

/*
 * Send an integer to the server as four bytes.
 */
void writeNumber(const Connection& conn, int value) {
	conn.write((value >> 24) & 0xFF);
	conn.write((value >> 16) & 0xFF);
	conn.write((value >> 8)	 & 0xFF);
	conn.write(value & 0xFF);
}

/*
 * Read a string from the server.
 */
string readString(const Connection& conn) {
	string s;
	char ch;
	while ((ch = conn.read()) != '$') {
		s += ch;
	}
	return s;
}


/*Prints available commands*/
void printCommands() {
	cout << "============================================================================"<< endl;
	cout << "The client program generally takes commands on the format \n<command> <option> <parameter> where either some or all parameters are necessary." << endl;
	cout << "The available options are \'-ng\' for Newsgroups and \'-art\' for article." << endl;
	cout << "Below are the available commands for the client application." << endl;
	cout << "-List of newsgroups: list -ng" << endl;
	cout << "-Create a newsgroup: create -ng <newsgroup name>" << endl;
	cout << "-Delete a newsgroup: delete -ng <group identification number>" << endl;
	cout << "-List of articles in a newsgroup: list -art <group identification number>" << endl;
  cout << "-Create an article: create -art <group identification number> <title> <author> <text>" << endl;
  cout << "-Delete an article: delete -art <group identification number> <article identification number>" << endl;
	cout << "-Get an article: get -art <group identification number> <article identification number>" << endl;
	cout << "============================================================================"<< endl;
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		cerr << "Usage: news_client host-name port-number" << endl;
		exit(1);
	}

	int port = -1;
	try {
		port = stoi(argv[2]);
	} catch (exception& e) {
		cerr << "Wrong port number. " << e.what() << endl;
		exit(1);
	}

	Connection conn(argv[1], port);
	if (!conn.isConnected()) {
		cerr << "Connection attempt failed" << endl;
		exit(1);
	}

	cout << "Type a command. Use command \'help\' for listing of commands." << endl;
	string cmd;
	while (cin >> cmd) {
		try {
				// cout << nbr << " is ...";
				// writeNumber(conn, nbr);
				// string reply = readString(conn);
				// cout << " " << reply << endl;
				// cout << "Type another number: ";
					if(cmd == "help"){
						printCommands();
					}else{
						cout << "Unrecognized command, please try again." << endl;
					}
				} catch (ConnectionClosedException&) {
						cout << " no reply from server. Exiting." << endl;
						exit(1);
				}
	}
}
