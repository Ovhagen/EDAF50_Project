#ifndef DBONDISK_H
#define DBONDISK_H

#include <string>
#include "dbinterface.h"
#include "article.h"
#include <vector>

class Newsgroup;
class dbondisk : public dbinterface {

public:
  dbondisk();   //Implement as singleton
  virtual std::vector<std::pair<int, Newsgroup>> listNewsGroups() const;
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
  int newsGroupCounter;
};

#endif
