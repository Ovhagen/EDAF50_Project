/* myserver.cc: sample server program */
#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
#include "../messagehandler.h"
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
    MessageHandler mh(conn);
		if (conn != nullptr) {
			try {
        unsigned char result = mh.recvByte();
        if(result == static_cast<int>(Protocol::COM_CREATE_ART)) {
          string groupname = mh.recvStringParameter();
          unsigned char com_end = mh.recvByte();
          if (com_end != static_cast<int>(Protocol::COM_END)) {
            cerr << "expected com_end, something wrong" << endl;
          }
          int code = db.createNewsGroup(groupname);
          mh.sendByte(static_cast<int>(Protocol::ANS_CREATE_NG));
          if (code == static_cast<int>(Protocol::ANS_ACK)) {
            mh.sendByte(code);
          } else {
            mh.sendByte(static_cast<int>(Protocol::ANS_NAK));
            mh.sendByte(code);
          }
          mh.sendByte(static_cast<int>(Protocol::ANS_END));
        }
				//conn->write(result);
				// writeString(conn, result);
			} catch (ConnectionClosedException&) {
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
