#pragma once
#include "writer_interface.h"

#include <string>
#include <fstream>

class Writer : public WriterInterface {
public:
    explicit Writer(std::string dir);
    Writer(const Writer& o) = delete;
    Writer& operator=(const Writer& o) = delete;
    Writer(Writer&& o) = default;
    Writer& operator=(Writer&& o) = default;

    void OpenFile(const std::string& file_name) override;
    void CloseFile() override;
    void WriteByte(unsigned char byte) override;
    void WriteBit(bool bit) override;
    void Flush() override;

private:
    std::string dir_;
    std::ofstream file_;
    unsigned char buffer_byte_ = 0;
    char bit_pos_ = 0;
};