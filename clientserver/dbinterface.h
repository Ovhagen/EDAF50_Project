#ifndef DBINTER_H
#define DBINTER_H

#include <utility>
#include <map>
#include <string>
#include <vector>
#include "newsgroup.h"
#include "article.h"

class Newsgroup;
class dbinterface {
public:
  virtual ~dbinterface() {}
  virtual std::vector<std::pair<int, Newsgroup>> listNewsGroups() const = 0;
  virtual int createNewsGroup(const std::string& title) = 0;
  virtual int deleteNewsGroup(int newsGroupId) = 0;

  /*Returns empty map if non-existing newsgroup
  * Check is done on the server
  */
  virtual std::pair<int, std::map<int, Article>> listArticles(int newsGroupId) = 0;
  virtual int createArticle(int newsGroupId,
                                std::string title,
                                std::string author,
                                std::string text) = 0;

  /*Returns actual error code for nack case.*/
  virtual int deleteArticle(int newsGroupId, int articleId) = 0;

  /*Returns actual error code for nack case.*/
  virtual std::pair<int, std::vector<std::string>> getNewsArticle(int newsGroupId, int articleId) = 0;

private:

};

#endif
