#include "dbinmemory.h"
#include "protocol.h"
#include "newsgroup.h"

#include <iostream>
#include <algorithm>

#if 0
#define trace cout
#else
#define trace if(false) cout
#endif

using namespace std;

std::vector<pair<int, Newsgroup>>
dbinmemory::listNewsGroups() const {
  trace << "dbinmemory::listNewsGroups" << endl;
  std::vector<pair<int, Newsgroup>> newsgroups;
  for (auto it = db.begin(); it != db.end(); ++it) {
    newsgroups.push_back(make_pair(it->first, it->second));
  }
  return newsgroups;
}


int
dbinmemory::createNewsGroup(const std::string& title) {
  trace << "dbinmemory::createNewsGroup" << endl;
  auto it = find_if(db.begin(), db.end(), [&](const pair<int, Newsgroup>& temp){
    return temp.second.getGroupName() == title;
  });

  if (it != db.end()) {
    return static_cast<int>(Protocol::ERR_NG_ALREADY_EXISTS);
  }

  Newsgroup ng(title,++newsGroupCounter);
  db.insert(make_pair(newsGroupCounter, ng));
  return static_cast<int>(Protocol::ANS_ACK);
}


int
dbinmemory::deleteNewsGroup(int newsGroupId) {
  trace << "dbinmemory::deleteNewsGroup" << endl;
  int removed = db.erase(newsGroupId);
  if (removed == 1) {
    return static_cast<int>(Protocol::ANS_ACK);
  } else {
    return static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST);
  }
}


std::pair<int, std::map<int, Article>>
dbinmemory::listArticles(int newsGroupId) {
  trace << "dbinmemory::listArticles" << endl;
   auto it = db.find(newsGroupId);

   if (it == db.end()) {
     map<int, Article> emptyMap;
     return make_pair(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST), emptyMap);
   }

   return make_pair(static_cast<int>(Protocol::ANS_ACK), it->second.articles);
 }


int
dbinmemory::createArticle(int newsGroupId, std::string title, std::string author, std::string text) {
  trace << "dbinmemory::createArticle" << endl;
  auto it = db.find(newsGroupId);

  if (it == db.end()) {
    return static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST);
  }

  it->second.addArticle(title, author, text);
  return static_cast<int>(Protocol::ANS_ACK);
}


int
dbinmemory::deleteArticle(int newsGroupId, int articleId) {
  trace << "dbinmemory::deleteArticle" << endl;
  auto it = db.find(newsGroupId);

  if (it == db.end()) {
    return static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST);
  }

  bool removed = it->second.deleteArticle(articleId);

  if (removed) {
    return static_cast<int>(Protocol::ANS_ACK);
  } else {
    return static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST);
  }
}


std::pair<int, std::vector<std::string>>
dbinmemory::getNewsArticle(int newsGroupId, int articleId) {
  trace << "dbinmemory::getNewsArticle" << endl;
  auto it = db.find(newsGroupId);
  std::vector<string> articleItems;
  if (it == db.end()) {
    return make_pair(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST), articleItems);
  }
  auto articleIt = it->second.getArticle(articleId);

  if (articleIt == it->second.articles.end()) {
    return make_pair(static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST), articleItems);
  } else {
    articleItems.push_back(articleIt->second.title);
    articleItems.push_back(articleIt->second.author);
    articleItems.push_back(articleIt->second.text);
    return make_pair(static_cast<int>(Protocol::ANS_ACK), articleItems);
  }
}
