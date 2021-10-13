# What is this project?

This is simple in use archiver, which uses [Huffman coding](https://en.wikipedia.org/wiki/Huffman_coding) 
for compression. 

# Commands
* `archiver -c archive_name file1 [file2 ...]` - compress files 
`file1, file2, ...` and save the result in file `archive_name`.
* `archiver -d archive_name` - decompress files from archive `archive_name`
and put them in current directory.
* `archiver -h` - show help message.
* `-o` - this option allows you to specify directory for output files. 
For example `archiver -c archive_name -o output_dir file1 [file2 ...]`
will compress files `file1, file2, ...` and save the result in 
`output_dir/archive_name`.

# Benchmarks

### Compression percentages

Formula: `compressed_file_size / original_file_size * 100%`

* Text: 56.74%
* Image: 99.31%
* Video: 100.04%

### Performance

* Compression: 7 MB/s
* Decompression: 5.9 MB/s

# Building from source

To build this project you will need CMake of version 2.8
or higher and Clang-11 or higher.

## Linux

Run following commands from source root directory:

    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release .. && make

In the build directory you will find "MAIN" file, which
is the archiver's executable.

#License

This project is licensed under the terms of the MIT license.
