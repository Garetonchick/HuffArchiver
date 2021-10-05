#pragma once
#include "reader_interface.h"

#include <fstream>
#include <optional>

class Reader : public ReaderInterface {
public:
    explicit Reader(const std::string& file_path);
    Reader(const Reader& o) = delete;
    Reader& operator=(const Reader& o) = delete;
    Reader(Reader&& o) = default;
    Reader& operator=(Reader&& o) = default;

    bool HasNextByte() const override;
    bool HasNextBit() const override;
    const std::string& GetFileName() const override;

    unsigned char ReadNextByte() override;
    bool ReadNextBit() override;
    void Reset() override;

private:
    std::ifstream file_;
    std::string file_name_;
    size_t file_size_ = 0;
    size_t bytes_read_ = 0;
    size_t bit_pos_ = 0;
    unsigned char buffer_byte_ = 0;
};
