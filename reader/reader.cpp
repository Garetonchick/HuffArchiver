#include "reader.h"

#include <algorithm>
#include <stdexcept>

Reader::Reader(const std::string& file_path) : file_(file_path, std::ios::binary | std::ios::ate) {
    if(!file_) {
        throw std::runtime_error("READER: Can't open file:" + file_path);
    }

    size_t name_start_pos = std::find_if(file_path.rbegin(), file_path.rend(), [](char c)
                                         { return c == '/' || c == '\\'; }) - file_path.rbegin();

    name_start_pos = file_path.size() - name_start_pos;

    if(name_start_pos < file_path.size()) {
        file_name_ = file_path.substr(name_start_pos, file_path.size() - name_start_pos);
    }

    file_size_ = file_.tellg();
    file_.seekg(0);
}

bool Reader::HasNextByte() const {
    return bytes_read_ < file_size_;
}

bool Reader::HasNextBit() const {
    return bytes_read_ < file_size_;
}

const std::string& Reader::GetFileName() const {
    return file_name_;
}

unsigned char Reader::ReadNextByte() {
    char buffer;

    file_.read(&buffer, 1);
    ++bytes_read_;

    if(bit_pos_ != 0) {
        ++bytes_read_;
        bit_pos_ = 0;
        buffer_byte_ = 0;
    }

    return buffer;
}

bool Reader::ReadNextBit() {
    bool bit = false;

    if(bit_pos_ == 0) {
        char buffer;
        file_.read(&buffer, 1);

        buffer_byte_ = buffer;
    }

    bit = ((buffer_byte_ >> (7 - bit_pos_)) & 1);

    if(bit_pos_ == 7) {
        bit_pos_ = 0;
        ++bytes_read_;
    } else {
        ++bit_pos_;
    }

    return bit;
}

void Reader::Reset() {
    file_.seekg(0);
    bytes_read_ = 0;
    buffer_byte_ = 0;
    bit_pos_ = 0;
}


