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
#include <math.h>
#include <stdlib.h>

using namespace std;

/*Prints available commands*/
void printCommands() {
	cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<< endl;
	cout << "The client program generally takes commands on the format <command> <option>\n<parameter1> <parameter2>... where either some or all parameters are required." << endl;
  cout << "Add double quotes \"Text goes here\" to input several words as one argument to a command." << endl;
	cout << "The available options are \'-ng\' for Newsgroups and \'-art\' for article." << endl;
	cout << endl;
	cout << "Below are the available commands for the client application:" << endl;
	cout << "-List of newsgroups: list -ng" << endl;
	cout << "-Create a newsgroup: create -ng \"<newsgroup name>\"" << endl;
	cout << "-Delete a newsgroup: delete -ng <group identification number>" << endl;
	cout << "-List of articles in a newsgroup: list -art <group identification number>" << endl;
  cout << "-Create an article: create -art <group identification number> \"<title>\" \"<author>\" \"<text>\"" << endl;
  cout << "-Delete an article: delete -art <group identification number> <article identification number>" << endl;
	cout << "-Get an article: get -art <group identification number> <article identification number>" << endl;
	cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<< endl;
}

/*Prints a list of groups and articles for ListGroup and ListArticle command*/
void printList(vector<pair<int, string>> list){
	if(list.size() == 0){
		cout << "No results found." << endl;

	}else{
		cout << "+++++++++++++++++++++++++++++++++++++++++++++"<< endl;
		cout << "+ ID    Name                                +" << endl;
		cout << "+                                           +" << endl;
		for(const auto& p : list){
			int nameLength = p.second.length();
			int restLength = nameLength - 34;
			if(nameLength > 34){
				string subName1 = p.second.substr(0, 34);
				string subName2 = p.second.substr(34, restLength);
				string spaces1(38 - to_string(p.first).length() - subName1.length(), ' ');
				string spaces2(70 - nameLength, ' ');
				cout << "+ " << p.first << "    " << subName1 << spaces1 << "+" << endl;
				cout << "+       " << subName2 << spaces2 << "+" << endl;
			}else{
				string spaces(38 - to_string(p.first).length() - p.second.length(), ' ');
				cout << "+ " << p.first << "    " << p.second << spaces << "+" << endl;
			}
		}
		cout << "+++++++++++++++++++++++++++++++++++++++++++++"<< endl;
	}
}

void displayArticle(vector<string> article){
	if(article.size() == 0){
		cout << "No results found." << endl;
	}else{
		string title = article[0];
		string author = article[1];
		string text = article[2];
		cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<< endl;
		if(title.size()%2 != 0){
			/*Even out spaces*/
			title += " ";
		}
		string titleSpaces((56-title.size())/2, ' ');
		cout << "+ " << titleSpaces << title << titleSpaces << "  +" << endl;
		if(author.size()%2 != 0){
			/*Even out spaces*/
			author += " ";
		}
		string authorSpaces((47-author.size())/2, ' ');
		cout << "+" << authorSpaces << "written by "  << author << authorSpaces << "  +" << endl;
		cout << "+                                                           +" << endl;
		for(int i = 0; i < text.size(); i+=50){
			string subText;
			if((i+50) > text.size()){
				subText = text.substr(i, text.size()%50);
			}else{
				subText = text.substr(i, 50);
			}

			if(subText.size()%2 != 0){
				/*Even out spaces*/
				subText += " ";
			}
			string textSpaces((58-subText.size())/2, ' ');
			cout << "+" << textSpaces << subText << textSpaces << " +" << endl;
		}
		cout << "+                                                           +" << endl;
		cout <<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<< endl;
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
			if (p.at(p.size()-1) != '\"'){
				do{
					string newArgument;
					ss >> newArgument;
					p += " " + newArgument;
				}while(p.at(p.size()-1) != '\"');
			}
			p.erase(std::remove(p.begin(), p.end(), '\"'), p.end());
	}
		parameters.push_back(p);
	}

	if(parameters.size() != nbrOfParam){
		throw std::invalid_argument("Invalid number of parameters given. Type \'help\' for list of commands.");
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

	cout << "Type a command. Use command \'help\' for listing of commands.\nUse double quotes as \"argument\" for multiple word capture of argument." << endl;
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
						printList(groupList);
					}
					else if(option == "-art"){
						/*List the articles in a news group*/
						 parameters = readCommandParameters(ss, 1);
						 int groupId = stoi(parameters[0]);
						 auto articleList = ch.listNewsArticles(groupId);
						 printList(articleList);
					}
					else{
						cout << "Recognized command \'list\' but no option was specified." << endl;
						cout << "The format should be list -ng or list -art [Group ID] " << endl;
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
						cout << "Recognized command \'create\' but no option was specified." << endl;
						cout << "The format should be create -ng \"[Group name]\"  or create -art [Group ID] \"[Title]\" \"[Author]\" \"[Text]\" " << endl;
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
						parameters = readCommandParameters(ss, 2);
						ch.deleteArticle(stoi(parameters[0]), stoi(parameters[1]));
					}
					else{
						cout << "Recognized command \'delete\' but no option was specified." << endl;
						cout << "The format should be delete -ng [Group ID] or delete -art [Group ID] [Article ID]" << endl;
					}
				}

				else if(command == "get"){
					if(option == "-art"){
						/*Delete a news groups*/
						parameters = readCommandParameters(ss, 2);
						auto article = ch.getArticle(stoi(parameters[0]), stoi(parameters[1]));
						displayArticle(article);
					}
					else{
							cout << "Recognized command \'get\' but no option was specified." << endl;
							cout << "The format should be get -art [Group ID] [Article ID]" << endl;
					}
				}

				else{
					cout << "Unrecognized command, please try again." << endl;
				}
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

			catch(const std::logic_error& lr){

				cerr << "Protocol failure in " << lr.what()<< ". Incorrect sequence received." << endl;
			}

			catch (ConnectionClosedException&) {
					cout << " no reply from server. Exiting." << endl;
					exit(1);
			}
			cout << "->";
	}
}
