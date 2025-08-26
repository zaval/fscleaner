//
// Created by Dmytrii Zavalnyi on 23.04.2025..
//

#include "database.h"
#include <sstream>
#include <thread>

std::atomic<int> Database::current_rowid = 1;

std::vector<std::string> Database::get_folders() const {
    const auto folders_sql =
        "SELECT path FROM files WHERE is_folder = 1 AND size = 0";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, folders_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    std::vector<std::string> folders;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const auto folder = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        folders.emplace_back(folder);
    }
    sqlite3_finalize(stmt);
    return folders;
}

std::uintmax_t Database::calculate_size(const std::string &path) const {
    const auto folders_sql =
        "SELECT SUM(size) as size FROM files WHERE is_folder = 0 AND path like ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, folders_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    std::stringstream ss;
    ss << path << "%";

    sqlite3_bind_text(stmt, 1, ss.str().c_str(), -1, SQLITE_STATIC);

    int64_t size = 0;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        size = sqlite3_column_int64(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return size;
}

// void Database::dump_cache() {
//     std::lock_guard lock(db_mutex);
//     for (const auto& row : files_rows) {
//         // std::cout << row.mode << " " << row.rowid << " " << row.path << " " << row.size << " " << row.is_folder << " "
//         //         << row.parent << '\n';
//
//
//         if (row.mode == INSERT) {
//             sqlite3_stmt* insert_stmt;
//             const auto insert_sql =
//             "INSERT INTO files (path, is_folder, size, parent) VALUES (?, ?, ?, ?)";
//             if (sqlite3_prepare_v2(db, insert_sql, -1, &insert_stmt, nullptr) != SQLITE_OK) {
//                 throw std::runtime_error(sqlite3_errmsg(db));
//             }
//
//             sqlite3_reset(insert_stmt);
//             sqlite3_bind_text(insert_stmt, 1, row.path.c_str(), -1, SQLITE_STATIC);
//             // sqlite3_bind_int64(insert_stmt, 2, 1);
//             sqlite3_bind_int64(insert_stmt, 2, row.is_folder ? 1 : 0);
//             sqlite3_bind_int64(insert_stmt, 3, row.size);
//             sqlite3_bind_int64(insert_stmt, 4, row.parent);
//
//             if (const auto res = sqlite3_step(insert_stmt); res != SQLITE_DONE) {
//                 std::cerr << "Failed to insert file: " << row.path << " " << res << '\n';
//                 // throw std::runtime_error(sqlite3_errmsg(db));
//             }
//             const auto rowid = sqlite3_last_insert_rowid(db);
//             sqlite3_finalize(insert_stmt);
//         }
//         else {
//             sqlite3_stmt* update_stmt;
//             const auto update_sql = "UPDATE files SET size = ? WHERE id = ?";
//             if (sqlite3_prepare_v2(db, update_sql, -1, &update_stmt, nullptr) != SQLITE_OK) {
//                 throw std::runtime_error(sqlite3_errmsg(db));
//             }
//             sqlite3_reset(update_stmt);
//             sqlite3_bind_int64(update_stmt, 1, row.size);
//             sqlite3_bind_int64(update_stmt, 2, row.rowid);
//             if (const auto res = sqlite3_step(update_stmt); res != SQLITE_DONE) {
//                 std::cerr << "Failed to update folder size: " << res << '\n';
//                 // throw std::runtime_error(sqlite3_errmsg(db));
//             }
//             sqlite3_finalize(update_stmt);
//             }
//
//     }
//     files_rows.clear();
// }

Database::Database(): db(nullptr) {

    if (sqlite3_open(":memory:", &db) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    // Create table
    const auto create_table_sql = R"(
CREATE TABLE files (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    path TEXT NOT NULL UNIQUE,
    size INTEGER DEFAULT 0,
    is_folder INTEGER DEFAULT 0,
    parent INTEGER
);
)";
        // "CREATE TABLE files (path TEXT PRIMARY KEY, size INTEGER, is_folder INTEGER DEFAULT 0)";
    char* err_msg = nullptr;
    if (sqlite3_exec(db, create_table_sql, nullptr, nullptr, &err_msg) != SQLITE_OK) {
        const std::string error = err_msg;
        sqlite3_free(err_msg);
        throw std::runtime_error("Failed to create table: " + error);
    }
}

void Database::insert_file(const uint64_t &id, const std::string &path, const std::uintmax_t &size, const int64_t &parent) const {

    // auto rowid = current_rowid.fetch_add(1);
    // std::lock_guard lock(db_mutex);
    // files_rows.emplace_back(rowid, path, parent, size, false, INSERT);
    // // if (files_rows.size() > CACHE_SIZE) {
    // //     dump_cache();
    // // }
    // return;

    sqlite3_stmt* insert_stmt;
    const auto insert_sql =
        "INSERT INTO files (id, path, size, parent) VALUES (?, ?, ?, ?)";
    if (sqlite3_prepare_v2(db, insert_sql, -1, &insert_stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    sqlite3_reset(insert_stmt);
    sqlite3_bind_int64(insert_stmt, 1, id);
    sqlite3_bind_text(insert_stmt, 2, path.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int64(insert_stmt, 3, static_cast<long>(size));
    sqlite3_bind_int64(insert_stmt, 4, parent);

    if (const auto res = sqlite3_step(insert_stmt); res != SQLITE_DONE) {
        std::cerr << "Failed to insert file: " << path << " " << res << '\n';
        // throw std::runtime_error(sqlite3_errmsg(db));
    }
    sqlite3_finalize(insert_stmt);
}

// int64_t Database::insert_folder(const uint64_t &id, const std::string &path, const int64_t &parent, const int64_t &size) const {
void Database::insert_folder(const uint64_t &id, const std::string &path, const int64_t &parent, const int64_t &size) const {

    // auto rowid = current_rowid.fetch_add(1);
    // std::lock_guard lock(db_mutex);
    // files_rows.emplace_back(rowid, path, parent, size, true, INSERT);
    // // if (files_rows.size() > CACHE_SIZE) {
    // //     dump_cache();
    // // }
    // return rowid;

    sqlite3_stmt* insert_stmt;
    const auto insert_sql =
        "INSERT INTO files (id, path, is_folder, size, parent) VALUES (?, ?, 1, ?, ?)";
    if (sqlite3_prepare_v2(db, insert_sql, -1, &insert_stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    sqlite3_reset(insert_stmt);
    sqlite3_bind_int64(insert_stmt, 1, id);
    sqlite3_bind_text(insert_stmt, 2, path.c_str(), -1, SQLITE_STATIC);
    // sqlite3_bind_int64(insert_stmt, 2, 1);
    sqlite3_bind_int64(insert_stmt, 3, size);
    sqlite3_bind_int64(insert_stmt, 4, parent);

    if (const auto res = sqlite3_step(insert_stmt); res != SQLITE_DONE) {
        std::cerr << "Failed to insert file: " << path << " " << res << '\n';
        // throw std::runtime_error(sqlite3_errmsg(db));
    }
    // const auto rowid = sqlite3_last_insert_rowid(db);
    // sqlite3_finalize(insert_stmt);
    // return rowid;
}

void Database::update_folder_size(const int64_t &rowid, const int64_t &size) {

    // std::lock_guard lock(db_mutex);
    // files_rows.emplace_back(rowid, "", 0, size, true, UPDATE);
    // // if (files_rows.size() > CACHE_SIZE) {
    // //     dump_cache();
    // // }
    // return;

    sqlite3_stmt* update_stmt;
    const auto update_sql =
        "UPDATE files SET size = ? WHERE id = ?";
    if (sqlite3_prepare_v2(db, update_sql, -1, &update_stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }
    sqlite3_reset(update_stmt);
    sqlite3_bind_int64(update_stmt, 1, size);
    sqlite3_bind_int64(update_stmt, 2, rowid);
    if (const auto res = sqlite3_step(update_stmt); res != SQLITE_DONE) {
        std::cerr << "Failed to update folder size: " << res << '\n';
        // throw std::runtime_error(sqlite3_errmsg(db));
    }
    sqlite3_finalize(update_stmt);
}

void Database::recalculate_folder_size(const int64_t &rowid) const {
    sqlite3_stmt* update_stmt;
    const auto update_sql =
        "update files set size = (select sum(size) from files where parent = ?) where id = ?;";
    if (sqlite3_prepare_v2(db, update_sql, -1, &update_stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }
    sqlite3_reset(update_stmt);
    sqlite3_bind_int64(update_stmt, 1, rowid);
    sqlite3_bind_int64(update_stmt, 2, rowid);
    if (const auto res = sqlite3_step(update_stmt); res != SQLITE_DONE) {
        std::cerr << "Failed to update folder size: " << res << '\n';
        // throw std::runtime_error(sqlite3_errmsg(db));
    }
    sqlite3_finalize(update_stmt);
}

// int64_t Database::insert_folder(const std::string &path, const int64_t &parent) const {
//     return insert_folder(path, parent, 0);
// }

void Database::calculate_folders_size() const {
//     const auto sql = R"(
// WITH RECURSIVE folders AS (
//     SELECT path
//     FROM files
//     WHERE is_folder = 1 AND size = 0
// )
// UPDATE files
// SET size = (
//     SELECT SUM(f.size)
//     FROM files f
//     WHERE f.is_folder = 0
//       AND f.path LIKE (files.path || '/%')
// )
// WHERE path IN (SELECT path FROM folders);
// )";
    const auto sql = R"(
WITH folder_sizes AS (
    SELECT
        f1.id,
        f1.path,
        SUM(f2.size) AS total_size
    FROM files AS f1
             JOIN files AS f2 ON f2.path LIKE f1.path || '/%'
    WHERE
        f1.is_folder = 1 AND f1.size = 0 AND f2.is_folder = 0
    GROUP BY
        f1.id
)
UPDATE files
SET size = (SELECT total_size FROM folder_sizes WHERE folder_sizes.id = files.id)
WHERE
    EXISTS (SELECT 1 FROM folder_sizes WHERE folder_sizes.id = files.id);
)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    if (const auto res = sqlite3_step(stmt); res != SQLITE_DONE) {
        std::cerr << "Failed to calculate folders size" << '\n';
        // throw std::runtime_error(sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);

    // const auto folders = get_folders();
    // for (const auto& folder : folders) {
    //     const auto size = calculate_size(folder);
    //     insert_folder(folder, size);
    // }
}

void Database::print_summary() const {
    const auto summary_sql =
        "SELECT COUNT(*) as file_count, SUM(size) as total_size FROM files";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, summary_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const int64_t count = sqlite3_column_int64(stmt, 0);
        const int64_t total = sqlite3_column_int64(stmt, 1);
        std::cout << "Total files: " << count << "\n";
        std::cout << "Total size: " << total << " bytes\n";
    }

    sqlite3_finalize(stmt);
}

void Database::save_to_file(const std::string &filename) const {
    sqlite3* file_db = nullptr;
    sqlite3_backup* backup = nullptr;

    try {
        // Open/create the target database file
        if (sqlite3_open(filename.c_str(), &file_db) != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(file_db));
        }

        // Initialize the backup
        backup = sqlite3_backup_init(file_db, "main", db, "main");
        if (!backup) {
            throw std::runtime_error(sqlite3_errmsg(file_db));
        }

        // Perform the backup
        if (sqlite3_backup_step(backup, -1) != SQLITE_DONE) {
            throw std::runtime_error(sqlite3_errmsg(file_db));
        }

        // Finish and clean up
        sqlite3_backup_finish(backup);

        if (sqlite3_close(file_db) != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(file_db));
        }
    } catch (...) {
        if (backup) sqlite3_backup_finish(backup);
        if (file_db) sqlite3_close(file_db);
        throw;
    }

}

void Database::begin_transaction() const {
    char* zErrMsg = 0;
    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &zErrMsg);
    if (zErrMsg != nullptr)
        std::cerr << zErrMsg << std::endl;
}

void Database::commit_transaction() const {
    char* zErrMsg = 0;
    sqlite3_exec(db, "COMMIT;", NULL, NULL, &zErrMsg);
    if (zErrMsg != nullptr)
        std::cerr << zErrMsg << std::endl;
}

int Database::get_rowid() {
    // const auto rowid = current_rowid.fetch_add(1);

    return current_rowid.fetch_add(1);
}

void Database::batch_insert(const std::vector<FilesRow> &rows) const {
     for (const auto&[rowid, path, parent, size, is_folder, mode] : rows) {
         // std::cout << row.mode << " " << row.rowid << " " << row.path << " " << row.size << " " << row.is_folder << " "
         //         << row.parent << '\n';


         if (mode == INSERT) {
             sqlite3_stmt* insert_stmt;
             const auto insert_sql =
             "INSERT INTO files (id, path, is_folder, size, parent) VALUES (?, ?, ?, ?, ?)";
             if (sqlite3_prepare_v2(db, insert_sql, -1, &insert_stmt, nullptr) != SQLITE_OK) {
                 throw std::runtime_error(sqlite3_errmsg(db));
             }

             sqlite3_reset(insert_stmt);
             sqlite3_bind_int64(insert_stmt, 1, rowid);
             sqlite3_bind_text(insert_stmt, 2, path.c_str(), -1, SQLITE_STATIC);
             // sqlite3_bind_int64(insert_stmt, 2, 1);
             sqlite3_bind_int64(insert_stmt, 3, is_folder ? 1 : 0);
             sqlite3_bind_int64(insert_stmt, 4, size);
             sqlite3_bind_int64(insert_stmt, 5, parent);

             if (const auto res = sqlite3_step(insert_stmt); res != SQLITE_DONE) {
                 std::cerr << "Failed to insert file: " << path << " " << res << '\n';
                 // throw std::runtime_error(sqlite3_errmsg(db));
             }
             // const auto rowid = sqlite3_last_insert_rowid(db);
             sqlite3_finalize(insert_stmt);
         }
         else {
             sqlite3_stmt* update_stmt;
             const auto update_sql = "UPDATE files SET size = ? WHERE id = ?";
             if (sqlite3_prepare_v2(db, update_sql, -1, &update_stmt, nullptr) != SQLITE_OK) {
                 throw std::runtime_error(sqlite3_errmsg(db));
             }
             sqlite3_reset(update_stmt);
             sqlite3_bind_int64(update_stmt, 1, size);
             sqlite3_bind_int64(update_stmt, 2, rowid);
             if (const auto res = sqlite3_step(update_stmt); res != SQLITE_DONE) {
                 std::cerr << "Failed to update folder size: " << res << '\n';
                 // throw std::runtime_error(sqlite3_errmsg(db));
             }
             sqlite3_finalize(update_stmt);
             }

     }
}

Database::~Database() {
    if (db) {
        // dump_cache();
        sqlite3_close(db);
    }

}
