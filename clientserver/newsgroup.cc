#include <string>
#include <iostream>
#include "newsgroups.h"

/*Implementation of Newsgroups*/

/*Actions on articel map*/
void
Newsgroup::addArticel(std::string titel, std::string author, std::string text){
  Articel newArticel(titel, nbrOfArticels, author, text);
  articels[nbrOfArticels] = newArticel;
  ++nbrOfArticels;
}

bool
Newsgroup::deleteArticel(int idNbr){
  return articels.erase(idNbr) == 1 ? true : false;
}

/*Get operations*/
Articel&
Newsgroup::getArticel(int idNbr) const{
  return articles[idNbr];                 //Handle non-existing case?
}

int
Newsgroup::getGroupId() const{
  return id;
}

std::string
Newsgroup::getGroupName() const{
  return groupName;
}
