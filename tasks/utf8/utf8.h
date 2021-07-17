#pragma once

#include <cstddef>
#include <vector>
#include <string>


// CountUtf8Symbols returns the number of utf8 symbols in string.
size_t CountUtf8Symbols(const std::string& str);

// EncodeUtf8 encodes given @codepoints into string @str.
void EncodeUtf8(const std::vector<uint32_t>& codepoints, std::string& str);

// DecodeUtf8 decodes given string @str into @codepoints.
void DecodeUtf8(const std::string& str, std::vector<uint32_t>& codepoints);

