#include <iostream>
#include <queue>

#include "archiver/archiver.h"
#include "reader/file_reader.h"
#include "writer/file_writer.h"

enum class CommandType { kCompress, kDecompress, kHelp, kUnknownType };

struct CommandProperties {
    CommandType command_type = CommandType::kUnknownType;
    std::string archive_name;
    std::vector<std::string> files_to_compress;
    std::string output_directory;
};

void ProcessOutputOption(CommandProperties& properties, std::queue<std::string>& tokens) {
    tokens.pop();

    if (tokens.empty()) {
        std::cout << "Option -o was used without output_dir specified" << std::endl;
        exit(0);
    }

    properties.output_directory = tokens.front();
    tokens.pop();
}

CommandProperties ParseArguments(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Too little options" << std::endl;
        exit(0);
    }

    std::queue<std::string> tokens;

    for (int i = 1; i < argc; ++i) {
        tokens.push(argv[i]);
    }

    CommandProperties properties;

    while (!tokens.empty()) {
        if (tokens.front() == "-c") {
            properties.command_type = CommandType::kCompress;
            tokens.pop();

            if (tokens.empty()) {
                std::cout << "There's no archive name" << std::endl;
                exit(0);
            }

            properties.archive_name = tokens.front();
            tokens.pop();

            if (!tokens.empty() && tokens.front() == "-o") {
                ProcessOutputOption(properties, tokens);
            }

            if (tokens.empty()) {
                std::cout << "There's no files to compress" << std::endl;
                exit(0);
            }

            while (!tokens.empty()) {
                properties.files_to_compress.emplace_back(tokens.front());
                tokens.pop();
            }
        } else if (tokens.front() == "-d") {
            properties.command_type = CommandType::kDecompress;
            tokens.pop();

            if (tokens.empty()) {
                std::cout << "No archive name" << std::endl;
                exit(0);
            }

            properties.archive_name = tokens.front();
            tokens.pop();

            if (!tokens.empty() && tokens.front() == "-o") {
                ProcessOutputOption(properties, tokens);
            }
        } else if (tokens.front() == "-h") {
            properties.command_type = CommandType::kHelp;
            tokens.pop();
        }
    }

    return properties;
}

void PrintHelp() {
    std::cout << "Usage:" << std::endl << std::endl;
    std::cout << "archiver -c archive_name file1 [file2 ...] : "
              << "Compress files file1 [file2 ...] and save them in archive archive_name" << std::endl;
    std::cout << "archiver -d archive_name : "
              << "Decompress archive archive_name and save result in current directory" << std::endl;
    std::cout << "archiver -h"
              << " : "
              << "Print help message" << std::endl;
}

int main(int argc, char* argv[]) {
    CommandProperties properties = ParseArguments(argc, argv);
    Archiver archiver;

    if (properties.command_type == CommandType::kCompress) {
        std::vector<std::unique_ptr<ReaderInterface>> readers;

        try {
            for (const std::string& file : properties.files_to_compress) {
                readers.emplace_back(std::make_unique<FileReader>(file));
            }

            archiver.Compress(std::move(readers), std::make_unique<FileWriter>(properties.output_directory),
                              properties.archive_name);
        }

        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
            return 0;
        }

    } else if (properties.command_type == CommandType::kDecompress) {
        try {
            archiver.Decompress(std::make_unique<FileReader>(properties.archive_name),
                                std::make_unique<FileWriter>(properties.output_directory));
        }

        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
            return 0;
        }
    } else if (properties.command_type == CommandType::kHelp) {
        PrintHelp();
    } else if (properties.command_type == CommandType::kUnknownType) {
        std::cout << "Unknown option" << std::endl;
    }

    return 0;
}
