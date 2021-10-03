#include "reader.h"

Reader::Reader(const std::string& file_path) : file_(file_path, std::ios::binary | std::ios::ate) {
    file_size_ = file_.tellg();
    file_.seekg(0);
}

unsigned char Reader::ReadNextByte() const {
    char buffer;

    file_.read(&buffer, 1);
    ++bytes_read_;

    return buffer;
}

bool Reader::HasNextByte() const {
    return bytes_read_ < file_size_;
}
