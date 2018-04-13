#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "dbinmemory.h"

#include <memory>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <algorithm>
#include <cassert>
#include <vector>
#include "protocol.h"

using namespace std;

void test(dbinmemory& dbin) {
#if 1
  /*
  *  Testing creation and deletion of groups
  */
	assert(dbin.createNewsGroup("Sports") == static_cast<int>(Protocol::ANS_ACK));
  assert(dbin.createNewsGroup("Fishing") == static_cast<int>(Protocol::ANS_ACK));
  assert(dbin.createNewsGroup("Movies") == static_cast<int>(Protocol::ANS_ACK));
  std::vector<pair<int, Newsgroup>> result = dbin.listNewsGroups();
	assert(result[1].getGroupName() == "Sports"); //id starts at 1
  assert(result[2].getGroupName() == "Fishing");
  assert(result[3].getGroupName() == "Movies");
  assert(dbin.deleteNewsGroup(2) == static_cast<int>(Protocol::ANS_ACK));
  assert(dbin.deleteNewsGroup(2) == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST)); //double delete
  cout << "Group test passed" << endl;
  /*
  *  Testing creation , listing and deletion of articles
  */
  Newsgroup& sports = result[1];
  Newsgroup& movies = result[3];
  assert(dbin.createArticle(sports.getGroupId(), "How to become a faster sprinter",
                                                 "Pontus Ovhagen",
                                                 "Drink some red bull.") == static_cast<int>(Protocol::ANS_ACK));
  assert(dbin.createArticle(55, "Fail",
                                "Fail",
                                "Should not happen.") ==
                                static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
  assert(dbin.createArticle(movies.getGroupId(), "What was the greatest movie of 2017?",
                                                 "Pontus Ovhagen",
                                                 "Logan - The Wolverine. End of story.") == static_cast<int>(Protocol::ANS_ACK));

  auto sportArticles = dbin.listArticles(sports.getGroupId()); //This is std::pair<int, std::vector<std::string>>
  auto movieArticles = dbin.listArticles(movies.getGroupId());
  assert(movieArticles.first == static_cast<int>(Protocol::ANS_ACK));
  assert(movieArticles.second.find(1)->second.getArticleTitle() == "What was the greatest movie of 2017?");

  assert(dbin.getNewsArticle(999, 1).first == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
  assert(dbin.getNewsArticle(movies.getGroupId(), 999).first == static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST));
  auto article = dbin.getNewsArticle(movies.getGroupId(), 1);
  assert(article.first == static_cast<int>(Protocol::ANS_ACK));
  vector<string> info = article.second;
  assert(info[0] == "What was the greatest movie of 2017?" && info[1] == "Pontus Ovhagen" &&
  info[2] == "Logan - The Wolverine. End of story.");

  assert(dbin.deleteArticle(999, 1) == static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST));
  assert(dbin.deleteArticle(movies.getGroupId(), 999) == static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST));
  assert(dbin.deleteArticle(movies.getGroupId(), 1) == static_cast<int>(Protocol::ANS_ACK));
  movieArticles = dbin.listArticles(movies.getGroupId());
  assert(movieArticles.second.size() == 0); // No more articles should be in this group
  assert(dbin.deleteNewsGroup(sports.getGroupId()) == static_cast<int>(Protocol::ANS_ACK));
  assert(dbin.deleteNewsGroup(movies.getGroupId()) == static_cast<int>(Protocol::ANS_ACK));
  assert(dbin.listNewsGroups().size() == 0); //No groups should be left
  cout << "Article test passed" << endl;

#else
    throw std::runtime_error{"test failed"};
#endif
}

int main(){
  dbinmemory dbin;
  test(dbin);

}
