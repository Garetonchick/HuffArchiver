#include "writer.h"

Writer::Writer(const std::string& file_path) : file_(file_path, std::ios::binary | std::ios::out) {
}

void Writer::WriteByte(unsigned char byte) {
    file_.write(reinterpret_cast<char*>(&byte), 1);
}

void Writer::CloseFile() {
    file_.close();
}