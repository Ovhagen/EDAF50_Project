#ifndef NEWSGROUP_H
#define NEWSGROUP_H

#include <string>
#include <map>
#include "article.h"
#include "dbinmemory.h"

class Newsgroup {

public:
  Newsgroup(std::string name = "", int identification = 0):
  groupName(name), id(identification), nbrOfArticles(0){}
  ~Newsgroup() = default;

  void addArticle( std::string titel, std::string author, std::string text);
  bool deleteArticle(int idNbr);

  std::map<int, Article>::const_iterator getArticle(int idNbr) const;

  int getGroupId() const;
  std::string getGroupName() const;

private:
  friend class dbinmemory;

  std::string groupName;
  int id;
  int nbrOfArticles;
  std::map<int, Article> articles;
};


#endif
