#include "filepin.hpp"

int main() {
  ProtocolScanner &scan = *new ProtocolScanner();
  while (true) 
    { scan.handleNext(); printf("\n"); }
  return 0;
}

ProtocolScanner::ProtocolScanner(): Scanner(stdin) {}

using Action = ProtocolScanner::Action;
#define check(res) if (res != 0) { printf("!%i", abs(res)); continue; }
void ProtocolScanner::onHandle(Action act, list<path> files) {
  switch (act) {
    case Action::Query:
        foreach (file, files) {
            FsAttrs attr;
            int res = getFileAttr(file->c_str(), attr);
            check(res)
            printf((attr & ATTR_I)? "t" : "f");
        }
        break;
    case Action::Pin:
        foreach (file, files) {
            FsAttrs attr;
            int res = getFileAttr(file->c_str(), attr);
            check(res)
            addFlag(attr, ATTR_I);
            res = setFileAttr(file->c_str(), attr);
            check(res) else printf(".");
        }
        break;
    case Action::Unpin:
        foreach (file, files) {
            FsAttrs attr;
            int res = getFileAttr(file->c_str(), attr);
            check(res)
            subFlag(attr, ATTR_I);
            res = setFileAttr(file->c_str(), attr);
            check(res) else printf(".");
        }
        break;
  }
}

void ProtocolScanner::handleNext() {
  Action act = scanAction();
  unsigned count = scanCount();
  if (count == 0) unsupported;
  unsigned wss = scanWs();
  if (wss == 0) unsupported;

  scanSubjectFiles(count);
  onHandle(act, subject);
  subject.clear();
  if (peek == '\n') consume();
  else unsupported;
}
ProtocolScanner::Action ProtocolScanner::scanAction() {
  switch (peek) {
    case '?':consume(); return Action::Query;
    case '+':consume(); return Action::Pin;
    case '-':consume(); return Action::Unpin;
  }
  unsupported;
}
unsigned int ProtocolScanner::scanCount() {
  unsigned n = 0;
  while (peek>='0' && peek<='9')
    n = n*10 + consume()-'0';
  return n;
}
bool isWs(char c) { return c==' ' || c=='\n'; }
unsigned ProtocolScanner::scanWs() {
  unsigned count = 0;
  while (isWs(peek)) { consume(); ++count; }
  return count;
}

path ProtocolScanner::scanPath() {
  path currentPath("");
  while (!isWs(peek)) {
    if (peek == '\\') { consume(); currentPath += consume(); }
    else { currentPath += consume(); }
  }
  return currentPath;
}
void ProtocolScanner::scanSubjectFiles(unsigned n) {
  for (; n!=0; --n) {
    subject.push_back(scanPath());
    while (peek == ' ') consume();
  }
}
