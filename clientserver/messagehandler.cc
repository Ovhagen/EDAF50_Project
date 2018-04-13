#include <stdexcept>
#include <string>
#include <algorithm>
#include <iostream>
#include "connection.h"
#include "connectionclosedexception.h"
#include "messagehandler.h"
#include "protocol.h"

/*Implementation of MessageHandler*/
using std::cerr;
using std::endl;
using std::string;

void MessageHandler::sendByte(int code) {
  try {
    conn->write(static_cast<char>(code));
  } catch (ConnectionClosedException e) {
    cerr << "sendByte write error" << endl;
  }
}

void MessageHandler::sendCode(int code) {
  sendByte(code);
}

void MessageHandler::sendInt(int value) {
  // std::cout << ((value >> 24) & 0xFF) << std::endl;
  // std::cout << ((value >> 16) & 0xFF) << std::endl;
  // std::cout << ((value >> 8) & 0xFF) << std::endl;
  // std::cout << (value & 0xFF) << std::endl;
  sendByte((value >> 24) & 0xFF);
  sendByte((value >> 16) & 0xFF);
  sendByte((value >> 8) & 0xFF);
  sendByte(value & 0xFF);
}

void MessageHandler::sendIntParameter(int param) {
  sendCode(static_cast<int>(Protocol::PAR_NUM));
  sendInt(param);
}

void MessageHandler::sendStringParameter(string param){
  sendCode(static_cast<int>(Protocol::PAR_STRING));
  sendInt(param.length());
  for (auto it = param.begin(); it != param.end(); ++it) {
    sendByte(*it);
  }
}

int MessageHandler::recvByte() {
  int code = conn->read();
  if (code == -1) {
    cerr << "recvByte read error" << endl;
  }
  return code;
}

int MessageHandler::recvCode() {
  int code = recvByte();
  return code;
}

int MessageHandler::recvInt() {
  int b1 = recvByte();
  int b2 = recvByte();
  int b3 = recvByte();
  int b4 = recvByte();

  return b1 << 24 | b2 << 16 | b3 << 8 | b4;
}

int MessageHandler::recvIntParameter() {
  int code = recvCode();
  if (code != static_cast<int>(Protocol::PAR_NUM)) {
    cerr << "ProtocolViolationException: Receive numeric parameter " << static_cast<int>(Protocol::PAR_NUM) << " " << code << endl;
  }
  return recvInt();
}

string MessageHandler::recvStringParameter() {
  int code = recvCode();
  if (code != static_cast<int>(Protocol::PAR_STRING)) {
    cerr << "ProtocolViolationException: Receive string parameter " << static_cast<int>(Protocol::PAR_NUM) << " " << code << endl;
  }
  int n = recvInt();
  if (n < 0) {
    cerr << "ProtocolViolationException: Receive numeric parameter, number of characters < 0" << endl;
  }
  string result = "";
  for (int i = 1; i <= n; i++) {
    char ch = static_cast<char>(conn->read());
    result.append(1, ch);
  }
  if(result.length()!=n) {
    cerr << "Resulting string wrong" << endl;
  }
  return result;
}
