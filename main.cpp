#include "archiver/archiver.h"

#include <iostream>
#include <string>
#include <memory>

#include "reader/reader.h"
#include "writer/writer.h"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Too little options" << std::endl;
        return 0;
    }

    Archiver archiver;

    if(std::string(argv[1]) == "-c") {
        if(argc < 4) {
            if(argc == 2) {
                std::cout << "There's no archive name" << std::endl;
            } else if(argc == 3) {
                std::cout << "There's no file names" << std::endl;
            }

            return 0;
        }

        std::string archive_name(argv[2]);
        std::vector<std::unique_ptr<ReaderInterface>> readers;

        for(int i = 3; i < argc; ++i) {
            readers.emplace_back(std::make_unique<Reader>(argv[i]));
        }

        archiver.Compress(std::move(readers), std::make_unique<Writer>(""), archive_name);

    } else if (std::string(argv[1]) == "-d") {
        if(argc < 3) {
            std::cout << "No archive name" << std::endl;
            return 0;
        }

        std::string archive_name(argv[2]);

        archiver.Decompress(std::make_unique<Reader>(archive_name), std::make_unique<Writer>(""));
    } else if(std::string(argv[1]) == "-h") {
        std::cout << "Usage:" << std::endl << std::endl;
        std::cout << "archiver -c archive_name file1 [file2 ...] : "
                  << "Compress files file1 [file2 ...] and save them in archive archive_name" << std::endl;
        std::cout << "archiver -d archive_name : "
                  << "Decompress archive archive_name and save result in current directory" << std::endl;
        std::cout << "archiver -h" << " : " << "Print help message" << std::endl;
    } else {
        std::cout << "Unknown option" << std::endl;
    }

    return 0;
}

