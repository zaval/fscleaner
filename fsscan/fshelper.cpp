//
// Created by Dmytrii Zavalnyi on 12.08.2025..
//

#include "fshelper.h"

#include <filesystem>
#include <fstab.h>
#include <iostream>
#include <unordered_set>
#include <sys/mount.h>



std::vector<std::string> FSHelper::getMountedFolders() {

    std::unordered_set<std::string> unique_volumes{};
    std::vector<std::string> mounted_folders;

    // std::vector<std::string> folders;
    struct fstab* fs;

    while ((fs = getfsent()) != nullptr) {
        const auto file = fs->fs_file;
        if (strcmp("/", file) != 0) {
            unique_volumes.insert(file);
        }
    }
    endfsent();

#ifdef __APPLE__
    struct statfs *mounts;
    int count = getmntinfo(&mounts, MNT_NOWAIT);

    for (int i = 0; i < count; i++) {
        if (strcmp("/", mounts[i].f_mntonname) != 0) {
            unique_volumes.insert(mounts[i].f_mntonname);
        }
    }

    std::vector<std::string> cloud_paths = {
        "/Users/" + std::string(getenv("USER") ? getenv("USER") : "") + "/Library/CloudStorage",
        "/Volumes"
    };

    for (const auto& base_path : cloud_paths) {
        if (std::filesystem::exists(base_path)) {
            try {
                for (const auto& entry : std::filesystem::directory_iterator(base_path)) {
                    if (entry.is_directory()) {
                        std::string path = entry.path().string();
                        if (path.find("OneDrive") != std::string::npos ||
                            path.find("iCloud") != std::string::npos ||
                            path.find("Google Drive") != std::string::npos ||
                            path.find("GoogleDrive") != std::string::npos ||
                            path.find("Dropbox") != std::string::npos) {
                            unique_volumes.insert(path);
                            }
                    }
                }
            } catch (const std::filesystem::filesystem_error& e) {
                std::cerr << "Error scanning " << base_path << ": " << e.what() << std::endl;
            }
        }
    }
#endif


    mounted_folders.assign(unique_volumes.begin(), unique_volumes.end());

    std::cout << "Found " << mounted_folders.size() << " mounted folders:" << std::endl;
    for (const auto& folder : mounted_folders) {
        std::cout << "  " << folder << std::endl;
    }


    return mounted_folders;
    // return {unique_volumes.begin(), unique_volumes.end()};

}
