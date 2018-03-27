#ifndef DBINTER_H
#define DBINTER_H

#include <utility>
#include <map>
#include <string>
#include <vector>
#include "newsgroup.h"

class dbinterface{
public:
  virtual ~dbinterface() {}
  virtual std::pair<int, std::map<int, Newsgroup>> listNewsGroup() = 0;
  virtual int createNewsGroup() = 0;
  virtual int deleteNewsGroup() = 0;

  /*Returns empty map if non-existing newsgroup
  * Check is done on the server
  */
  virtual std::pair<int, std::map<int, Articels>> listArticels(int newsGroupId) = 0;
  virtual int createArticel(int newsGroupId,
                                std::string title,
                                std::string author,
                                std::string text) = 0;

  /*Returns actual error code for nack case.*/
  virtual int deleteArticel(int newsGroupId, int articelId) = 0;

  /*Returns actual error code for nack case.*/
  virtual std::pair<int, std::vector<std::string>> getNewsArticel() = 0;

private:

};

#endif
