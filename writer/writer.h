#pragma once
#include <string>
#include <fstream>

class Writer {
public:
    explicit Writer(const std::string& file_path);

    void WriteByte(unsigned char byte);
    void CloseFile();

private:
    std::ofstream file_;
};