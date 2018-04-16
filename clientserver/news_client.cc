#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
#include "messagehandler.h"
#include "commandhandler.h"

#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <sstream>
#include <utility>

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
	cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<< endl;
	cout << "The client program generally takes commands on the format \n<command> <option> <parameter1> <parameter2>... where either some or all parameters are required." << endl;
	cout << "The available options are \'-ng\' for Newsgroups and \'-art\' for article." << endl;
	cout << "Below are the available commands for the client application." << endl;
	cout << "-List of newsgroups: list -ng" << endl;
	cout << "-Create a newsgroup: create -ng \"<newsgroup name>\"" << endl;
	cout << "-Delete a newsgroup: delete -ng <group identification number>" << endl;
	cout << "-List of articles in a newsgroup: list -art <group identification number>" << endl;
  cout << "-Create an article: create -art <group identification number> \"<title>\" \"<author>\" \"<text>\"" << endl;
  cout << "-Delete an article: delete -art <group identification number> <article identification number>" << endl;
	cout << "-Get an article: get -art <group identification number> <article identification number>" << endl;
	cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<< endl;
}

void printGroupList(vector<pair<int, string>> list){
	if(list.size() == 0){
		cout << "No results found." << endl;

	}else{
		cout << "++++++++++++++++++++++++++++++++++++++++"<< endl;
		cout << "+ ID   Name                            +" << endl;
		cout << "+                                      +" << endl;
		for(const auto& p : list){
			string spaces(33 - to_string(p.first).length() - p.second.length(), ' ');
			string tableBoundry = spaces + "+";
			cout << "+ " << p.first << "    " << p.second << tableBoundry << endl;
		}
		cout << "++++++++++++++++++++++++++++++++++++++++"<< endl;
	}
}

/*Read the available parameters from the input.*/
vector<string> readCommandParameters(istringstream& ss, int nbrOfParam){
	std::vector<string> parameters;
	string p;
	while(ss >> p){
		if (p == ""){
			throw std::invalid_argument("Invalid parameter given. Please try again.");
		}

		else if(p.at(0) == '\"'){
			do{
				string newArgument;
				ss >> newArgument;
				p += " " + newArgument;
				cout << p << endl;
			}while(p.at(p.size()-1) != '\"');
			p.erase(std::remove(p.begin(), p.end(), '\"'), p.end());
		}
		parameters.push_back(p);
	}

	if(parameters.size() != nbrOfParam){
		throw std::invalid_argument("Invalid number of parameters given. Please try again.");
	}
	return parameters;
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
	cout << "->";
	string line;
	MessageHandler mh(conn);
	CommandHandler ch(mh);
	while (getline(cin, line)) {
		istringstream ss(line);
		try {
				string command;
				string option;
				vector<string> parameters;
				ss >> command;
				ss >> option;

				if(command == "help"){
					printCommands();
				}

				else if(command == "list"){
					if(option == "-ng"){
						/*List the news groups*/
						auto groupList = ch.listNewsGroup();
						printGroupList(groupList);
					}
					else if(option == "-art"){
						/*List the articles in a news group*/
						 parameters = readCommandParameters(ss, 1);
						 int groupId = stoi(parameters[0]);
						 auto articleList = ch.listNewsArticles(groupId);
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
						ch.createNewsGroup(groupName);
					}
					else if(option == "-art"){
						/*Create an article*/
						parameters = readCommandParameters(ss, 4);
						ch.createArticle(stoi(parameters[0]), parameters[1],
														 parameters[2], parameters[3]);
					}
					else{
						cout << "Recognized command but no option was specified." << endl;
					}
				}

				else if(command == "delete"){
					if(option == "-ng"){
						/*Delete a news groups*/
						parameters = readCommandParameters(ss, 1);
						int groupId = stoi(parameters[0]);
						ch.deleteNewsGroup(groupId);
					}
					else if(option == "-art"){
						/*Delete an article*/
						parameters = readCommandParameters(ss, 1);
						int groupId = stoi(parameters[0]);
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

			catch(const std::invalid_argument& ia){
				string message = ia.what();
				if(message == "stoi"){
					cerr << "A non-integer argument was detected. Please try again." << endl;
				}
				else{
					cerr << ia.what() << endl;
				}
			}

			catch (ConnectionClosedException&) {
					cout << " no reply from server. Exiting." << endl;
					exit(1);
			}
			cout << "->";
	}
}
