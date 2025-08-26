//
// Created by Dmytrii Zavalnyi on 24.04.2025..
//

#include "argumentparser.h"

ArgumentParser::ArgumentParser(int argc, char *argv[]): args_(argv + 1, argv + argc) {}

ProgramOptions ArgumentParser::parse() const {
    ProgramOptions options;

    for (size_t i = 0; i < args_.size(); ++i) {
        std::string_view arg = args_[i];

        if (arg == "-h" || arg == "--help") {
            options.show_help = true;
            return options;
        }
        if (arg == "-v" || arg == "--verbose") {
            options.verbose = true;
        }
        if (arg == "-i" || arg == "--ignore-mounts") {
            options.ignore_mounts = true;
        }
        // else if (arg == "-s" || arg == "--follow-symlinks") {
        //     options.follow_symlinks = true;
        // }
        // else if (arg == "-m" || arg == "--memory") {
        //     options.in_memory_db = true;
        // }
        else if (arg == "-d" || arg == "--database") {
            if (i + 1 < args_.size()) {
                options.database_path = args_[++i];
                // options.in_memory_db = false;
            } else {
                throw std::runtime_error("Missing database path after -d/--database");
            }
        }
        // else if (arg == "-j" || arg == "--threads") {
        //     if (i + 1 < args_.size()) {
        //         try {
        //             options.thread_count = std::stoul(args_[++i]);
        //         } catch (const std::exception& e) {
        //             throw std::runtime_error("Invalid thread count value");
        //         }
        //     } else {
        //         throw std::runtime_error("Missing thread count after -j/--threads");
        //     }
        // }
        else if (arg.starts_with("-")) {
            throw std::runtime_error("Unknown option: " + std::string(arg));
        }
        else {
            // Assume it's the start path
            options.start_path = arg;
        }
    }

    return options;
}

void ArgumentParser::print_help() {
    std::cout << "Usage: fsSize [options] [path]\n"
              << "\nOptions:\n"
              << "  -h, --help            Show this help message\n"
              << "  -v, --verbose         Enable verbose output\n"
              // << "  -s, --follow-symlinks Follow symbolic links\n"
              // << "  -m, --memory          Use in-memory database (default)\n"
              << "  -d, --database <path> Use persistent database at path\n"
              // << "  -j, --threads <num>   Number of threads to use (default: system)\n"
              << "\nArguments:\n"
              << "  path                  Starting directory (default: current)\n"
              << std::endl;
}

