#pragma once

#include <cstdio>

#define foreach(name, list) \
for (auto \
  (name) = (list).begin(), \
     end = (list).end();   \
  (name) != end; ++(name))
#define unsupported exit(1)

class Scanner {
private:
  typedef FILE *File;
  File input;
protected:
  Scanner(File input);
  char peek;
  char consume();
};
Scanner::Scanner(File input):
  input(input) { peek = getc(input); }
char Scanner::consume() {
  char consumed = peek;
  peek = getc(input);
  return consumed;
}

