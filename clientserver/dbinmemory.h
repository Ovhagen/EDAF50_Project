#ifndef DBINMEM_H
#define DBINMEM_H

#include <string>
#include <map>
#include "dbinterface.h"
#include "article.h"

class Newsgroup;
class dbinmemory : public dbinterface {

public:
  dbinmemory() : newsGroupCounter(0) {};   //Implement as singleton
  virtual std::map<int, Newsgroup> listNewsGroups() const;
  virtual int createNewsGroup(const std::string& title);
  virtual int deleteNewsGroup(int newsGroupId);
  virtual std::pair<int, std::map<int, Article>> listArticles(int newsGroupId);
  virtual int createArticle(int newsGroupId,
                                std::string title,
                                std::string author,
                                std::string text);
  virtual int deleteArticle(int newsGroupId, int articleId);
  virtual std::pair<int, std::vector<std::string>> getNewsArticle(int newsGroupId, int articleId);

private:
  std::map<int, Newsgroup> db;
  int newsGroupCounter;
};

#endif
