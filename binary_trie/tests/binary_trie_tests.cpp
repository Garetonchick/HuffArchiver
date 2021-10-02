#include "../binary_trie.h"
#include <gtest/gtest.h>

#include <vector>
#include <random>
#include <functional>
#include <chrono>

std::mt19937 random_gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
auto gen_number = std::bind(std::uniform_int_distribution<size_t>(0, 10000), random_gen);

template<class T>
void CheckIteration(const BinaryTrie<T>& trie, std::vector<T>& expected_traversal) {
    {
        size_t i = 0;

        for (auto it : trie) {
            ASSERT_EQ(it, expected_traversal[i]);
            ++i;
        }
    }

    {
        size_t i = 0;

        for (auto it = trie.begin(); it != trie.end(); ++it) {
            ASSERT_EQ(*it, expected_traversal[i]);
            ++i;
        }
    }
}

TEST(BinaryTrie, BinaryTrieTest) {
    std::vector<char> expected_traversal = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k'};
    BinaryTrie<char> trie('a');

    for(size_t i = 1; i < expected_traversal.size(); ++i) {
        trie.Merge(BinaryTrie<char>(expected_traversal[i]));
    }

    CheckIteration(trie, expected_traversal);
}

TEST(BinaryTrie, BinaryTrieRandomTest1) {
    std::vector<size_t> expected_traversal(10000);

    expected_traversal[0] = gen_number();
    BinaryTrie trie(expected_traversal[0]);

    for(size_t i = 1; i < expected_traversal.size(); ++i) {
        expected_traversal[i] = gen_number();
        trie.Merge(BinaryTrie<size_t>(expected_traversal[i]));
    }

    CheckIteration(trie, expected_traversal);
}

TEST(BinaryTrie, BinaryTrieManualTest) {
    std::vector<size_t> expected_traversal = {0, 1, 2, 3};

    BinaryTrie trie(expected_traversal[0]);
    trie.Merge(BinaryTrie<size_t>(expected_traversal[1]));
    BinaryTrie trie2(expected_traversal[2]);
    trie2.Merge(BinaryTrie<size_t>(expected_traversal[3]));
    trie.Merge(std::move(trie2));

    CheckIteration(trie, expected_traversal);
}

template<class T>
BinaryTrie<T> RandomRecursiveMerge(const std::vector<T>& traversal, size_t l, size_t r) {
    if(l == r) {
        return BinaryTrie<T>(traversal[l]);
    }

    size_t mid = std::uniform_int_distribution<size_t>(l, r - 1)(random_gen);

    BinaryTrie<T> left = RandomRecursiveMerge(traversal, l, mid);
    BinaryTrie<T> right = RandomRecursiveMerge(traversal, mid + 1, r);

    left.Merge(std::move(right));

    return left;
}

template<class T>
void DumpTrieIteration(const BinaryTrie<T>& trie) {
    std::cout << "Trie dump:" << std::endl;

    for (auto it : trie) {
        std::cout << it << " ";
    }

    std::cout << std::endl;
}

template<class T>
void DumpExpectedTraversal(const std::vector<T>& expected_traversal) {
    std::cout << "Expected traversal" << std::endl;

    for(auto it : expected_traversal) {
        std::cout << it << " ";
    }

    std::cout << std::endl;
}

TEST(BinaryTrie, BinaryTrieRandomTest2) {
    std::vector<size_t> expected_traversal(10000);

    for(size_t i = 0; i < expected_traversal.size(); ++i) {
        expected_traversal[i] = gen_number();
    }

    BinaryTrie trie(RandomRecursiveMerge(expected_traversal, 0, expected_traversal.size() - 1));

    CheckIteration(trie, expected_traversal);

    //DumpExpectedTraversal(expected_traversal);
    //DumpTrieIteration(trie);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
