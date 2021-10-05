#include "archiver.h"

#include <algorithm>
#include <tuple>
#include <utility>

#include "../priority_queue/priority_queue.h"

void Archiver::Compress(std::vector<std::unique_ptr<ReaderInterface>>&& readers,
                        std::unique_ptr<WriterInterface> writer, const std::string& output_file_name) {
    writer->OpenFile(output_file_name);

    for (size_t i = 0; i < readers.size(); ++i) {
        AddCompressedFile(readers[i], writer, i + 1 == readers.size());
    }

    writer->CloseFile();
}

void Archiver::Decompress(std::unique_ptr<ReaderInterface> reader, std::unique_ptr<WriterInterface> writer) {
    while (true) {
        BinaryTrie<short> trie = RestoreBinaryTrie(reader);

        if (!Archiver::DecompressFile(reader, writer, trie)) {
            break;
        }
    }
}

void Archiver::AddCompressedFile(std::unique_ptr<ReaderInterface>& reader, std::unique_ptr<WriterInterface>& writer,
                                 bool is_archive_end) {
    FrequenciesArray frequencies = CountFrequencies(reader);
    HuffmanCodesArray huffman_codes = BuildHuffmanCodes(frequencies);

    auto sorted_symbols = ToCanonical(huffman_codes);

    WriteHuffmanCode(writer, {.code = short(sorted_symbols.size()), .length = kMaxHuffmanCodeBits});

    for (SymbolWithCode symbol : sorted_symbols) {
        WriteHuffmanCode(writer, {.code = symbol.symbol, .length = kMaxHuffmanCodeBits});
    }

    char last_length = 1;
    short length_count = 0;

    for (SymbolWithCode symbol : sorted_symbols) {
        if (last_length != symbol.huffman.length) {
            WriteHuffmanCode(writer, {.code = length_count, .length = kMaxHuffmanCodeBits});
            ++last_length;

            while (last_length < symbol.huffman.length) {
                WriteHuffmanCode(writer, {.code = 0, .length = kMaxHuffmanCodeBits});
                ++last_length;
            }

            length_count = 1;
        } else {
            ++length_count;
        }
    }

    if (length_count != 0) {
        WriteHuffmanCode(writer, {.code = length_count, .length = kMaxHuffmanCodeBits});
    }

    for (char c : reader->GetFileName()) {
        WriteHuffmanCode(writer, huffman_codes[c]);
    }

    WriteHuffmanCode(writer, huffman_codes[size_t(SpecialCodes::kFileNameEnd)]);

    reader->Reset();

    while (reader->HasNextByte()) {
        WriteHuffmanCode(writer, huffman_codes[reader->ReadNextByte()]);
    }

    if (is_archive_end) {
        WriteHuffmanCode(writer, huffman_codes[size_t(SpecialCodes::kArchiveEnd)]);
    } else {
        WriteHuffmanCode(writer, huffman_codes[size_t(SpecialCodes::kOneMoreFile)]);
    }
}

Archiver::FrequenciesArray Archiver::CountFrequencies(std::unique_ptr<ReaderInterface>& reader) {
    FrequenciesArray frequencies = {0};

    frequencies[size_t(SpecialCodes::kFileNameEnd)] = 1;
    frequencies[size_t(SpecialCodes::kOneMoreFile)] = 1;
    frequencies[size_t(SpecialCodes::kArchiveEnd)] = 1;

    for (char c : reader->GetFileName()) {
        ++frequencies[c];
    }

    while (reader->HasNextByte()) {
        ++frequencies[reader->ReadNextByte()];
    }

    return frequencies;
}

Archiver::HuffmanCodesArray Archiver::BuildHuffmanCodes(const FrequenciesArray& frequencies) {
    struct QueueNode {
        bool operator<(const QueueNode& o) const {
            return priority < o.priority;
        }

        size_t priority;
        size_t trie_index;
    };

    std::vector<BinaryTrie<short>> tries;
    PriorityQueue<QueueNode> queue;

    for (size_t i = 0; i < kMaxAlphabetSize; ++i) {
        if (frequencies[i]) {
            queue.Push({.priority = frequencies[i], .trie_index = tries.size()});
            tries.emplace_back(i);
        }
    }

    while (queue.GetSize() > 1) {
        auto [prior1, idx1] = queue.Top();
        queue.Pop();
        auto [prior2, idx2] = queue.Top();
        queue.Pop();

        tries[idx1].Merge(std::move(tries[idx2]));
        queue.Push({.priority = prior1 + prior2, .trie_index = idx1});
    }

    size_t final_idx = queue.Top().trie_index;
    BinaryTrie<short> trie(std::move(tries[final_idx]));

    std::array<HuffmanCode, kMaxAlphabetSize> huffman_codes;

    for (auto iter = trie.begin(); iter != trie.end(); ++iter) {
        auto path = iter.GetPath();

        huffman_codes[*iter] = ToHuffmanCode(path);
    }

    return huffman_codes;
}

std::vector<Archiver::SymbolWithCode> Archiver::ToCanonical(HuffmanCodesArray& huffman_codes) {
    std::vector<short> codes_order;

    for (size_t i = 0; i < kMaxAlphabetSize; ++i)
        if (huffman_codes[i].length != 0) {
            codes_order.push_back(i);
        }

    std::sort(codes_order.begin(), codes_order.end(), [&huffman_codes](short a, short b) {
        return std::tie(huffman_codes[a].length, a) < std::tie(huffman_codes[b].length, b);
    });

    if (!codes_order.empty()) {
        huffman_codes[codes_order[0]].code = 0;
    }

    for (size_t i = 1; i < codes_order.size(); ++i) {
        char length = huffman_codes[codes_order[i]].length;
        char previous_length = huffman_codes[codes_order[i - 1]].length;

        huffman_codes[codes_order[i]].code = huffman_codes[codes_order[i - 1]].code + 1;

        if (length > previous_length) {
            huffman_codes[codes_order[i]].code <<= (length - previous_length);
        }
    }

    std::vector<SymbolWithCode> symbols;

    for (short i : codes_order) {
        symbols.push_back({.symbol = i, .huffman = huffman_codes[i]});
    }

    return symbols;
}

void Archiver::WriteHuffmanCode(std::unique_ptr<WriterInterface>& writer, HuffmanCode code) {
    for (char i = 0; i < code.length; ++i) {
        writer->WriteBit((code.code >> (code.length - 1 - i)) & 1);
    }
}

bool Archiver::DecompressFile(std::unique_ptr<ReaderInterface>& reader, std::unique_ptr<WriterInterface>& writer,
                              const BinaryTrie<short>& trie) {
    std::string file_name;

    while (true) {
        short symbol = ReadCodeWithTrie(reader, trie);

        if (symbol == short(SpecialCodes::kFileNameEnd)) {
            break;
        }

        file_name.push_back(char(symbol));
    }

    writer->OpenFile(file_name);

    bool has_one_more_file = false;

    while (true) {
        short symbol = ReadCodeWithTrie(reader, trie);

        if (symbol == short(SpecialCodes::kOneMoreFile) || symbol == short(SpecialCodes::kArchiveEnd)) {
            has_one_more_file = (symbol == short(SpecialCodes::kOneMoreFile));
            break;
        }

        writer->WriteByte(symbol);
    }

    writer->CloseFile();

    return has_one_more_file;
}

BinaryTrie<short> Archiver::RestoreBinaryTrie(std::unique_ptr<ReaderInterface>& reader) {
    short symbols_count = ReadMaxHuffmanCodeBits(reader);
    std::vector<short> alphabet(symbols_count);

    for (short& symbol : alphabet) {
        symbol = ReadMaxHuffmanCodeBits(reader);
    }

    BinaryTrie<short> trie;

    {
        HuffmanCode huffman{.length = 1};

        for (short symbols_processed = 0; symbols_processed < symbols_count; ++huffman.length) {
            short length_count = ReadMaxHuffmanCodeBits(reader);

            while (length_count--) {
                trie.Insert(alphabet[symbols_processed], ToBinaryPath(huffman));
                ++huffman.code;
                ++symbols_processed;
            }

            huffman.code <<= 1;
        }
    }

    return trie;
}

short Archiver::ReadMaxHuffmanCodeBits(std::unique_ptr<ReaderInterface>& reader) {
    short code = 0;

    for (size_t i = 0; i < kMaxHuffmanCodeBits; ++i) {
        if (!reader->HasNextBit()) {
            throw std::invalid_argument("ARCHIVER::DECOMPRESS: Invalid file format");
        }

        if (reader->ReadNextBit()) {
            code |= (1 << (kMaxHuffmanCodeBits - 1 - i));
        }
    }

    return code;
}

short Archiver::ReadCodeWithTrie(std::unique_ptr<ReaderInterface>& reader, const BinaryTrie<short>& trie) {
    auto traverser = trie.GetRootTraverser();
    bool bad_trie = false;

    while (!traverser.HasValue()) {
        if (!reader->HasNextBit()) {
            throw std::invalid_argument("ARCHIVER::DECOMPRESS: Invalid file format");
        }

        if (reader->ReadNextBit()) {

            if (!traverser.CanGoRight()) {
                bad_trie = true;
                break;
            }

            traverser.GoRight();
        } else {
            if (!traverser.CanGoLeft()) {
                bad_trie = true;
                break;
            }

            traverser.GoLeft();
        }
    }

    if (bad_trie || !traverser.HasValue()) {
        throw std::runtime_error("ARCHIVER::DECOMPRESS: Something went wrong with BinaryTrie");
    }

    return traverser.GetValue();
}

Archiver::HuffmanCode Archiver::ToHuffmanCode(const BinaryTrie<short>::BinaryPath& binary_path) {
    HuffmanCode huffman{.length = char(binary_path.length)};

    for (char i = 0; i < huffman.length; ++i) {
        if ((binary_path.code >> i) & 1) {
            huffman.code |= (1 << (huffman.length - 1 - i));
        }
    }

    return huffman;
}

BinaryTrie<short>::BinaryPath Archiver::ToBinaryPath(const HuffmanCode& huffman_code) {
    BinaryTrie<short>::BinaryPath path{.length = size_t(huffman_code.length)};

    for (size_t i = 0; i < path.length; ++i) {
        if ((huffman_code.code >> i) & 1) {
            path.code |= (1 << (path.length - 1 - i));
        }
    }

    return path;
}