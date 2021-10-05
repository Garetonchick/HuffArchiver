#include "writer.h"

Writer::Writer(std::string dir) : dir_(std::move(dir)) {
}

void Writer::OpenFile(const std::string& file_name) {
    file_.open(dir_ + file_name);

    if(!file_) {
        throw std::runtime_error("WRITER::OPEN_FILE: Can't open file: " + dir_ + file_name);
    }
}

void Writer::WriteByte(unsigned char byte) {
    file_.write(reinterpret_cast<char*>(&byte), 1);
}

void Writer::CloseFile() {
    Flush();
    file_.close();
}

void Writer::WriteBit(bool bit) {
    if(bit) {
        buffer_byte_ |= (1 << (7 - bit_pos_));
    }

    ++bit_pos_;

    if(bit_pos_ == 8) {
        Flush();
    }
}

void Writer::Flush() {
    if(bit_pos_ != 0) {
        WriteByte(buffer_byte_);
        buffer_byte_ = 0;
        bit_pos_ = 0;
    }
}