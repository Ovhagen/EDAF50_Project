#ifndef ARTICLE_H
#define ARTICLE_H

#include <string>
#include <iostream>

class Article{
public:
  Article(std::string tt = "", int id = 0, std::string auth = "", std::string tx = ""):  //default case?
  title(tt), articleId(id), author(auth), text(tx){}
  ~Article() = default;
  std::string getArticleTitle() const {return title;} //Added for testing.

private:
  friend class dbinmemory;

  std::string title;
  int articleId;
  std::string author;
  std::string text;
};


#endif
