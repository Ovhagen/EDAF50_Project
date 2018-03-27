#ifndef NEWSGROUP_H
#define NEWSGROUP_H

#include <string>
#include <map>
#include "articel.h"

class Newsgroup{

public:
  Newsgroup(std::string name, int identification, int nbrArticles):
  groupName(name), id(identification), nbrOfArticels(nbrArticles){}
  ~Newsgroup() = default;

  void addArticel(  std::string titel, std::string author, std::string text);
  bool deleteArticel(int idNbr);

  Articel& getArticel(int idNbr) const;
  int getGroupId() const;
  std::string getGroupName() const;

private:
  std::string groupName;
  int id;
  int nbrOfArticels;
  std::map<int, Articel> articles;
};


#endif
