#include <fstream>
#include <optional>

class Reader {
public:
    Reader(const std::string& file_path);

    unsigned char read_next_byte();
    bool has_next_byte();

private:
    std::ifstream file_;
    size_t file_size_;
    size_t bytes_read_ = 0;
};
