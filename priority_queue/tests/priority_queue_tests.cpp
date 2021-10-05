#include "../priority_queue.h"
#include <gtest/gtest.h>

#include <queue>
#include <chrono>
#include <random>

TEST(PriorityQueue, FullTest) {
    std::mt19937 random_gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    std::priority_queue<size_t> tester_queue;
    PriorityQueue<size_t, std::greater<>> queue_to_test;

    for (size_t max_number = 10; max_number <= 1'000'000; max_number *= 10) {
        auto gen_number = std::bind(std::uniform_int_distribution<size_t>(0, max_number), random_gen);

        const size_t n = 1'000'000;

        for (size_t i = 0; i < 4; ++i) {
            for (size_t i = 0; i < n; ++i) {
                ASSERT_EQ(queue_to_test.GetSize(), tester_queue.size());
                ASSERT_EQ(queue_to_test.IsEmpty(), tester_queue.empty());

                size_t number = gen_number();

                tester_queue.push(number);
                queue_to_test.Push(number);

                ASSERT_EQ(queue_to_test.Top(), tester_queue.top());
            }

            for (size_t i = 0; i < n; ++i) {
                ASSERT_EQ(queue_to_test.GetSize(), tester_queue.size());
                ASSERT_EQ(queue_to_test.IsEmpty(), tester_queue.empty());
                ASSERT_EQ(queue_to_test.Top(), tester_queue.top());

                tester_queue.pop();
                queue_to_test.Pop();
            }

            ASSERT_EQ(queue_to_test.GetSize(), tester_queue.size());
            ASSERT_EQ(queue_to_test.IsEmpty(), tester_queue.empty());
        }
    }
}

TEST(PriorityQueue, UsageTest) {
    std::mt19937 random_gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    auto gen_number = std::bind(std::uniform_int_distribution<size_t>(0, 1'000'000), random_gen);

    std::priority_queue<size_t, std::vector<size_t>, std::greater<>> tester_queue;
    PriorityQueue<size_t> queue_to_test;

    for (size_t i = 0; i < 1'000'000; ++i) {
        size_t number = gen_number();

        tester_queue.push(number);
        queue_to_test.Push(number);
    }

    while (tester_queue.size() > 1) {
        size_t expected1 = tester_queue.top();

        ASSERT_EQ(queue_to_test.Top(), expected1);

        tester_queue.pop();
        queue_to_test.Pop();

        size_t expected2 = tester_queue.top();

        ASSERT_EQ(queue_to_test.Top(), expected2);

        tester_queue.pop();
        queue_to_test.Pop();

        tester_queue.push(expected1 + expected2);
        queue_to_test.Push(expected1 + expected2);
    }

    ASSERT_EQ(queue_to_test.Top(), tester_queue.top());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
