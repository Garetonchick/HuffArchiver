#include <fstream>
#include <optional>

class Reader {
public:
  Reader(const std::string& file_path);

  unsigned char read_next_byte();
  bool has_next_byte();

private:
  std::ifstream file_;
};
