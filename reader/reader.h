#pragma once
#include <fstream>
#include <optional>

class Reader {
public:
    explicit Reader(const std::string& file_path);

    unsigned char ReadNextByte();
    bool HasNextByte() const;

private:
    std::ifstream file_;
    size_t file_size_;
    size_t bytes_read_ = 0;
};
