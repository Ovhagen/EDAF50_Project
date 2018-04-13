#include "dbondisk.h"
#include "protocol.h"
#include "newsgroup.h"

#include <string.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>

using namespace std;

  dbondisk::dbondisk() {
    mkdir("dbroot", ACCESSPERMS);

    int max_id = 0;
    DIR *dp;
    struct dirent *dirp;
    string path = "dbroot/";
    if((dp = opendir(path.c_str())) == NULL) {

    }
    while ((dirp = readdir(dp)) != NULL) {
      if ( !strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")) {
        // skip . and ..
      } else {
        string dir_name = string(dirp->d_name);
        istringstream iss(dir_name);
        string dir_nbr_string;
        int dir_nbr;
        iss >> dir_nbr_string;
        dir_nbr = stoi(dir_nbr_string);
        if (dir_nbr > max_id) {
          max_id = dir_nbr;
        }
      }
    }
    newsGroupCounter = max_id;
  }

  vector<std::pair<int, Newsgroup>> dbondisk::listNewsGroups() const {
      vector<pair<int, Newsgroup>> newsgroups;
      DIR *dp;
      struct dirent *dirp;
      string path = "dbroot/";
      if((dp = opendir(path.c_str())) == NULL) {
     }
     while ((dirp = readdir(dp)) != NULL) {
       if ( !strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")) {
        // skip . and ..
      } else {
        string dir_name = string(dirp->d_name);
        istringstream iss(dir_name);
        string dir_nbr_string;
        int dir_nbr;
        iss >> dir_nbr_string;
        dir_nbr = stoi(dir_nbr_string);
        string newsgroup_title;
        iss >> newsgroup_title;
        Newsgroup ng(newsgroup_title, dir_nbr);
        newsgroups.push_back(make_pair(dir_nbr, ng));
      }
    }
      cout << "step4" << endl;
    return newsgroups;
  }

  int dbondisk::createNewsGroup(const std::string& title) {
    ++newsGroupCounter;
    string path = "dbroot/" + to_string(newsGroupCounter) + " " + title;
    return (mkdir(path.c_str(), ACCESSPERMS) == 0) ? static_cast<int>(Protocol::ANS_ACK) : static_cast<int>(Protocol::ERR_NG_ALREADY_EXISTS);
  }

  int dbondisk::deleteNewsGroup(int newsGroupId) {
    DIR *dp;
    struct dirent *dirp;

    if((dp = opendir("dbroot")) == NULL) {
     cout << "Error("   << errno << ") opening " << dp << endl;
     return errno;
   }
   while ((dirp = readdir(dp)) != NULL) {
    string dir_name = string(dirp->d_name);
    istringstream iss(dir_name);
    string dir_nbr_string;
    int dir_nbr;
    iss >> dir_nbr_string;
    dir_nbr = stoi(dir_nbr_string);
    if (dir_nbr == newsGroupId) {
      int removed = remove(dir_name.c_str());
      if (removed == 0) {
        return static_cast<int>(Protocol::ANS_ACK);
      }
    }
  }
  closedir(dp);
  return static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST);
 }

std::pair<int, std::map<int, Article>> dbondisk::listArticles(int newsGroupId) {}

int dbondisk::createArticle(int newsGroupId,
                              std::string title,
                              std::string author,
                              std::string text) {}

int dbondisk::deleteArticle(int newsGroupId, int articleId) {}

std::pair<int, std::vector<std::string>> dbondisk::getNewsArticle(int newsGroupId, int articleId) {}
