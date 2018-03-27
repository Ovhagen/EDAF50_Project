#ifndef DBINMEM_H
#define DBINMEM_H

#include <string>
#include <map>
#include "newsgroup.h"

class dbinmemory : public dbinterface{

public:
  dbinmemory() : newsGroupCounter(0) {}   //Implement as singleton
  
private:
  std::map<int, Newsgroup> db;
  int newsGroupCounter;

};

#endif
