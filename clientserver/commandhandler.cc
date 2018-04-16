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

vector<pair<int, string>>
CommandHandler::listNewsArticles(int id){

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
    cout << "Successfully created the group \'" << groupName << "\'" << endl;
  } else if(response == static_cast<int>(Protocol::ANS_NAK)){
    int errorCode = mh.recvCode();
    if(errorCode == static_cast<int>(Protocol::ERR_NG_ALREADY_EXISTS)){
      cout << "Request failed. A group with the name \'" << groupName << "\' already exists." << endl;
    }
  }else{
    throw std::bad_exception();
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
    cout << "Successfully created the group with id \'" << id << "\'"<< endl;
  } else if(response == static_cast<int>(Protocol::ANS_NAK)){
    int errorCode = mh.recvCode();
    if(errorCode == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)){
      cout << "Request failed. A group with id \'" << id << "\' does not exist." << endl;
    }
  }else{
    throw std::bad_exception();
  }

  int endCode = mh.recvCode();
  checkAnswerCode(endCode, static_cast<int>(Protocol::ANS_END));
}

bool
CommandHandler::checkAnswerCode(int code, int answerCode){
	if(code == answerCode){
		return true;
	}
	else{
		throw std::bad_exception();
	}
}
