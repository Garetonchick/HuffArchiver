#include "file_writer.h"

FileWriter::FileWriter(std::string directory) : directory_(std::move(directory)) {
    if(!directory_.empty()) {
        directory_.push_back('/');
    }
}

void FileWriter::OpenFile(const std::string& filename) {
    file_.open(directory_ + filename);

    if(!file_) {
        throw std::runtime_error("WRITER::OPEN_FILE: Can't open file: " + directory_ + filename);
    }
}

void FileWriter::WriteByte(unsigned char byte) {
    file_.write(reinterpret_cast<char*>(&byte), 1);
}

void FileWriter::CloseFile() {
    Flush();
    file_.close();
}

void FileWriter::WriteBit(bool bit) {
    if(bit) {
        buffer_byte_ |= (1 << (7 - bit_pos_));
    }

    ++bit_pos_;

    if(bit_pos_ == 8) {
        Flush();
    }
}

void FileWriter::Flush() {
    if(bit_pos_ != 0) {
        WriteByte(buffer_byte_);
        buffer_byte_ = 0;
        bit_pos_ = 0;
    }
}