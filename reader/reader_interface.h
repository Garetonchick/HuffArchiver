#pragma once
#include <string>

class ReaderInterface {
public:
    virtual ~ReaderInterface() = default;

    virtual bool HasNextByte() const = 0;
    virtual bool HasNextBit() const = 0;
    virtual const std::string& GetFileName() const = 0;

    virtual unsigned char ReadNextByte() = 0;
    virtual bool ReadNextBit() = 0;
    virtual void Reset() = 0;
};
