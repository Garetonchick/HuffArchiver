#include "file_reader.h"

#include <algorithm>
#include <stdexcept>

FileReader::FileReader(const std::string& file_path) : file_(file_path, std::ios::binary | std::ios::ate) {
    if (!file_) {
        throw std::runtime_error("READER: Can't open file: " + file_path);
    }

    size_t name_start_pos = file_path.find_last_of("/\\");

    if(name_start_pos == std::string::npos) {
        name_start_pos = 0;
    } else {
        ++name_start_pos;
    }

    filename_ = file_path.substr(name_start_pos);

    file_size_ = file_.tellg();
    file_.seekg(0);
}

bool FileReader::HasNextByte() const {
    return bytes_read_ < file_size_;
}

bool FileReader::HasNextBit() const {
    return bytes_read_ < file_size_;
}

const std::string& FileReader::GetFileName() const {
    return filename_;
}

unsigned char FileReader::ReadNextByte() {
    char buffer;

    file_.read(&buffer, 1);
    ++bytes_read_;

    if (bit_pos_ != 0) {
        ++bytes_read_;
        bit_pos_ = 0;
        buffer_byte_ = 0;
    }

    return buffer;
}

bool FileReader::ReadNextBit() {
    bool bit = false;

    if (bit_pos_ == 0) {
        char buffer;
        file_.read(&buffer, 1);

        buffer_byte_ = buffer;
    }

    bit = ((buffer_byte_ >> (7 - bit_pos_)) & 1);

    if (bit_pos_ == 7) {
        bit_pos_ = 0;
        ++bytes_read_;
    } else {
        ++bit_pos_;
    }

    return bit;
}

void FileReader::Reset() {
    file_.seekg(0);
    bytes_read_ = 0;
    buffer_byte_ = 0;
    bit_pos_ = 0;
}
