#pragma once
#include <vector>
#include <array>
#include <memory>

#include "reader/reader_interface.h"
#include "writer/writer_interface.h"
#include "binary_trie/binary_trie.h"

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
        int16_t code = 0;
        char length = 0;
    };

    struct SymbolWithCode {
        int16_t symbol = 0;
        HuffmanCode huffman;
    };

    using FrequenciesArray = std::array<size_t, kMaxAlphabetSize>;
    using HuffmanCodesArray = std::array<HuffmanCode, kMaxAlphabetSize>;

private:
    void AddCompressedFile(std::unique_ptr<ReaderInterface>& reader, std::unique_ptr<WriterInterface>& writer,
                           bool is_last);
    FrequenciesArray CountFrequencies(std::unique_ptr<ReaderInterface>& reader);
    HuffmanCodesArray BuildHuffmanCodes(const FrequenciesArray& frequencies);
    std::vector<SymbolWithCode> ToCanonical(HuffmanCodesArray& huffman_codes);
    void WriteHuffmanCode(std::unique_ptr<WriterInterface>& writer, HuffmanCode code);
    bool DecompressFile(std::unique_ptr<ReaderInterface>& reader, std::unique_ptr<WriterInterface>& writer,
                        const BinaryTrie<int16_t>& trie);
    BinaryTrie<int16_t> RestoreBinaryTrie(std::unique_ptr<ReaderInterface>& reader);
    int16_t ReadMaxHuffmanCodeBits(std::unique_ptr<ReaderInterface>& reader);
    int16_t ReadCodeWithTrie(std::unique_ptr<ReaderInterface>& reader, const BinaryTrie<int16_t>& trie);
    HuffmanCode ToHuffmanCode(const BinaryTrie<int16_t>::BinaryPath& binary_path);
    BinaryTrie<int16_t>::BinaryPath ToBinaryPath(const HuffmanCode& huffman_code);
};