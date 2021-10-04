#pragma once
#include <string>

class WriterInterface {
public:
    virtual ~WriterInterface() {}

    virtual void OpenFile(const std::string& file_name) = 0;
    virtual void CloseFile() = 0;
    virtual void WriteByte(unsigned char byte) = 0;
    virtual void WriteBit(bool bit) = 0;
    virtual void Flush() = 0;
};