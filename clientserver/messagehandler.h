#ifndef MESSAGE_HAND_H
#define MESSAGE_HAND_H

#include "connection.h"

class MessageHandler{

public:
  MessageHandler(Connection connection) : conn(connection){}
  void sendByte(int code);
  int recvCode();
  int recvInt();
private:
  Connection conn;

};


#endif
