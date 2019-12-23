#pragma once

#if __WORDSIZE == 64
#define MIs64Bit
#endif

#define protecting(var, body) auto old##var = (var); \
  (body); (var) = old##var;
#define foreach(name, list) for (auto (name) = (list).begin(), end = (list).end(); (name) != end; ++(name))
#define impossible exit(1)
#define unsupported exit(1)

template <typename T>
void *voidPtr(T v) { return static_cast<void *>(v); }

////
typedef const char *CString;

typedef int FileId;
typedef struct stat FileState;
typedef unsigned long FsAttrs;

#define MGetFlags _IOR('f', 1, long) /**< Ext2 `getflags` ioctl call. */
#define MSetFlags _IOW('f', 2, long) /**< Ext2 `setflags` ioctl call. */

const FsAttrs ATTR_I = 0x00000010; /**< Ext2fs __Immutable__ file attribute. */

////
void addFlag(FsAttrs &fs, FsAttrs flag);
void subFlag(FsAttrs &fs, FsAttrs flag);

/** @return [-1(open failed), -2(stat failed), -3(file not normal)] */
FileId openAttrCtrlFd(CString path);
#define MPassAppOpenFail(fd) if (fd==-2 || fd==-1) return fd; \
  if (fd == -3) { errno = EOPNOTSUPP; return -1; }

/** @return [-1(open failed)/error, -2(stat failed), -3(file not normal)] */
int getFileAttr(CString path, FsAttrs &flags);
/** @return [-1(open failed)/error, -2(stat failed), -3(file not normal)] */
int setFileAttr(CString path, FsAttrs flags);

////
/**
 * Enter file "immutable" attribute query loop.
 * ';' means terminate of list, ' '|'\n' is whitespace, EOT(0x04) means end of transaction
 * ('+' File*) to add flag
 * ('-' File*) to remove flag
 * ('?' File*) to read flag, 't' is (hasFlag) and 'f' is (noFlag)
 * '\' can escape ';' ' ' as normal file name part
 */
int main();
const char EOT = '\x04';

////
class StdinParser {
protected:
  StdinParser();
  char peek;
  char consume();
};

class CommandInParser: protected StdinParser {
public:
  enum Action { PinFiles, UnpinFiles, CheckFiles };
private:
  Action s;
  std::list<std::string> pathList = *new std::list<std::string>();
  void names();
  Action action();

public:
  CommandInParser(): StdinParser() {}
  ~CommandInParser() = default;
  void handleNext();
  void handleAll();
  virtual void acceptCommand(Action act, std::list<std::string> path) = 0;
};

class AppCommandInParser: public CommandInParser {
protected:
    void acceptCommand(Action cmd, std::__cxx11::list< std::__cxx11::string > paths);
public:
    AppCommandInParser(): CommandInParser() {}
};
