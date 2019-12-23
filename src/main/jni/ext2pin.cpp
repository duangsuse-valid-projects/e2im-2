#include <iostream>
#include <cerrno>
#include <list>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include "ext2pin.hpp"

using namespace std;

////
void addFlag(FsAttrs &fs, FsAttrs flag)
  { fs = fs | flag; }
void subFlag(FsAttrs &fs, FsAttrs flag)
  { fs = fs & ~flag; }
static inline void closeSilently(FileId fd) {
  protecting(errno, close(fd));
}

static inline bool notNormalFile(FileState s)
  { return !S_ISREG(s.st_mode) && !S_ISDIR(s.st_mode); }
FileId openAttrCtrlFd(CString path) {
  FileState fState;
  if (stat(path, &fState) != 0) return -2; // stat fails
  if (notNormalFile(fState)) return -3; // not acceptable
  return open(path, O_RDONLY | O_NONBLOCK);
}

////
int getFileAttr(CString path, FsAttrs &flags) {
  FileId fd; int ioctlRes;
  fd = openAttrCtrlFd(path); MPassAppOpenFail(fd);

#ifdef MIs64Bit
  int buffer; // 64bit
  ioctlRes = ioctl(fd, MGetFlags, &buffer);
  flags = static_cast<FsAttrs>(buffer);
#else
  ioctlRes = ioctl(fd, MGetFlags, voidPtr(&flags));
#endif

  closeSilently(fd); return ioctlRes;
}

int setFileAttr(CString path, FsAttrs flags) {
  FileId fd; int ioctlRes;
  fd = openAttrCtrlFd(path); MPassAppOpenFail(fd);

#ifdef MIs64Bit
  int buffer; // 64bit
  buffer = (int) flags;
  ioctlRes = ioctl(fd, MSetFlags, &buffer);
#else
  ioctlRes = ioctl(fd, MSetFlags, voidPtr(&flags));
#endif

  closeSilently(fd); return ioctlRes;
}

////
StdinParser::StdinParser()
  { std::cin >> peek; }
char StdinParser::consume() {
  char consumed = peek;
  std::cin >> peek;
  return consumed;
}

void CommandInParser::names() {
  string currentPath = *new string("");
nextCharItem:
  if (peek == '\\') { consume(); currentPath += consume(); }
  else switch (peek) {
    case ' ': case '\n': consume();
      pathList.push_back(currentPath);
      currentPath = *new string("");
      goto nextCharItem;
    case EOT: case ';': return;
    default:
      currentPath += consume();
      goto nextCharItem;
  }
}
using State = CommandInParser::Action;
State CommandInParser::action() {
  switch (peek) {
    case '+': consume(); return PinFiles;
    case '-': consume(); return UnpinFiles;
    case '?': consume(); return CheckFiles;
    default: cerr << "E: unk act `"<<peek<<'\'' << endl;
    unsupported;
  }
}
void CommandInParser::handleNext() {
retry:
  switch (peek) {
    case ' ': case '\n': consume(); goto retry;
    case EOT: return;
    case ';': consume(); acceptCommand(s, pathList); pathList.clear(); break;
    default: s = action(); names(); break;
  }
}
void CommandInParser::handleAll() {
  while (peek != EOT) handleNext();
}

void AppCommandInParser::acceptCommand(Action cmd, std::list<string> paths) {
  switch (cmd) {
    case Action::PinFiles:
      foreach(it, paths) { cout << "+" << *it; }
    case Action::UnpinFiles:
      foreach(it, paths) { cout << "+" << *it; }
    case Action::CheckFiles:
      foreach(it, paths) { cout << "+" << *it; }
    default: impossible;
  }
}

int main() {
  AppCommandInParser app = *new AppCommandInParser;
  app.handleAll();
}

