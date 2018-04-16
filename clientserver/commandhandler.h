#ifndef CMD_H
#define CMD_H

#include <iostream>
#include <string>
#include <vector>
#include "messagehandler.h"
#include "protocol.h"

class CommandHandler{

public:
  CommandHandler(MessageHandler messh) : mh(messh){}
  std::vector<std::pair<int, std::string>> listNewsGroup();
  std::vector<std::pair<int, std::string>> listNewsArticles(int id);
  void createNewsGroup(std::string groupName);
  void deleteNewsGroup(int id);


private:
  bool checkAnswerCode(int code, int answerCode);

  MessageHandler mh;
};


#endif
