#include "commandhandler.h"
#include "protocol.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <utility>

using namespace std;

vector<pair<int, string>>
CommandHandler::listNewsGroup(){
  mh.sendCode(static_cast<int>(Protocol::COM_LIST_NG));
  mh.sendCode(static_cast<int>(Protocol::COM_END));

  /*Start receiving of command*/
  int ansCode = mh.recvCode();
  checkAnswerCode(ansCode, static_cast<int>(Protocol::ANS_LIST_NG));

  int nbrOfGroups = mh.recvIntParameter();
  vector<pair<int, string>> groupList;
  for(int i = 0; i < nbrOfGroups; i++){
    int id = mh.recvIntParameter();
    string groupName = mh.recvStringParameter();
    groupList.push_back(make_pair(id, groupName));
  }

  int endCode = mh.recvCode();
  checkAnswerCode(endCode, static_cast<int>(Protocol::ANS_END));

  return groupList;
}

void
CommandHandler::createNewsGroup(std::string groupName){
  mh.sendCode(static_cast<int>(Protocol::COM_CREATE_NG));
  mh.sendStringParameter(groupName);
  mh.sendCode(static_cast<int>(Protocol::COM_END));

  /*Start receiving of command*/
  int ansCode = mh.recvCode();
  checkAnswerCode(ansCode, static_cast<int>(Protocol::ANS_CREATE_NG));

  int response = mh.recvCode();
  if(response == static_cast<int>(Protocol::ANS_ACK)){
    cout << "Successfully created the group \'" << groupName << "\'." << endl;
  } else if(response == static_cast<int>(Protocol::ANS_NAK)){
    int errorCode = mh.recvCode();
    if(errorCode == static_cast<int>(Protocol::ERR_NG_ALREADY_EXISTS)){
      cout << "Request failed. A group with the name \'" << groupName << "\' already exists." << endl;
    }
  }else{
    throw std::logic_error("CommandHandler::createNewsGroup");
  }

  int endCode = mh.recvCode();
  checkAnswerCode(endCode, static_cast<int>(Protocol::ANS_END));
}

void
CommandHandler::deleteNewsGroup(int id){
  mh.sendCode(static_cast<int>(Protocol::COM_DELETE_NG));
  mh.sendIntParameter(id);
  mh.sendCode(static_cast<int>(Protocol::COM_END));

  /*Start receiving of command*/
  int ansCode = mh.recvCode();
  checkAnswerCode(ansCode, static_cast<int>(Protocol::ANS_DELETE_NG));

  int response = mh.recvCode();
  if(response == static_cast<int>(Protocol::ANS_ACK)){
    cout << "Successfully created the group with id \'" << id << "\'."<< endl;
  } else if(response == static_cast<int>(Protocol::ANS_NAK)){
    int errorCode = mh.recvCode();
    if(errorCode == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)){
      cout << "Request failed. A group with id \'" << id << "\' does not exist." << endl;
    }
  }else{
    throw std::logic_error("CommandHandler::deleteNewsGroup");
  }

  int endCode = mh.recvCode();
  checkAnswerCode(endCode, static_cast<int>(Protocol::ANS_END));
}

vector<pair<int, string>>
CommandHandler::listNewsArticles(int id){
  mh.sendCode(static_cast<int>(Protocol::COM_LIST_ART));
  mh.sendIntParameter(id);
  mh.sendCode(static_cast<int>(Protocol::COM_END));

  /*Start receiving of command*/
  int ansCode = mh.recvCode();
  checkAnswerCode(ansCode, static_cast<int>(Protocol::ANS_LIST_ART));

  int response = mh.recvCode();
  vector<pair<int, string>> articleList;
  if(response == static_cast<int>(Protocol::ANS_ACK)){
    int nbrOfArticles = mh.recvIntParameter();
    for(int i = 0; i < nbrOfArticles; i++){
      int id = mh.recvIntParameter();
      string articleName = mh.recvStringParameter();
      articleList.push_back(make_pair(id, articleName));
    }
  }

  else if(response == static_cast<int>(Protocol::ANS_NAK)){
    int errorCode = mh.recvCode();
    if(errorCode == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)){
      cout << "Request failed. A group with id \'" << id << "\' does not exist." << endl;
    }
  }

  else{
    throw std::logic_error("CommandHandler::listNewsArticles");
  }

  int endCode = mh.recvCode();
  checkAnswerCode(endCode, static_cast<int>(Protocol::ANS_END));

  return articleList;
}

void
CommandHandler::createArticle(int id, string title, string author, string text){
  mh.sendCode(static_cast<int>(Protocol::COM_CREATE_ART));
  mh.sendIntParameter(id);
  mh.sendStringParameter(title);
  mh.sendStringParameter(author);
  mh.sendStringParameter(text);
  mh.sendCode(static_cast<int>(Protocol::COM_END));

  /*Start receiving of command*/
  int ansCode = mh.recvCode();
  checkAnswerCode(ansCode, static_cast<int>(Protocol::ANS_CREATE_ART));

  int response = mh.recvCode();
  if(response == static_cast<int>(Protocol::ANS_ACK)){
    cout << "Successfully created the article \'" << title << "\'." << endl;
  } else if(response == static_cast<int>(Protocol::ANS_NAK)){
    int errorCode = mh.recvCode();
    if(errorCode == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)){
      cout << "Request failed. A group with id \'" << id << "\' does not exist." << endl;
    }
  }else{
    throw std::logic_error("CommandHandler::createArticle");
  }

  int endCode = mh.recvCode();
  checkAnswerCode(endCode, static_cast<int>(Protocol::ANS_END));
}

void
CommandHandler::deleteArticle(int id, int artId){
  mh.sendCode(static_cast<int>(Protocol::COM_DELETE_ART));
  mh.sendIntParameter(id);
  mh.sendIntParameter(artId);
  mh.sendCode(static_cast<int>(Protocol::COM_END));

  /*Start receiving of command*/
  int ansCode = mh.recvCode();
  checkAnswerCode(ansCode, static_cast<int>(Protocol::ANS_DELETE_ART));

  int response = mh.recvCode();
  if(response == static_cast<int>(Protocol::ANS_ACK)){
    cout << "Successfully deleted the article with id \'" << artId << "\' for the group with id \'" << id << "\'." << endl;
  } else if(response == static_cast<int>(Protocol::ANS_NAK)){
    int errorCode = mh.recvCode();
    if(errorCode == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)){
      cout << "Request failed. A group with id \'" << id << "\' does not exist." << endl;
    }else if(errorCode == static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST)){
      cout << "Request failed. An article with id \'" << artId << "\' does not exist in the group with id \'" << id << "\'." << endl;
    }
  }else{
    throw std::logic_error("CommandHandler::deleteArticle");
  }

  int endCode = mh.recvCode();
  checkAnswerCode(endCode, static_cast<int>(Protocol::ANS_END));
}

vector<string>
CommandHandler::getArticle(int id, int artId){
  mh.sendCode(static_cast<int>(Protocol::COM_GET_ART));
  mh.sendIntParameter(id);
  mh.sendIntParameter(artId);
  mh.sendCode(static_cast<int>(Protocol::COM_END));

  /*Start receiving of command*/
  int ansCode = mh.recvCode();
  checkAnswerCode(ansCode, static_cast<int>(Protocol::ANS_GET_ART));

  int response = mh.recvCode();
  string title, author, text;
  if(response == static_cast<int>(Protocol::ANS_ACK)){
    title = mh.recvStringParameter();
    author = mh.recvStringParameter();
    text =  mh.recvStringParameter();
  } else if(response == static_cast<int>(Protocol::ANS_NAK)){
    int errorCode = mh.recvCode();
    if(errorCode == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)){
      cout << "Request failed. A group with id \'" << id << "\' does not exist." << endl;
      mh.recvCode(); //flush out last byte code
      return vector<string>();
    }else if(errorCode == static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST)){
      cout << "Request failed. An article with id \'" << artId << "\' does not exist in the group with id \'" << id << "\'." << endl;
      mh.recvCode(); //flush out last byte code
      return vector<string>();
    }
  }else{
    cout << "error" << endl;
    throw std::logic_error("CommandHandler::getArticle");
  }

  int endCode = mh.recvCode();
  checkAnswerCode(endCode, static_cast<int>(Protocol::ANS_END));

  return vector<string>({title, author, text});
}

bool
CommandHandler::checkAnswerCode(int code, int answerCode){
	if(code == answerCode){
		return true;
	}
	else{
		throw std::logic_error("CommandHandler::checkAnswerCode");
	}
}
