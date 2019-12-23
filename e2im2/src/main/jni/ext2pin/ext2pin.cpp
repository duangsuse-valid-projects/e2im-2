#include "ext2pin.hpp"

using namespace std;

////
void addFlag(FsAttrs &fs, FsAttrs flag)
  { fs = fs | flag; }
void subFlag(FsAttrs &fs, FsAttrs flag)
  { fs = fs & ~flag; }

FailableFileId openAttrCtrlFd(CString path) {
  FileState fState;
  if (stat(path, &fState) != 0) return StatFail;
  if (notNormalFile(fState)) return NotSupported;
  auto res = open(path, O_RDONLY | O_NONBLOCK);
  return static_cast<FailableFileId>(res);
}

////
int getFileAttr(CString path, FsAttrs &flags) {
  FileId fd; int ioctlRes;
  fd = openAttrCtrlFd(path); MPassAppOpenFail(fd)

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
  fd = openAttrCtrlFd(path); MPassAppOpenFail(fd)

#ifdef MIs64Bit
  int buffer; // 64bit
  buffer = (int) flags;
  ioctlRes = ioctl(fd, MSetFlags, &buffer);
#else
  ioctlRes = ioctl(fd, MSetFlags, voidPtr(&flags));
#endif

  closeSilently(fd); return ioctlRes;
}
