#include "logger.h"

#include <iomanip>

Logger::Logger(std::ostream& output) : output_(output) {
    output_ << std::fixed;
}

void Logger::SetPrecision(size_t precision) {
    output_ << std::setprecision(int(precision));
}

void Logger::Log(const std::string& message) {
    output_ << message;
}

void Logger::Log(double number) {
    output_ << number;
}

void Logger::LogLn(const std::string& message) {
    output_ << message << std::endl;
}

void Logger::LogPercentage(double percentage) {
    output_ << percentage << "%";
}