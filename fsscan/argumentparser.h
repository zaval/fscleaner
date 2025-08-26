//
// Created by Dmytrii Zavalnyi on 24.04.2025..
//

#ifndef ARGUMENTPARSER_H
#define ARGUMENTPARSER_H
#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <iostream>


struct ProgramOptions {
    std::filesystem::path start_path = std::filesystem::current_path();
    std::optional<std::string> database_path;
    bool show_help = false;
    bool verbose = false;
    bool ignore_mounts = false;
    // bool follow_symlinks = false;
    // bool in_memory_db = true;
    // size_t thread_count = 0;  // 0 means use system default
};



class ArgumentParser {
public:
    ArgumentParser(int argc, char* argv[]);


    ProgramOptions parse() const;

    static void print_help();

private:
    std::vector<std::string> args_;


};



#endif //ARGUMENTPARSER_H
