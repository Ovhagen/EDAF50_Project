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
  void createNewsGroup(std::string groupName);
  void deleteNewsGroup(int id);
  std::vector<std::pair<int, std::string>> listNewsArticles(int id);
  void createArticle(int id, std::string title, std::string author, std::string text);
  void deleteArticle(int id, int artId);
  std::vector<std::string> getArticle(int id, int artId);


private:
  bool checkAnswerCode(int code, int answerCode);

  MessageHandler mh;
};


#endif
