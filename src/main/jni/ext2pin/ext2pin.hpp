#pragma once

#include <cerrno>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include "aux_ext2pin.hpp"

////
typedef int FileId;
typedef struct stat FileState;
typedef unsigned long FsAttrs;

#define MGetFlags _IOR('f', 1, long) /**< Ext2 `getflags` ioctl call. */
#define MSetFlags _IOW('f', 2, long) /**< Ext2 `setflags` ioctl call. */

const FsAttrs ATTR_I = 0x00000010; /**< Ext2fs __Immutable__ file attribute. */

////
enum FailableFileId: FileId
  { OpenFail = (-1), StatFail = (-2), NotSupported = (-3) };
static inline void closeSilently(FileId fd)
  { protecting(errno, close(fd)); }
static inline bool notNormalFile(FileState s)
  { return !S_ISREG(s.st_mode) && !S_ISDIR(s.st_mode); }

////
void addFlag(FsAttrs &fs, FsAttrs flag);
void subFlag(FsAttrs &fs, FsAttrs flag);

FailableFileId openAttrCtrlFd(CString path);
#define MPassAppOpenFail(fd) if (fd==OpenFail || fd==StatFail || fd==NotSupported) return fd;

////
int getFileAttr(CString path, FsAttrs &flags);
int setFileAttr(CString path, FsAttrs flags);
