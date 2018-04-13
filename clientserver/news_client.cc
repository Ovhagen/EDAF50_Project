/* myclient.cc: sample client program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
#include "messagehandler.h"

#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

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

/*Read the available parameters from the input.*/
vector<string> readCommandParameters(istringstream& ss, int nbrOfParam){
	std::vector<string> parameters;
	for(int i = 0; i < nbrOfParam; i++){
		string p;
		ss >> p;
		parameters.push_back(p);
	}
	return parameters;
}

bool checkCode(int code, int protoCode){
	if(code == protoCode){
		return true;
	}
	else{
		throw std::bad_exception();
	}

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

	std::shared_ptr<Connection> conn = std::make_shared<Connection>(argv[1], port);
	if (!conn->isConnected()) {
		cerr << "Connection attempt failed" << endl;
		exit(1);
	}

	cout << "Type a command. Use command \'help\' for listing of commands." << endl;
	string line;
	MessageHandler mh(conn);
	while (getline(cin, line)) {
		istringstream ss(line);
		try {
				string command;
				string option;
				vector<string> parameters;
				ss >> command;
				ss >> option;
				cout << "Command: " << command << endl;
				cout << "Option: " << option << endl;

				if(command == "help"){
					printCommands();
				}

				else if(command == "list"){
					if(option == "-ng"){
						/*List the news groups*/
					}
					else if(option == "-art"){
						/*List the articles in a news group*/
						 parameters = readCommandParameters(ss, 1);
						 int groupId = stoi(parameters[0]);
						 cout << "Groupid: " << groupId << endl;
					}
					else{
						cout << "Recognized command but no option was specified." << endl;
					}
				}

				else if(command == "create"){
					if(option == "-ng"){
						/*Create a news groups*/
						parameters = readCommandParameters(ss, 1);
						string groupName = parameters[0];
						mh.sendCode(static_cast<int>(Protocol::COM_CREATE_NG));
						mh.sendStringParameter(groupName);
						mh.sendCode(static_cast<int>(Protocol::COM_END));

						/*Start receiving of command*/
						int ansCode = mh.recvCode();
						checkCode(ansCode, static_cast<int>(Protocol::ANS_END));

						int response = mh.recvCode();
						if(checkCode(response, static_cast<int>(Protocol::ANS_ACK))){
							cout << "Successfully created the group \'" << groupName << "\'" << endl;
						} else if(checkCode(response, static_cast<int>(Protocol::ANS_NAK))){
							int errorCode = mh.recvCode();
							if(checkCode(errorCode, static_cast<int>(Protocol::ERR_NG_ALREADY_EXISTS))){
								cout << "Request failed. A group with the name \'" << groupName << "\' already exists." << endl;
							}
						}

						int endCode = mh.recvCode();
						checkCode(endCode, static_cast<int>(Protocol::ANS_END));
					}
					else if(option == "-art"){
						/*Create an article*/
						parameters = readCommandParameters(ss, 1);
						int groupId = stoi(parameters[0]);
						cout << groupId << endl;
					}
					else{
						cout << "Recognized command but no option was specified." << endl;
					}
				}


				else if(command == "delete"){
					if(option == "-ng"){
						/*Delete a news groups*/
					}
					else if(option == "-art"){
						/*Delete an article*/
						parameters = readCommandParameters(ss, 1);
						int groupId = stoi(parameters[0]);
						cout << groupId << endl;
					}
					else{
						cout << "Recognized command but no option was specified." << endl;
					}
				}
				else{
					cout << "Unrecognized command, please try again." << endl;
				}


			}

			catch(const std::bad_exception& ba){
				cerr << "Protocol failure in " << ba.what()<< ". Incorrect sequence received." << endl;
			}

			catch(std::invalid_argument&){
					cerr << "Invalid parameter given. Please try again." << endl;
			}

			catch (ConnectionClosedException&) {
					cout << " no reply from server. Exiting." << endl;
					exit(1);
			}
	}
}
