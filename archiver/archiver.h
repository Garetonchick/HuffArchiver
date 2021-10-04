#pragma once
#include "../reader/reader_interface.h"
#include "../writer/writer_interface.h"
#include "../binary_trie/binary_trie.h"

#include <vector>
#include <array>
#include <memory>

class Archiver {
public:
    void Compress(std::vector<std::unique_ptr<ReaderInterface>>&& readers, std::unique_ptr<WriterInterface> writer,
                  const std::string& output_file_name);
    void Decompress(std::unique_ptr<ReaderInterface> reader, std::unique_ptr<WriterInterface> writer);

private:
    static const size_t kMaxAlphabetSize = 259;
    static const size_t kMaxHuffmanCodeBits = 9;

    enum class SpecialCodes { kFileNameEnd = 256, kOneMoreFile = 257, kArchiveEnd = 258 };

    struct HuffmanCode {
        short code = 0;
        char length = 0;
    };

    struct SymbolWithCode {
        short symbol = 0;
        HuffmanCode huffman;
    };

    using FrequenciesArray = std::array<size_t, kMaxAlphabetSize>;
    using HuffmanCodesArray = std::array<HuffmanCode, kMaxAlphabetSize>;

private:
    void AddCompressedFile(std::unique_ptr<ReaderInterface>& reader, std::unique_ptr<WriterInterface>& writer,
                           bool is_archive_end);
    FrequenciesArray CountFrequencies(std::unique_ptr<ReaderInterface>& reader);
    HuffmanCodesArray BuildHuffmanCodes(const FrequenciesArray& frequencies);
    std::vector<SymbolWithCode> ToCanonical(HuffmanCodesArray& huffman_codes);
    void WriteHuffmanCode(std::unique_ptr<WriterInterface>& writer, HuffmanCode code);

    bool DecompressFile(std::unique_ptr<ReaderInterface>& reader, std::unique_ptr<WriterInterface>& writer,
                        const BinaryTrie<short>& trie);
    BinaryTrie<short> RestoreBinaryTrie(std::unique_ptr<ReaderInterface>& reader);
    short ReadMaxHuffmanCodeBits(std::unique_ptr<ReaderInterface>& reader);
    short ReadCodeWithTrie(std::unique_ptr<ReaderInterface>& reader, const BinaryTrie<short>& trie);
    HuffmanCode ToHuffmanCode(const BinaryTrie<short>::BinaryPath& binary_path);
    BinaryTrie<short>::BinaryPath ToBinaryPath(const HuffmanCode& huffman_code);
};