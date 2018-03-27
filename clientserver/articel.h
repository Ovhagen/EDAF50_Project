#ifndef ARTICEL_H
#define ARTICEL_H

#include <string>
#include <iostream>

class Articel{
public:
  Articel(std::string tt, int id, std::string auth, std::string tx):  //default case?
  titel(tt), articelId(id), author(auth), text(tx){}
  ~Articel() = default;

private:
  std::string titel;
  int articelId;
  std::string author;
  std::string text;
};


#endif
