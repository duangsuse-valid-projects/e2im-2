#pragma once

#include <list>
#include <string>

#include "aux_filepin.hpp"
#include "ext2pin/ext2pin.hpp"

using namespace std;
using path = string;

class ProtocolScanner: Scanner {
public:
  enum Action { Query, Pin, Unpin };
private:
  list<path> subject = *new list<path>();
protected:
  void onHandle(Action act, list<path> files);
  Action scanAction();
  unsigned scanCount();
  unsigned scanWs();
  void scanSubjectFiles(unsigned n);
  /** NOTE this scanner will accept empty text */
  path scanPath();
public:
  ProtocolScanner();
  ~ProtocolScanner() = delete;
  void handleNext();
};

////
int main();

