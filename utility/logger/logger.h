#include <iostream>

class Logger {
public:
    explicit Logger(std::ostream& output = std::cout);

    void SetPrecision(size_t precision);

    void Log(const std::string& message);
    void Log(double number);
    void LogLn(const std::string& message = "");
    void LogPercentage(double percentage);

private:
    std::ostream& output_;
};