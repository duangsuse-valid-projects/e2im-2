#include "filepin.hpp"

int main() {
  ProtocolScanner &scan = *new ProtocolScanner();
  while (true) scan.handleNext();
  return 0;
}

ProtocolScanner::ProtocolScanner(): Scanner(stdin) {}

using Action = ProtocolScanner::Action;
#define check(res) if (res != 0) { printf("!%i", res); continue; }
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
            check(res)
        }
        break;
    case Action::Unpin:
        foreach (file, files) {
            FsAttrs attr;
            int res = getFileAttr(file->c_str(), attr);
            check(res)
            subFlag(attr, ATTR_I);
            res = setFileAttr(file->c_str(), attr);
            check(res)
        }
        break;
  }
}

void ProtocolScanner::handleNext() {
  Action act = scanAction();
  unsigned count = scanCount();
  skipWs();
  scanSubjectFiles();
  onHandle(act, subject);
  subject.clear();
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
    n = n*10 + peek-'0';
  return n;
}
bool isWs(char c) { return c==' ' || c=='\n'; }
void ProtocolScanner::skipWs() {
  while (isWs(peek)) consume();
}
path ProtocolScanner::scanPath() {
  path currentPath("");
  while (!isWs(peek)) {
    if (peek == '\\') { consume(); currentPath += consume(); }
    else { currentPath += consume(); }
  }
  return currentPath;
}
void ProtocolScanner::scanSubjectFiles() {
  while (peek != '\n') {
    subject.push_back(scanPath());
    while (peek == ' ') consume();
  }
}
