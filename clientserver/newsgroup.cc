#include <string>
#include <iostream>
#include "newsgroup.h"
#include "article.h"

/*Implementation of Newsgroups*/

/*Actions on article map*/
void
Newsgroup::addArticle(std::string titel, std::string author, std::string text){
  Article newArticle(titel, nbrOfArticles, author, text);
  articles[nbrOfArticles] = newArticle;
  ++nbrOfArticles;
}

bool
Newsgroup::deleteArticle(int idNbr){
  return articles.erase(idNbr) == 1 ? true : false;
}

/*Get operations*/
map<int, Article>::const_iterator&
Newsgroup::getArticle(int idNbr) const{
  return articles.find(idNbr);        //Handle non-existing case?
}

int
Newsgroup::getGroupId() const{
  return id;
}

std::string
Newsgroup::getGroupName() const{
  return groupName;
}
