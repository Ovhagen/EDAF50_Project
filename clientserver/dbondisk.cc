#include "dbondisk.h"
#include "protocol.h"
#include "newsgroup.h"

#include <fstream>
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
  return static_cast<int>(Protocol::ERR_NG_ALREADY_EXISTS);
}

std::pair<int, std::map<int, Article>> dbondisk::listArticles(int newsGroupId) {
  DIR *dp;
  struct dirent *dirp;
  map<int, Article> articles;
  string path = "dbroot/";

  if((dp = opendir(path.c_str())) == NULL) {
    cout << "Error("   << errno << ") opening " << dp << endl;
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
      string newsgroup_title;
      iss >> newsgroup_title;
      cout << "using stoi on dir: " << dir_nbr_string << endl;
      dir_nbr = stoi(dir_nbr_string);
      cout << "stoi on dir worked" << endl;
      cout << dir_nbr << " == " << newsGroupId << endl;
      if (dir_nbr == newsGroupId) {
        // FIND MAX ARTICLE NBR FROM NEWSGROUP DIRECTORY
        DIR *adp;
        struct dirent *artDirp;
        string path = "dbroot/" + to_string(dir_nbr) + " " + newsgroup_title + "/";
        if((adp = opendir(path.c_str())) == NULL) {

        }
        while ((artDirp = readdir(adp)) != NULL) {
          if ( !strcmp(artDirp->d_name, ".") || !strcmp(artDirp->d_name, "..")) {
            // skip . and ..
          } else {
            string art_name = string(artDirp->d_name);
            istringstream iss(art_name);
            string art_nbr_string;
            int art_nbr;
            iss >> art_nbr_string;
            string art_title;
            iss >> art_title;
            cout << "using stoi on art: " << art_nbr_string << endl;
            art_nbr = stoi(art_nbr_string);
            cout << "stoi on art worked" << endl;
            Article art(art_title, 0, "", "");
            cout << "art_nbr: " << art_nbr << " and art_title: " << art_title << endl;
            articles[art_nbr] = art;
          }
        }
        return make_pair(static_cast<int>(Protocol::ANS_ACK), articles);
      }
      // --------------------------------------------------
    }
  }
  return make_pair(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST), articles);
}

int dbondisk::createArticle(int newsGroupId, std::string title, std::string author, std::string text) {

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
      int max_id;
      cout << dir_nbr << " == " << newsGroupId << endl;
      if (dir_nbr == newsGroupId) {
        // FIND MAX ARTICLE NBR FROM NEWSGROUP DIRECTORY
        max_id = 0;
        DIR *adp;
        struct dirent *artDirp;
        string art_path = "dbroot/" + to_string(dir_nbr) + " " + newsgroup_title + "/";
        if((adp = opendir(path.c_str())) == NULL) {

        }
        while ((artDirp = readdir(adp)) != NULL) {
          if ( !strcmp(artDirp->d_name, ".") || !strcmp(artDirp->d_name, "..")) {
            // skip . and ..
          } else {
            string art_name = string(artDirp->d_name);
            istringstream iss(art_name);
            string art_nbr_string;
            int art_nbr;
            iss >> art_nbr_string;
            art_nbr = stoi(art_nbr_string);
            if (art_nbr > max_id) {
              max_id = art_nbr;
            }
          }
        }
        max_id++;
        // --------------------------------------------------
        cout << "CREATE ARTICLE" << endl;
        cout << "Path: " << art_path << dir_nbr << " " << newsgroup_title << endl;
        ofstream outfile(art_path + to_string(max_id) + " " + title + ".txt");
        outfile << author << endl;
        outfile << text << endl;
        return static_cast<int>(Protocol::ANS_ACK);
      } else {
        return static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST);
      }
    }
  }
}

int dbondisk::deleteArticle(int newsGroupId, int articleId) {
  DIR *dp;
  struct dirent *dirp;

  if((dp = opendir("dbroot")) == NULL) {
    cout << "Error("   << errno << ") opening " << dp << endl;
    return errno;
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
      string newsgroup_title;
      iss >> newsgroup_title;
      dir_nbr = stoi(dir_nbr_string);
      if (dir_nbr == newsGroupId) {
        // FIND MAX ARTICLE NBR FROM NEWSGROUP DIRECTORY
        DIR *adp;
        struct dirent *artDirp;
        string path = "dbroot/" + to_string(dir_nbr) + " " + newsgroup_title + "/";
        if((adp = opendir(path.c_str())) == NULL) {

        }
        while ((artDirp = readdir(adp)) != NULL) {
          if ( !strcmp(artDirp->d_name, ".") || !strcmp(artDirp->d_name, "..")) {
            // skip . and ..
          } else {
            string art_name = string(artDirp->d_name);
            istringstream iss(art_name);
            string art_nbr_string;
            int art_nbr;
            iss >> art_nbr_string;
            string art_title;
            iss >> art_title;
            art_nbr = stoi(art_nbr_string);
            if (art_nbr == articleId) {
              string art_path = path + to_string(articleId) + " " + art_title;
              remove(art_path.c_str());
              return static_cast<int>(Protocol::ANS_ACK);
            }
          }
        }
        return static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST);
      } else {
        return static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST);
      }
      // --------------------------------------------------
    }
  }
}

std::pair<int, std::vector<std::string>> dbondisk::getNewsArticle(int newsGroupId, int articleId) {
  DIR *dp;
  struct dirent *dirp;
  vector<string> articleItems;

  if((dp = opendir("dbroot")) == NULL) {
    cout << "Error("   << errno << ") opening " << dp << endl;
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
      string newsgroup_title;
      iss >> newsgroup_title;
      dir_nbr = stoi(dir_nbr_string);
      if (dir_nbr == newsGroupId) {
        // FIND MAX ARTICLE NBR FROM NEWSGROUP DIRECTORY
        DIR *adp;
        struct dirent *artDirp;
        string path = "dbroot/" + to_string(dir_nbr) + " " + newsgroup_title + "/";
        if((adp = opendir(path.c_str())) == NULL) {

        }
        while ((artDirp = readdir(adp)) != NULL) {
          if ( !strcmp(artDirp->d_name, ".") || !strcmp(artDirp->d_name, "..")) {
            // skip . and ..
          } else {
            string art_name = string(artDirp->d_name);
            istringstream iss(art_name);
            string art_nbr_string;
            int art_nbr;
            iss >> art_nbr_string;
            string art_title;
            iss >> art_title;
            art_nbr = stoi(art_nbr_string);
            cout << artDirp->d_name << endl;
            cout << art_nbr << " == " << articleId << endl;
            if (art_nbr == articleId) {
              cout << "art_title: " << art_title << endl;
              articleItems.push_back(art_title);
              ifstream infile;
              string art_path = path + to_string(art_nbr) + " " + art_title;
              cout << "art_path: " << art_path << endl;
              infile.open(art_path);
              string art_author;
              infile >> art_author;
              cout << "art_author: " << art_author << endl;
              articleItems.push_back(art_author);
              string art_text;
              getline(infile, art_text, '\0');
              cout << "art_text: " << art_text << endl;
              articleItems.push_back(art_text);
              infile.close();
              return make_pair(static_cast<int>(Protocol::ANS_ACK), articleItems);
            }
          }
        }
        return make_pair(static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST), articleItems);
      } else {
        return make_pair(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST), articleItems);
      }
      // --------------------------------------------------
    }
  }
}
