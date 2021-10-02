#include "reader.h"


Reader::Reader(const std::string& file_path):
    file_(file_path, std::ios::binary) {}

unsigned char Reader::read_next_byte() {
  char buffer;
  file_.read(&buffer, 1);
  return buffer;
}

bool Reader::has_next_byte() {
  return !file_.eof();
}
