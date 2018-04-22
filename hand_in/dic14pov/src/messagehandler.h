#ifndef MESSAGE_HAND_H
#define MESSAGE_HAND_H

#include "connection.h"
#include <memory>

class MessageHandler{

public:
  MessageHandler(std::shared_ptr<Connection> connection) : conn(connection){}
  int recvByte();
  int recvCode();
  int recvInt();
  int recvIntParameter();
  std::string recvStringParameter();
  void sendByte(int code);
  void sendCode(int code);
  void sendInt(int value);
  void sendIntParameter(int param);
  void sendStringParameter(std::string param);
private:
  std::shared_ptr<Connection> conn;

};


#endif
