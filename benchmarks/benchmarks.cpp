#include <filesystem>

#include "archiver/archiver.h"
#include "reader/file_reader.h"
#include "writer/file_writer.h"
#include "utility/timer/timer.h"
#include "utility/logger/logger.h"

namespace {
int64_t ALL_FILES_SIZE_SUM = 0;
int64_t ALL_FILES_COMPRESSION_TIME_SUM = 0;
int64_t ALL_FILES_DECOMPRESSION_TIME_SUM = 0;

int64_t GetFileSize(const std::string& file_path) {
    std::ifstream file(file_path, std::ios_base::binary | std::ios_base::ate);

    return file.tellg();
}

double CalculateCompressionPercentage(const std::string& directory) {
    Archiver archiver;
    std::vector<std::unique_ptr<ReaderInterface>> readers;
    int64_t file_sizes_sum = 0;

    for (const auto& file : std::filesystem::directory_iterator(directory)) {
        if (!std::filesystem::is_directory(file.path())) {
            readers.push_back(std::make_unique<FileReader>(file.path()));
            file_sizes_sum += GetFileSize(file.path());
        }
    }

    ALL_FILES_SIZE_SUM += file_sizes_sum;

    Timer timer;

    archiver.Compress(std::move(readers), std::make_unique<FileWriter>(directory + "/compressed"), "archive");

    ALL_FILES_COMPRESSION_TIME_SUM += timer.GetMilliseconds();

    timer.Reset();

    archiver.Decompress(std::make_unique<FileReader>(directory + "/compressed/archive"),
                        std::make_unique<FileWriter>(directory + "/decompressed"));

    ALL_FILES_DECOMPRESSION_TIME_SUM += timer.GetMilliseconds();

    return double(GetFileSize(directory + "/compressed/archive")) / double(file_sizes_sum) * 100.0;
}
}  // namespace

int main() {
    Logger logger;

    logger.SetPrecision(2);

    logger.LogLn("[Text benchmarks]");
    logger.Log("Compression percentage: ");
    logger.LogPercentage(CalculateCompressionPercentage("mock/texts"));
    logger.LogLn();
    logger.LogLn("-------------------------------");

    logger.LogLn("[Image benchmarks]");
    logger.Log("Compression percentage: ");
    logger.LogPercentage(CalculateCompressionPercentage("mock/images"));
    logger.LogLn();
    logger.LogLn("-------------------------------");

    logger.LogLn("[Video benchmarks]");
    logger.Log("Compression percentage: ");
    logger.LogPercentage(CalculateCompressionPercentage("mock/video"));
    logger.LogLn();
    logger.LogLn("-------------------------------");

    double compression_speed =
        double(ALL_FILES_SIZE_SUM) / double(ALL_FILES_COMPRESSION_TIME_SUM) * double(1000) / double(int64_t(1) << 20);
    double decompression_speed =
        double(ALL_FILES_SIZE_SUM) / double(ALL_FILES_DECOMPRESSION_TIME_SUM) * double(1000) / double(int64_t(1) << 20);

    logger.SetPrecision(2);

    logger.LogLn("[Performance benchmarks]");
    logger.Log("Compression speed: ");
    logger.Log(compression_speed);
    logger.LogLn("MB/s");
    logger.Log("Decompression speed: ");
    logger.Log(decompression_speed);
    logger.LogLn("MB/s");
}