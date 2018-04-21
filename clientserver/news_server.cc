/* myserver.cc: sample server program */
#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
#include "messagehandler.h"
#include "dbinterface.h"
#include "dbinmemory.h"
#include "dbondisk.h"
#include "newsgroup.h"

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
	if (argc != 3) {
		cerr << "Usage: myserver port-number db-type" << endl;
		exit(1);
	}

	int port = -1;
	try {
		port = stoi(argv[1]);
	} catch (exception& e) {
		cerr << "Wrong port number. " << e.what() << endl;
		exit(1);
	}

	int dbtype = -1;
	try {
		dbtype = stoi(argv[2]);
	} catch (exception& e) {
		cerr << "Wrong db type (1 for in memory, 2 for on disk). " << e.what() << endl;
		exit(1);
	}

	Server server(port);
	if (!server.isReady()) {
		cerr << "Server initialization error." << endl;
		exit(1);
	}

	cout << "Server started."<< endl;
		dbinterface* db;
	if (dbtype == 1) {
		dbinmemory inmem;
		db = &inmem;
		cout << "In memory version active."<< endl;
	} else if (dbtype == 2) {
		dbondisk ondisk;
		db = &ondisk;
		cout << "On disk version active."<< endl;
	} else {
		cerr << "Wrong db type (1 for in memory, 2 for on disk). " << endl;
		exit(1);
	}

	while (true) {
		auto conn = server.waitForActivity();
    MessageHandler mh(conn);
		if (conn != nullptr) {
			try {
        unsigned int cmd = mh.recvByte();
				switch (cmd) {
					case static_cast<int>(Protocol::COM_LIST_NG): {
						unsigned char com_end = mh.recvByte();
						if (com_end != static_cast<int>(Protocol::COM_END)) {
							cout << "fatal error" << endl;
							throw ConnectionClosedException();
						}
						vector<pair<int, Newsgroup>> newsgroups = db->listNewsGroups();
						mh.sendByte(static_cast<int>(Protocol::ANS_LIST_NG));
						mh.sendIntParameter(static_cast<int>(newsgroups.size()));
						for (auto it = newsgroups.begin(); it != newsgroups.end(); ++it) {
							mh.sendIntParameter(it->first);
							mh.sendStringParameter(it->second.getGroupName());
						}
						mh.sendByte(static_cast<int>(Protocol::ANS_END));
						break;
					}
					case static_cast<int>(Protocol::COM_CREATE_NG): {
							string groupname = mh.recvStringParameter();
							cout << groupname << endl;
							unsigned char com_end = mh.recvByte();
							if (com_end != static_cast<int>(Protocol::COM_END)) {
								throw ConnectionClosedException();
							}
							unsigned int code = db->createNewsGroup(groupname);
							cout << code << endl;
							mh.sendByte(static_cast<int>(Protocol::ANS_CREATE_NG));
							if (code == static_cast<int>(Protocol::ANS_ACK)) {
								mh.sendByte(code);
							} else {
								mh.sendByte(static_cast<int>(Protocol::ANS_NAK));
								mh.sendByte(code);
							}

							mh.sendByte(static_cast<int>(Protocol::ANS_END));
							break;
						}
					case static_cast<int>(Protocol::COM_DELETE_NG): {
							int id = mh.recvIntParameter();
							unsigned char com_end = mh.recvByte();
							if (com_end != static_cast<int>(Protocol::COM_END)) {
								throw ConnectionClosedException();
							}
							int code = db->deleteNewsGroup(id);
							mh.sendByte(static_cast<int>(Protocol::ANS_DELETE_NG));
							if (code == static_cast<int>(Protocol::ANS_ACK)) {
								mh.sendByte(code);
							} else {
								mh.sendByte(static_cast<int>(Protocol::ANS_NAK));
								mh.sendByte(code);
							}
							mh.sendByte(static_cast<int>(Protocol::ANS_END));
							break;
						}
				case static_cast<int>(Protocol::COM_LIST_ART): {
						int group_id = mh.recvIntParameter();
						unsigned char com_end = mh.recvByte();
						if (com_end != static_cast<int>(Protocol::COM_END)) {
							throw ConnectionClosedException();
						}
						pair<int, map<int, Article>> articles = db->listArticles(group_id);
						mh.sendByte(static_cast<int>(Protocol::ANS_LIST_ART));
						if (articles.first == static_cast<int>(Protocol::ANS_ACK)) {
							mh.sendByte(static_cast<int>(Protocol::ANS_ACK));
							cout << articles.second.size() << endl;
							mh.sendIntParameter(articles.second.size());
							for (auto it = articles.second.begin(); it != articles.second.end(); ++it) {
								mh.sendIntParameter(it->first);
								mh.sendStringParameter(it->second.getArticleTitle());
							}
						} else {
							mh.sendByte(static_cast<int>(Protocol::ANS_NAK));
							mh.sendByte(articles.first);
						}
						mh.sendByte(static_cast<int>(Protocol::ANS_END));
						break;
					}
				case static_cast<int>(Protocol::COM_CREATE_ART): {
							int group_id = mh.recvIntParameter();
							string article_title = mh.recvStringParameter();
							string article_author = mh.recvStringParameter();
							string article_text = mh.recvStringParameter();
							unsigned char com_end = mh.recvByte();
							if (com_end != static_cast<int>(Protocol::COM_END)) {
								throw ConnectionClosedException();
							}
							int code = db->createArticle(group_id, article_title, article_author, article_text);
							mh.sendByte(static_cast<int>(Protocol::ANS_CREATE_ART));
							if (code == static_cast<int>(Protocol::ANS_ACK)) {
								mh.sendByte(code);
							} else {
								mh.sendByte(static_cast<int>(Protocol::ANS_NAK));
								mh.sendByte(code);
							}
							mh.sendByte(static_cast<int>(Protocol::ANS_END));
							break;
					}
				case static_cast<int>(Protocol::COM_DELETE_ART): {
						int group_id = mh.recvIntParameter();
						int article_id = mh.recvIntParameter();
						unsigned char com_end = mh.recvByte();
						if (com_end != static_cast<int>(Protocol::COM_END)) {
							throw ConnectionClosedException();
						}
						int code = db->deleteArticle(group_id, article_id);
						mh.sendByte(static_cast<int>(Protocol::ANS_DELETE_ART));
						if (code == static_cast<int>(Protocol::ANS_ACK)) {
							mh.sendByte(code);
						} else {
							mh.sendByte(static_cast<int>(Protocol::ANS_NAK));
							mh.sendByte(code);
						}
						mh.sendByte(static_cast<int>(Protocol::ANS_END));
						break;
					}
				case static_cast<int>(Protocol::COM_GET_ART): {
						int group_id = mh.recvIntParameter();
						int article_id = mh.recvIntParameter();
						unsigned char com_end = mh.recvByte();
						if (com_end != static_cast<int>(Protocol::COM_END)) {
							throw ConnectionClosedException();
						}
						pair<int, vector<string>> article = db->getNewsArticle(group_id, article_id);
						mh.sendByte(static_cast<int>(Protocol::ANS_GET_ART));
						if (article.first == static_cast<int>(Protocol::ANS_ACK)) {
							mh.sendByte(static_cast<int>(Protocol::ANS_ACK));
							for (auto it = article.second.begin(); it != article.second.end(); ++it) {
								mh.sendStringParameter(*it);
							}
						} else {
							mh.sendByte(static_cast<int>(Protocol::ANS_NAK));
							mh.sendByte(article.first);
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
