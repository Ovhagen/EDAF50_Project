/* myserver.cc: sample server program */
#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
#include "messagehandler.h"
#include "dbinmemory.h"


#include <memory>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>

using namespace std;

/*
 * Read an integer from a client.
 */
int readNumber(const shared_ptr<Connection>& conn) {
	unsigned char byte1 = conn->read();
	unsigned char byte2 = conn->read();
	unsigned char byte3 = conn->read();
	unsigned char byte4 = conn->read();
	return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

/*
 * Send a string to a client.
 */
void writeString(const shared_ptr<Connection>& conn, const string& s) {
	for (char c : s) {
		conn->write(c);
	}
	conn->write('$');
}

int main(int argc, char* argv[]){
	if (argc != 2) {
		cerr << "Usage: myserver port-number" << endl;
		exit(1);
	}

	int port = -1;
	try {
		port = stoi(argv[1]);
	} catch (exception& e) {
		cerr << "Wrong port number. " << e.what() << endl;
		exit(1);
	}

	Server server(port);
	if (!server.isReady()) {
		cerr << "Server initialization error." << endl;
		exit(1);
	}

   dbinmemory db;

	while (true) {
		auto conn = server.waitForActivity();
		cout << "activity" << endl;
    MessageHandler mh(conn);
		if (conn != nullptr) {
			try {
        unsigned int cmd = mh.recvByte();
				cout << "got command: " << cmd << endl;
				switch (cmd) {
					case static_cast<int>(Protocol::COM_LIST_NG): {
						cout << "in list_ng" << endl;
						unsigned char com_end = mh.recvByte();
						cout << "com_end: " << com_end << endl;
						if (com_end != static_cast<int>(Protocol::COM_END)) {
							cout << "fatal error" << endl;
							throw ConnectionClosedException();
						}
						cout << "received com_end" << endl;
						vector<pair<int, Newsgroup>> newsgroups = db.listNewsGroups();
						mh.sendByte(static_cast<int>(Protocol::ANS_LIST_NG));
						mh.sendIntParameter(static_cast<int>(newsgroups.size()));
						cout << "amount of newsgroups: " << newsgroups.size() << endl;
						for (auto it = newsgroups.begin(); it != newsgroups.end(); ++it) {
							mh.sendIntParameter(it->first);
							mh.sendStringParameter(it->second.getGroupName());
							cout << "group id: " << it->first << endl;
							cout << "groupname: " << it->second.getGroupName() << endl;
						}
						mh.sendByte(static_cast<int>(Protocol::ANS_END));
						cout << "ans_end sent" << endl;
						break;
					}
					case static_cast<int>(Protocol::COM_CREATE_NG): {
							cout << "in create_ng" << endl;
							string groupname = mh.recvStringParameter();
							cout << groupname << endl;
							unsigned char com_end = mh.recvByte();
							if (com_end != static_cast<int>(Protocol::COM_END)) {
								throw ConnectionClosedException();
							}
							cout << "received com_end" << endl;
							unsigned int code = db.createNewsGroup(groupname);
							cout << code << endl;
							mh.sendByte(static_cast<int>(Protocol::ANS_CREATE_NG));
							if (code == static_cast<int>(Protocol::ANS_ACK)) {
								cout << "success" << endl;
								mh.sendByte(code);
							} else {
								cout << "failed to create newsgroup" << endl;
								mh.sendByte(static_cast<int>(Protocol::ANS_NAK));
								mh.sendByte(code);
							}

							mh.sendByte(static_cast<int>(Protocol::ANS_END));
							cout << "ans_end sent" << endl;
							break;
						}
					case static_cast<int>(Protocol::COM_DELETE_NG): {
							int id = mh.recvIntParameter();
							unsigned char com_end = mh.recvByte();
							if (com_end != static_cast<int>(Protocol::COM_END)) {
								throw ConnectionClosedException();
							}
							int code = db.deleteNewsGroup(id);
							if (code == static_cast<int>(Protocol::ANS_ACK)) {
								mh.sendByte(code);
							} else {
								mh.sendByte(static_cast<int>(Protocol::ANS_NAK));
								mh.sendByte(code);
							}
							mh.sendByte(static_cast<int>(Protocol::ANS_END));
							break;
						}
					default:
							throw ConnectionClosedException();
							break;
				}
				}
				//conn->write(result);
				// writeString(conn, result);
		 	catch (ConnectionClosedException&) {
				server.deregisterConnection(conn);
				cout << "Client closed connection" << endl;
			}
		} else {
			conn = make_shared<Connection>();
			server.registerConnection(conn);
			cout << "New client connects" << endl;
		}
	}
}
