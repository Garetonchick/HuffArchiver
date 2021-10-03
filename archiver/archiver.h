#pragma once
#include "../reader/reader.h"
#include "../writer/writer.h"

#include <vector>
#include <array>

class Archiver {
public:
    void Compress(const std::vector<Reader>& inputs, const Writer& out);
    void Decompress(const Reader& in, const Writer& out);

private:
    enum class SpecialCodes { kFileNameEnd = 256, kOneMoreFile = 257, kArchiveEnd = 258 };

private:
    static const size_t kAlphabetSize = 259;

    std::array<size_t, kAlphabetSize> frequencies;
};