#include "archiver/archiver.h"

#include <iostream>

#include "reader/reader.h"
#include "writer/writer.h"

enum class CommandType { kCompress, kDecompress, kHelp };

struct CommandProperties {
    CommandType command_type = CommandType::kHelp;
    std::string archive_name;
    std::vector<std::string> files_to_compress;
};

CommandProperties ParseArguments(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Too little options" << std::endl;
        exit(0);
    }

    CommandProperties properties;

    if (std::string(argv[1]) == "-c") {
        if (argc < 4) {
            if (argc == 2) {
                std::cout << "There's no archive name" << std::endl;
            } else if (argc == 3) {
                std::cout << "There's no files to compress" << std::endl;
            }

            exit(0);
        }

        properties.command_type = CommandType::kCompress;
        properties.archive_name = argv[2];

        for (int i = 3; i < argc; ++i) {
            properties.files_to_compress.emplace_back(argv[i]);
        }
    } else if (std::string(argv[1]) == "-d") {
        if (argc < 3) {
            std::cout << "No archive name" << std::endl;
            exit(0);
        }

        properties.command_type = CommandType::kDecompress;
        properties.archive_name = argv[2];
    } else if (std::string(argv[1]) == "-h") {
        properties.command_type = CommandType::kHelp;
    } else {
        std::cout << "Unknown option" << std::endl;
        exit(0);
    }

    return properties;
}

int main(int argc, char* argv[]) {
    CommandProperties properties = ParseArguments(argc, argv);
    Archiver archiver;

    if (properties.command_type == CommandType::kCompress) {
        std::vector<std::unique_ptr<ReaderInterface>> readers;


        try {
        for (const std::string& file : properties.files_to_compress) {
            readers.emplace_back(std::make_unique<Reader>(file));
        }

            archiver.Compress(std::move(readers), std::make_unique<Writer>(""), properties.archive_name);
        }

        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
            return 0;
        }

    } else if (properties.command_type == CommandType::kDecompress) {
        try {
            archiver.Decompress(std::make_unique<Reader>(properties.archive_name), std::make_unique<Writer>(""));
        }

        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
            return 0;
        }
    } else if (properties.command_type == CommandType::kHelp) {
        std::cout << "Usage:" << std::endl << std::endl;
        std::cout << "archiver -c archive_name file1 [file2 ...] : "
                  << "Compress files file1 [file2 ...] and save them in archive archive_name" << std::endl;
        std::cout << "archiver -d archive_name : "
                  << "Decompress archive archive_name and save result in current directory" << std::endl;
        std::cout << "archiver -h"
                  << " : "
                  << "Print help message" << std::endl;
    }

    return 0;
}
