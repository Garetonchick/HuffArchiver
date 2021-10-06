#include "binary_trie/binary_trie.h"
#include <gtest/gtest.h>

#include <vector>
#include <random>
#include <functional>
#include <chrono>
#include <bitset>
#include <queue>

std::mt19937 random_gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
auto gen_number = std::bind(std::uniform_int_distribution<size_t>(0, 10), random_gen);

template <class T>
void CheckIteration(const BinaryTrie<T>& trie, const std::vector<T>& expected_traversal) {
    {
        size_t i = 0;

        for (auto it : trie) {
            ASSERT_TRUE(i < expected_traversal.size());
            ASSERT_EQ(it, expected_traversal[i]);
            ++i;
        }
    }

    {
        size_t i = 0;

        for (auto it = trie.begin(); it != trie.end(); ++it) {
            ASSERT_TRUE(i < expected_traversal.size());
            ASSERT_EQ(*it, expected_traversal[i]);
            ++i;
        }
    }
}

template <class T>
void CheckIteratorPaths(const BinaryTrie<T>& trie, const std::vector<size_t>& expected_path_codes,
                        const std::vector<size_t>& expected_path_lengths) {
    size_t i = 0;

    for (auto iter = trie.begin(); iter != trie.end(); ++iter) {
        ASSERT_EQ(iter.GetPath().code, expected_path_codes[i]);
        ASSERT_EQ(iter.GetPath().length, expected_path_lengths[i]);
        ++i;
    }
}

template <class T>
void CheckTraversing(const BinaryTrie<T>& trie, const std::vector<T>& expected_traversal,
                     const std::vector<size_t>& expected_path_codes, const std::vector<size_t>& expected_path_lengths) {

    for (size_t i = 0; i < expected_traversal.size(); ++i) {
        auto traverser = trie.GetRootTraverser();

        for (size_t j = 0; j < expected_path_lengths[i]; ++j) {
            if ((expected_path_codes[i] >> j) & 1) {
                ASSERT_TRUE(traverser.CanGoRight());
                traverser.GoRight();
            } else {
                ASSERT_TRUE(traverser.CanGoLeft());
                traverser.GoLeft();
            }
        }

        ASSERT_EQ((*traverser).code, expected_path_codes[i]);
        ASSERT_EQ((*traverser).length, expected_path_lengths[i]);
        ASSERT_TRUE(traverser.HasValue());
        ASSERT_EQ(traverser.GetValue(), expected_traversal[i]);
    }
}

template <class T>
void ApplyPermutation(const std::vector<size_t>& perm, std::vector<T>& v) {
    auto copy = v;

    for (size_t i = 0; i < v.size(); ++i)
        v[i] = copy[perm[i]];
}

TEST(BinaryTrie, BinaryTrieTest) {
    std::vector<char> expected_traversal = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k'};
    BinaryTrie<char> trie('a');

    for (size_t i = 1; i < expected_traversal.size(); ++i) {
        trie.Merge(BinaryTrie<char>(expected_traversal[i]));
    }

    CheckIteration(trie, expected_traversal);
}

TEST(BinaryTrie, BinaryTrieRandomTest1) {
    std::vector<size_t> expected_traversal(20);

    expected_traversal[0] = gen_number();
    BinaryTrie trie(expected_traversal[0]);

    for (size_t i = 1; i < expected_traversal.size(); ++i) {
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

template <class T>
BinaryTrie<T> RandomRecursiveMerge(const std::vector<T>& traversal, size_t l, size_t r) {
    if (l == r) {
        return BinaryTrie<T>(traversal[l]);
    }

    size_t mid = std::uniform_int_distribution<size_t>(l, r - 1)(random_gen);

    BinaryTrie<T> left = RandomRecursiveMerge(traversal, l, mid);
    BinaryTrie<T> right = RandomRecursiveMerge(traversal, mid + 1, r);

    left.Merge(std::move(right));

    return left;
}

TEST(BinaryTrie, BinaryTrieRandomTest2) {
    std::vector<size_t> expected_traversal(20);

    for (size_t i = 0; i < expected_traversal.size(); ++i) {
        expected_traversal[i] = gen_number();
    }

    BinaryTrie trie(RandomRecursiveMerge(expected_traversal, 0, expected_traversal.size() - 1));

    CheckIteration(trie, expected_traversal);
}

TEST(BinaryTrie, BinaryTrieIteratorPathTest1) {
    std::vector<size_t> expected_traversal(20);

    for (size_t i = 0; i < expected_traversal.size(); ++i) {
        expected_traversal[i] = gen_number();
    }

    BinaryTrie<size_t> trie;

    for (size_t i = 0; i < expected_traversal.size(); ++i) {
        expected_traversal[i] = gen_number();
        trie.Merge(BinaryTrie<size_t>(expected_traversal[i]));
    }

    CheckIteration(trie, expected_traversal);

    {
        size_t i = 0;

        for (auto iter = trie.begin(); iter != trie.end(); ++iter) {
            size_t expected_code = (size_t(1) << (expected_traversal.size() - 1 - i));
            size_t expected_path_length = expected_traversal.size() - i;

            if (i == 0) {
                expected_code = 0;
                expected_path_length = expected_traversal.size() - 1;
            }

            ASSERT_TRUE(iter.GetPath().code == expected_code);
            ASSERT_TRUE(iter.GetPath().length == expected_path_length);
            ++i;
        }
    }
}

void GenerateAndTestTrie(size_t size) {
    if (!size) {
        return;
    }

    std::vector<BinaryTrie<size_t>> tries;
    std::vector<size_t> values(size);
    std::vector<std::vector<size_t>> tries_indices(size);
    std::vector<size_t> expected_iteration(size);
    std::vector<size_t> expected_path_codes(size);
    std::vector<size_t> expected_path_lengths(size);

    std::priority_queue<std::pair<size_t, size_t>, std::vector<std::pair<size_t, size_t>>, std::greater<>> q;

    for (size_t i = 0; i < size; ++i) {
        tries_indices[i] = {i};
        values[i] = gen_number();
        tries.emplace_back(values[i]);
        q.emplace(gen_number(), i);
    }

    while (q.size() > 1) {
        auto [prior1, idx1] = q.top();
        q.pop();
        auto [prior2, idx2] = q.top();
        q.pop();

        tries[idx1].Merge(std::move(tries[idx2]));
        q.emplace(prior1 + prior2, idx1);

        for (size_t i : tries_indices[idx1]) {
            expected_path_codes[i] <<= 1;
            ++expected_path_lengths[i];
        }

        for (size_t i : tries_indices[idx2]) {
            expected_path_codes[i] <<= 1;
            expected_path_codes[i] |= 1;
            ++expected_path_lengths[i];
        }

        for (size_t i : tries_indices[idx2]) {
            tries_indices[idx1].push_back(i);
        }
    }

    auto [_, final_idx] = q.top();

    for (size_t i = 0; i < expected_iteration.size(); ++i) {
        expected_iteration[i] = values[tries_indices[final_idx][i]];
    }

    ApplyPermutation(tries_indices[final_idx], expected_path_codes);
    ApplyPermutation(tries_indices[final_idx], expected_path_lengths);

    BinaryTrie<size_t> final_trie(std::move(tries[final_idx]));

    if (*std::max_element(expected_path_lengths.begin(), expected_path_lengths.end()) > 8 * sizeof(size_t)) {
        return;
    }

    CheckIteration(final_trie, expected_iteration);
    CheckIteratorPaths(final_trie, expected_path_codes, expected_path_lengths);
    CheckTraversing(final_trie, expected_iteration, expected_path_codes, expected_path_lengths);
}

TEST(BinaryTrie, BinaryTrieIteratorPathTest2) {
    GenerateAndTestTrie(100);
}

TEST(BinaryTrie, BinaryTrieIteratorPathTest3) {

    for (size_t i = 1; i < 1000; ++i) {
        GenerateAndTestTrie(i);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
