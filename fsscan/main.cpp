#include <unordered_set>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/stat.h>

#include "argumentparser.h"
#include "database.h"
#include "fshelper.h"
#include "threadpool.h"

namespace fs = std::filesystem;

struct TreeEntry {
    int64_t rowid;
    std::string path;
    int64_t size;
    bool is_folder;
    TreeEntry *parent = nullptr;
    std::vector<std::unique_ptr<TreeEntry>> children = {};

    void add_child(std::unique_ptr<TreeEntry> child) {
        child->parent = this;
        const auto sz = child->size;
        children.emplace_back(std::move(child));

        size += sz;
        TreeEntry *p = parent;
        while (p) {
            p->size += sz;
            p = p->parent;
        }
    }
};

struct FSRecord {
    uint64_t rowid;
    std::filesystem::path path;
    uint64_t size;
    uint64_t parent;
    TreeEntry *parent_tree_entry = nullptr;
};

struct WalkResult {
    std::vector<FSRecord> folders;
    std::vector<FSRecord> files;
};



std::atomic<uint64_t> current_rowid = 1;
std::atomic<uint64_t> inserted_count = 1;

std::uintmax_t get_file_size_for_long_path(const std::string& path) {

    const int fd = open(path.c_str(),  O_NOFOLLOW);
    if (fd == -1) {
        throw std::system_error(errno, std::system_category(), "Failed to open file: " + path);
    }

    struct stat st;
    int result = fstat(fd, &st);
    close(fd);

    if (result == -1) {
        throw std::system_error(errno, std::system_category(), "Failed to get file stats: " + path);
    }

    return st.st_size;
}

const bool is_terminal = isatty(STDOUT_FILENO);

std::atomic<int> processed_files_atomic{0};
void print_progress() {
    // processed_files++;
    int current = processed_files_atomic.fetch_add(1);
    if (current % 100 == 0) {
        // if (true) {
        // std::stringstream ss;
        if (is_terminal) {
            std::cout << "\r\033[K";
            // ss << "\r\033[K";
        }
        // ss << "Processing " << current << " files";
        std::cout << "Processing " << current << " files";
        if (!is_terminal) {
            std::cout << std::endl;
        }
        std::cout.flush();
        // std::cout << ss.str() << std::flush;
    }
}

// WalkResult walk(const fs::path &path, const int64_t &parent, const std::unordered_set<std::string> &ignoreFiles)  {
WalkResult walk(const fs::path &path, TreeEntry *parent, const std::unordered_set<std::string> &ignoreFiles)  {
    WalkResult result;

    const auto parent_id = parent ? parent->rowid : 0;
    std::error_code ec;
    // if (fs::is_symlink(path, ec)) {
    //     return result; // Skip symbolic links entirely
    // }
    std::vector<fs::path> directories;
    std::vector<std::pair<fs::path, std::uintmax_t>> files;
    try {
        for (auto const& dir_entry : fs::directory_iterator{
            path,
            fs::directory_options::skip_permission_denied | fs::directory_options::follow_directory_symlink,
            ec
        }) {
            const auto file_status = dir_entry.status(ec);
            if (ec || fs::is_symlink(file_status)) {
                continue;
            }

            if (fs::is_symlink(dir_entry, ec)) {
                continue; // Skip symbolic links entirely
            }

            const auto& entry_path = dir_entry.path();
            if (ignoreFiles.contains(entry_path.string())) {
                continue;
            }

            if (fs::is_directory(file_status)) {

                result.folders.emplace_back(current_rowid.fetch_add(1), entry_path, 0, parent_id, parent);
                directories.push_back(entry_path);

            } else if (fs::is_regular_file(file_status)) {

                std::uintmax_t file_size;

                if (entry_path.string().length() >= PATH_MAX) {
                    file_size = get_file_size_for_long_path(entry_path.string());
                } else {
                    file_size = dir_entry.file_size(ec);
                    if (ec) {
                        file_size = fs::file_size(entry_path);
                    }
                }
                result.files.emplace_back(current_rowid.fetch_add(1), entry_path, file_size, parent_id, parent);
            }

        }
    } catch (std::filesystem::filesystem_error const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return result;
    } catch (std::system_error const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return result;
    }

    return result;

}

void save_to_db(const Database &db, TreeEntry *entry) {
    if (!entry->is_folder) {
        db.insert_file(entry->rowid, entry->path, entry->size, entry->parent ? entry->parent->rowid : 0);
        if (inserted_count.fetch_add(1) % 100 == 0) {
            db.commit_transaction();
            db.begin_transaction();
        }
    } else {
        db.insert_folder(entry->rowid, entry->path, entry->parent ? entry->parent->rowid : 0, entry->size);
        for (const auto& child : entry->children) {
            save_to_db(db, child.get());
            if (inserted_count.fetch_add(1) % 100 == 0) {
                db.commit_transaction();
                db.begin_transaction();
            }
        }
    }
}

int main(int argc, char* argv[]) {
    std::vector<std::string> mountFolders = FSHelper::getMountedFolders();

    ArgumentParser parser(argc, argv);
    ProgramOptions options;
    try {
        options = parser.parse();
    } catch (std::runtime_error const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        ArgumentParser::print_help();
        return 1;
    }

    if (options.show_help) {
        ArgumentParser::print_help();
        return 0;
    }

    if (options.verbose) {
        std::cout << "Starting path: " << options.start_path << std::endl
                  << "Database: " << options.database_path.value_or("filesystem_stats.db") << std::endl;
    }
    if (!options.ignore_mounts) {
        mountFolders.clear();
    }

    const std::filesystem::path start_path{options.start_path};
    const auto db = Database();
    auto pool = ThreadPool(std::thread::hardware_concurrency());

    const std::unordered_set mountSet(mountFolders.begin(), mountFolders.end());

    std::unique_ptr<TreeEntry> rootTreeEntry = std::make_unique<TreeEntry>(current_rowid.fetch_add(1), start_path.string(), 0, true, nullptr);
    std::vector records{rootTreeEntry.get()};

    const auto start = std::chrono::high_resolution_clock::now();

    while (!records.empty()) {
        std::vector<std::future<WalkResult>> futures;
        futures.reserve(records.size());

        for (auto row : records) {

            futures.emplace_back(
            pool.enqueue(
                    [&mountSet](const std::string& path_, TreeEntry *parent_) -> WalkResult {
                        return walk(path_, parent_, mountSet);
                    },
                    row->path, row
                )
            );
        }
        records.clear();


        for (auto& future : futures) {
            const auto [folders, files] = future.get();

            for (const auto&[rowid, path, size, parent, parent_tree_entry] : folders) {

                parent_tree_entry->add_child(
                    std::make_unique<TreeEntry>(rowid, path.string(), size, true, parent_tree_entry)
                );
                records.emplace_back(parent_tree_entry->children.back().get());
            }


            for (const auto&[rowid, path, size, parent, parent_tree_entry] : files) {
                parent_tree_entry->add_child(
                    std::make_unique<TreeEntry>(rowid, path.string(), size, false, parent_tree_entry)
                );
                print_progress();
            }
        }
    }

    std::cout << std::endl << "Saving data" << std::endl;
    db.begin_transaction();
    save_to_db(db, rootTreeEntry.get());
    db.commit_transaction();


    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Finished in " << duration.count() << " ms\n";

    // std::cout << "calculate folders sizes" << std::endl;
    // db.calculate_folders_size();
    // std::cout << "calculated" << std::endl;

    try {
        db.save_to_file(options.database_path.value_or("filesystem_stats.db"));
        // db.save_to_file("filesystem_stats.db");
        std::cout << "Database saved to filesystem_stats.db\n";
    } catch (const std::exception& e) {
        std::cerr << "Error saving database: " << e.what() << '\n';
    }

}
