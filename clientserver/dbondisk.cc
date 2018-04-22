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

  int max_id = -1;
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
      int dir_nbr;
      iss >> dir_nbr;
      string newsgroup_title;
      iss >> newsgroup_title;
      string remaining;
      getline(iss, remaining);
      string full_name = newsgroup_title + remaining;
      Newsgroup ng(full_name, dir_nbr);
      newsgroups.push_back(make_pair(dir_nbr, ng));
    }
  }

  sort(newsgroups.begin(), newsgroups.end(), [](pair<int, Newsgroup> pair1, pair<int, Newsgroup> pair2) {
    return pair1.first < pair2.first;
  });
  return newsgroups;
}

int dbondisk::createNewsGroup(const std::string& title) {
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
      int dir_nbr;
      iss >> dir_nbr;

      string newsgroup_title;
      iss >> newsgroup_title;
      string remaining;
      getline(iss, remaining);
      string full_name = newsgroup_title + remaining;

      if (title == full_name) {
        return static_cast<int>(Protocol::ERR_NG_ALREADY_EXISTS);
      }
    }
  }
  ++newsGroupCounter;
  string ng_path = "dbroot/" + to_string(newsGroupCounter) + " " + title;
  mkdir(ng_path.c_str(), ACCESSPERMS);
  return static_cast<int>(Protocol::ANS_ACK);
}

int dbondisk::deleteNewsGroup(int newsGroupId) {
  DIR *dp;
  struct dirent *dirp;

  if((dp = opendir("dbroot")) == NULL) {
    cout << "Error("   << errno << ") opening " << dp << endl;
  }
  while ((dirp = readdir(dp)) != NULL) {
    if ( !strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")) {
      // skip . and ..
    } else {
    string dir_name = string(dirp->d_name);
    istringstream iss(dir_name);
    //string dir_nbr_string;
    int dir_nbr;
    iss >> dir_nbr;
    //dir_nbr = stoi(dir_nbr_string);
    if (dir_nbr == newsGroupId) {
      string path = "dbroot/" + dir_name;
      // delete all articles first
      DIR *adp;
      struct dirent *artDirp;
      //string path = "dbroot/" + to_string(dir_nbr) + " " + newsgroup_title + "/";
      if((adp = opendir(path.c_str())) == NULL) {

      }
      while ((artDirp = readdir(adp)) != NULL) {
        if ( !strcmp(artDirp->d_name, ".") || !strcmp(artDirp->d_name, "..")) {
          // skip . and ..
        } else {
          string art_name = string(artDirp->d_name);
          istringstream iss(art_name);
          int art_nbr;
          iss >> art_nbr;
          string art_title;
          iss >> art_title;
          string art_path = path + "/" + to_string(art_nbr) + " " + art_title;
          remove(art_path.c_str());
        }
      }
        closedir(adp);
      // ---------------

      remove(path.c_str());
      return static_cast<int>(Protocol::ANS_ACK);
    }
    }
  }
  closedir(dp);
  return static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST);
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
      int dir_nbr;
      iss >> dir_nbr;

      string newsgroup_title;
      iss >> newsgroup_title;
      string remaining;
      getline(iss, remaining);
      string full_name = newsgroup_title + remaining;

      if (dir_nbr == newsGroupId) {
        // FIND MAX ARTICLE NBR FROM NEWSGROUP DIRECTORY
        DIR *adp;
        struct dirent *artDirp;
        string path = "dbroot/" + to_string(dir_nbr) + " " + full_name + "/";
        if((adp = opendir(path.c_str())) == NULL) {

        }
        while ((artDirp = readdir(adp)) != NULL) {
          if ( !strcmp(artDirp->d_name, ".") || !strcmp(artDirp->d_name, "..")) {
            // skip . and ..
          } else {
            string art_name = string(artDirp->d_name);
            istringstream iss(art_name);
            int art_nbr;
            iss >> art_nbr;

            string art_title;
            iss >> art_title;
            string remaining;
            getline(iss, remaining);
            string full_art_name = art_title + remaining;

            Article art(full_art_name, 0, "", "");
            articles[art_nbr] = art;
          }
        }
        closedir(adp);
      /*  sort(articles.begin(), articles.end(), [](pair<int, Newsgroup> pair1, pair<int, Newsgroup> pair2) {
          return pair1.first < pair2.first;
        }); */
        return make_pair(static_cast<int>(Protocol::ANS_ACK), articles);
      }
      // --------------------------------------------------
    }
  }
  closedir(dp);
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
      int dir_nbr;
      iss >> dir_nbr;

      string newsgroup_title;
      iss >> newsgroup_title;
      string remaining;
      getline(iss, remaining);
      string full_name = newsgroup_title + remaining;

      int max_id;
      if (dir_nbr == newsGroupId) {
        // FIND MAX ARTICLE NBR FROM NEWSGROUP DIRECTORY
        max_id = 0;
        DIR *adp;
        struct dirent *artDirp;
        string art_path = "dbroot/" + to_string(dir_nbr) + " " + full_name + "/";
        if((adp = opendir(art_path.c_str())) == NULL) {

        }
        while ((artDirp = readdir(adp)) != NULL) {
          if ( !strcmp(artDirp->d_name, ".") || !strcmp(artDirp->d_name, "..")) {
            // skip . and ..
          } else {
            string art_name = string(artDirp->d_name);
            istringstream iss(art_name);
            int art_nbr;
            iss >> art_nbr;
            if (art_nbr > max_id) {
              max_id = art_nbr;
            }
          }
        }
        max_id++;
        // --------------------------------------------------
        ofstream outfile(art_path + to_string(max_id) + " " + title);
        outfile << author << endl;
        outfile << text << endl;
        return static_cast<int>(Protocol::ANS_ACK);
      }
    }
  }
  return static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST);
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
      int dir_nbr;
      iss >> dir_nbr;

      string newsgroup_title;
      iss >> newsgroup_title;
      string remaining;
      getline(iss, remaining);
      string full_name = newsgroup_title + remaining;

      if (dir_nbr == newsGroupId) {
        DIR *adp;
        struct dirent *artDirp;
        string path = "dbroot/" + to_string(dir_nbr) + " " + full_name + "/";
        if((adp = opendir(path.c_str())) == NULL) {

        }
        while ((artDirp = readdir(adp)) != NULL) {
          if ( !strcmp(artDirp->d_name, ".") || !strcmp(artDirp->d_name, "..")) {
            // skip . and ..
          } else {
            string art_name = string(artDirp->d_name);
            istringstream iss(art_name);
            int art_nbr;
            iss >> art_nbr;

            string art_title;
            iss >> art_title;
            string remaining;
            getline(iss, remaining);
            string full_art_name = art_title + remaining;

            if (art_nbr == articleId) {
              string art_path = path + to_string(articleId) + " " + full_art_name;
              remove(art_path.c_str());
              return static_cast<int>(Protocol::ANS_ACK);
            }
          }
        }
        return static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST);
      }
    }
  }
  return static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST);
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
      int dir_nbr;
      iss >> dir_nbr;

      string newsgroup_title;
      iss >> newsgroup_title;
      string remaining;
      getline(iss, remaining);
      string full_name = newsgroup_title + remaining;

      if (dir_nbr == newsGroupId) {
        // FIND MAX ARTICLE NBR FROM NEWSGROUP DIRECTORY
        DIR *adp;
        struct dirent *artDirp;
        string path = "dbroot/" + to_string(dir_nbr) + " " + full_name + "/";
        if((adp = opendir(path.c_str())) == NULL) {

        }
        while ((artDirp = readdir(adp)) != NULL) {
          if ( !strcmp(artDirp->d_name, ".") || !strcmp(artDirp->d_name, "..")) {
            // skip . and ..
          } else {
            string art_name = string(artDirp->d_name);
            istringstream iss(art_name);
            int art_nbr;
            iss >> art_nbr;

            string art_title;
            iss >> art_title;

            string remaining;
            getline(iss, remaining);
            string full_art_name = art_title + remaining;

            if (art_nbr == articleId) {
              articleItems.push_back(full_art_name);
              ifstream infile;
              string art_path = path + to_string(art_nbr) + " " + full_art_name;
              infile.open(art_path);
              string art_author;
              getline(infile, art_author);
              articleItems.push_back(art_author);
              string art_text;
              string temp;
              while (getline(infile, temp)) {
                art_text += temp;
                art_text += '\n';
              }
              string new_art_text = art_text.substr(0, art_text.size() - 1);
              articleItems.push_back(new_art_text);
              infile.close();
              return make_pair(static_cast<int>(Protocol::ANS_ACK), articleItems);
            }
          }
        }
        return make_pair(static_cast<int>(Protocol::ERR_ART_DOES_NOT_EXIST), articleItems);
      }
      // --------------------------------------------------
    }
  }
  return make_pair(static_cast<int>(Protocol::ERR_NG_DOES_NOT_EXIST), articleItems);
}
