#include "dbondisk.h"
#include "protocol.h"
#include "newsgroup.h"
#include "article.h"

#include <string>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>

using namespace std;

dbondisk::dbondisk() {
  mkdir("dbroot", ACCESSPERMS);
}

  std::vector<std::pair<int, Newsgroup>> listNewsGroups() const {}

  int dbondisk::createNewsGroup(const std::string& title) {
    string path = "dbroot/" + static_cast<string>(++newsGroupCounter) + " " + title;
    mkdir(path, ACCESSPERMS);
  }

  int dbondisk::deleteNewsGroup(int newsGroupId) {
    DIR *dp;
    struct dirent *dirp;

    if((dp = opendir("dbroot")) == NULL) {
     cout << "Error(" << errno << ") opening " << dir << endl;
     return errno;
   }
   while ((dirp = readdir(dp)) != NULL) {
    string dir_name = string(dirp->d_name);
    istringstream iss(dir_name);
    int dir_nbr;
    iss >> static_cast<int>(dir_nbr);
    if (dir_nbr == newsGroupId) {
      int removed = remove(dir_name);
      if (removed == 0) {
        return static_cast<int>(Protocol::ANS_ACK);
      }
    }
  }
  closedir(dp);
  return static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST);
 }

 std::pair<int, std::map<int, Article>> listArticles(int newsGroupId) {}

 int createArticle(int newsGroupId,
                               std::string title,
                               std::string author,
                               std::string text) {}

int deleteArticle(int newsGroupId, int articleId) {}

std::pair<int, std::vector<std::string>> getNewsArticle(int newsGroupId, int articleId) {}
