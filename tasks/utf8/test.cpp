#include <catch.hpp>
#include <utf8.h>

#include <cstddef>

struct TestData {
    std::string String;
    std::vector<uint32_t> Codepoints;
};

std::vector<TestData> TESTS = {
    {"Hello!", {72, 101, 108, 108, 111, 33}},
    {"Привет, мир!", {1055, 1088, 1080, 1074, 1077, 1090, 44, 32, 1084, 1080, 1088, 33}},
};

TEST_CASE("CountUtf8Symbols") {
    for (const auto& test : TESTS) {
        REQUIRE(CountUtf8Symbols(test.String) == test.Codepoints.size());
    }
}

TEST_CASE("EncodeUtf8") {
    for (const auto& test : TESTS) {
        std::string encodedString;
        EncodeUtf8(test.Codepoints, encodedString);
        REQUIRE(encodedString == test.String);
    }

}

TEST_CASE("DecodeUtf8") {
    for (const auto& test : TESTS) {
        std::vector<uint32_t> decodedCodepoints;
        DecodeUtf8(test.String, decodedCodepoints);
        REQUIRE(test.Codepoints == decodedCodepoints);
    }
}
