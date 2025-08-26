//
// Created by Dmytrii Zavalnyi on 23.04.2025..
//

#ifndef DATABASE_H
#define DATABASE_H
#include <cstdint>
#include <iostream>
#include <sqlite3.h>
#include <vector>


enum db_mode {
    INSERT,
    UPDATE,
};

struct FilesRow {
    int64_t rowid;
    std::string path;
    int64_t parent;
    int64_t size;
    bool is_folder;
    db_mode mode;
};

// inline thread_local std::vector<FilesRow> files_rows{};

class Database {
    sqlite3* db;
    // sqlite3_stmt* insert_stmt;

private:
    [[nodiscard]] std::vector<std::string> get_folders() const;
    [[nodiscard]] std::uintmax_t calculate_size(const std::string& path) const;

    // static std::atomic<int> current_rowid{1};
    // std::mutex db_mutex;
    // const int CACHE_SIZE = 10000;

    // std::vector<FilesRow> files_rows;



public:
    static std::atomic<int> current_rowid;
    Database();
    // void dump_cache();
    void insert_file(const uint64_t &id, const std::string& path, const std::uintmax_t &size, const int64_t &parent) const;
    // int64_t insert_folder(const uint64_t &id, const std::string& path, const int64_t &parent, const int64_t &size = 0) const;
    void insert_folder(const uint64_t &id, const std::string& path, const int64_t &parent, const int64_t &size = 0) const;
    void update_folder_size(const int64_t &rowid, const int64_t &size);
    void recalculate_folder_size(const int64_t &rowid) const;
    // int64_t insert_folder(const std::string& path, const int64_t &parent) const;
    void calculate_folders_size() const;
    void print_summary() const;
    void save_to_file(const std::string& filename) const;

    void begin_transaction() const;
    void commit_transaction() const;
    // void rollback_transaction() const;

    static int get_rowid();
    void batch_insert(const std::vector<FilesRow>& rows) const;

    ~Database();

};




#endif //DATABASE_H
