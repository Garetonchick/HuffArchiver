#include "binary_trie/binary_trie.h"
#include <gtest/gtest.h>

#include <vector>
#include <random>
#include <functional>
#include <chrono>
#include <bitset>
#include <queue>

std::mt19937 random_gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
auto gen_number = std::bind(std::uniform_int_distribution<size_t>(0, 100), random_gen);

template <class T>
void CheckIteration(const BinaryTrie<T>& trie, const std::vector<T>& expected_iteration) {
    {
        size_t i = 0;

        for (auto it : trie) {
            ASSERT_TRUE(i < expected_iteration.size());
            ASSERT_EQ(it, expected_iteration[i]);
            ++i;
        }
    }

    {
        size_t i = 0;

        for (auto it = trie.begin(); it != trie.end(); ++it) {
            ASSERT_TRUE(i < expected_iteration.size());
            ASSERT_EQ(*it, expected_iteration[i]);
            ++i;
        }
    }
}

template <class T>
void CheckIteratorPaths(const BinaryTrie<T>& trie,
                        const std::vector<typename BinaryTrie<T>::BinaryPath>& expected_paths) {
    size_t i = 0;

    for (auto iter = trie.begin(); iter != trie.end(); ++iter) {
        ASSERT_EQ(iter.GetPath().code, expected_paths[i].code);
        ASSERT_EQ(iter.GetPath().length, expected_paths[i].length);
        ++i;
    }
}

template <class T>
void CheckTraversing(const BinaryTrie<T>& trie, const std::vector<T>& expected_traversal,
                     const std::vector<typename BinaryTrie<T>::BinaryPath>& expected_paths) {

    for (size_t i = 0; i < expected_traversal.size(); ++i) {
        auto traverser = trie.GetRootTraverser();

        for (size_t j = 0; j < expected_paths[i].length; ++j) {
            if ((expected_paths[i].code >> j) & 1) {
                ASSERT_TRUE(traverser.CanGoRight());
                traverser.GoRight();
            } else {
                ASSERT_TRUE(traverser.CanGoLeft());
                traverser.GoLeft();
            }
        }

        ASSERT_EQ((*traverser).code, expected_paths[i].code);
        ASSERT_EQ((*traverser).length, expected_paths[i].length);
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

void GenerateAndTestTrie(size_t size) {
    if (!size) {
        return;
    }

    std::vector<BinaryTrie<size_t>> tries;
    std::vector<size_t> values(size);
    std::vector<std::vector<size_t>> tries_indices(size);
    std::vector<size_t> expected_iteration(size);
    std::vector<BinaryTrie<size_t>::BinaryPath> expected_paths(size);

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
            expected_paths[i].code <<= 1;
            ++expected_paths[i].length;
        }

        for (size_t i : tries_indices[idx2]) {
            expected_paths[i].code <<= 1;
            expected_paths[i].code |= 1;
            ++expected_paths[i].length;
        }

        for (size_t i : tries_indices[idx2]) {
            tries_indices[idx1].push_back(i);
        }
    }

    auto [_, final_idx] = q.top();

    for (size_t i = 0; i < expected_iteration.size(); ++i) {
        expected_iteration[i] = values[tries_indices[final_idx][i]];
    }

    ApplyPermutation(tries_indices[final_idx], expected_paths);

    BinaryTrie<size_t> final_trie(std::move(tries[final_idx]));

    for(size_t i = 0 ; i < expected_paths.size(); ++i) {
        if(expected_paths[i].code > 8 * sizeof(size_t)) {
            return;
        }
    }

    CheckIteration(final_trie, expected_iteration);
    CheckIteratorPaths(final_trie, expected_paths);
    CheckTraversing(final_trie, expected_iteration, expected_paths);
}

TEST(BinaryTrie, SmallTest) {
    std::vector<char> expected_traversal = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k'};
    BinaryTrie<char> trie('a');

    for (size_t i = 1; i < expected_traversal.size(); ++i) {
        trie.Merge(BinaryTrie<char>(expected_traversal[i]));
    }

    CheckIteration(trie, expected_traversal);
}

TEST(BinaryTrie, RandomTest) {
    std::vector<size_t> expected_iteration(20);

    expected_iteration[0] = gen_number();
    BinaryTrie trie(expected_iteration[0]);

    for (size_t i = 1; i < expected_iteration.size(); ++i) {
        expected_iteration[i] = gen_number();
        trie.Merge(BinaryTrie<size_t>(expected_iteration[i]));
    }

    CheckIteration(trie, expected_iteration);
}

TEST(BinaryTrie, BinaryTrieManualTest) {
    std::vector<size_t> expected_iteration = {0, 1, 2, 3};

    BinaryTrie trie(expected_iteration[0]);
    trie.Merge(BinaryTrie<size_t>(expected_iteration[1]));
    BinaryTrie trie2(expected_iteration[2]);
    trie2.Merge(BinaryTrie<size_t>(expected_iteration[3]));
    trie.Merge(std::move(trie2));

    CheckIteration(trie, expected_iteration);
}

TEST(BinaryTrie, BigTest) {
    for (size_t i = 1; i < 1000; ++i) {
        GenerateAndTestTrie(i);
    }
}

TEST(BinaryTrie, InsertTest) {
    BinaryTrie<size_t> trie;
    std::vector<size_t> expected_iteration;
    std::vector<BinaryTrie<size_t>::BinaryPath> expected_paths;
    std::set<BinaryTrie<size_t>::BinaryPath> used_paths;

    for(size_t i = 0; i < 1'000'000; ++i) {
        size_t path_length = std::uniform_int_distribution<size_t>(15, 25)(random_gen);
        BinaryTrie<size_t>::BinaryPath path{.code = gen_number(), path_length};

        if(used_paths.find(path) != used_paths.end()) {
            used_paths.insert(path);
            expected_paths.push_back(path);
            expected_iteration.push_back(gen_number());
            trie.Insert(expected_iteration.back(), path);
        }
    }

    std::vector<size_t> order(expected_iteration.size());

    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&expected_paths](size_t a, size_t b) {
        return expected_paths[a] < expected_paths[b];
    });

    ApplyPermutation(order, expected_iteration);
    ApplyPermutation(order, expected_paths);

    CheckIteration(trie, expected_iteration);
    CheckIteratorPaths(trie, expected_paths);
    CheckTraversing(trie, expected_iteration, expected_paths);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
