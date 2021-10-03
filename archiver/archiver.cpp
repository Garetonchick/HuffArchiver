#include "archiver.h"
#include "../binary_trie/binary_trie.h"
#include <queue>

void Archiver::Compress(const std::vector<Reader>& inputs, const Writer& out) {
    frequencies[size_t(SpecialCodes::kFileNameEnd)] = 1;
    frequencies[size_t(SpecialCodes::kOneMoreFile)] = 1;
    frequencies[size_t(SpecialCodes::kArchiveEnd)] = 1;

    for(const Reader& reader : inputs) {
        while(reader.HasNextByte()) {
            ++frequencies[reader.ReadNextByte()];
        }
    }

    std::priority_queue<std::pair<size_t, BinaryTrie<short>>, std::vector<std::pair<size_t, BinaryTrie<short>>>,
                        std::greater<>> queue;

    for(size_t i = 0; i < kAlphabetSize; ++i) {
        if (frequencies[i]) {
            queue.emplace(frequencies[i], BinaryTrie<short>(i));
        }
    }

    while(queue.size() > 1) {
        auto top1(std::move(queue.top()));
        queue.pop();
        auto top2(std::move(queue.top()));
        queue.pop();

        top1.second.Merge(top2.second);
        queue.emplace(top1.first + top2.first, top1.second);
    }


}

void Archiver::Decompress(const Reader& in, const Writer& out) {

}