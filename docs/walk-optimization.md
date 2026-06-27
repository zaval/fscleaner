
==== Message #1 ====
User: 
how to use `std::async` and move the function execution to a new thread? If possible, even create a threadpool and use it


==== Message #2 ====
Assistant: 
I'll help you understand how to use `std::async` effectively and create a thread pool. Looking at your code, I can see you're already using `std::async` in the `walk` function, but let me explain how it works and show you how to implement a proper thread pool.

## Using `std::async` to Move Execution to New Threads

`std::async` is a high-level way to run functions asynchronously. Here's how it works:

```cpp
#include <future>
#include <thread>

// Basic usage
auto future = std::async(std::launch::async, function, args...);
auto result = future.get(); // blocks until completion
```

### Launch Policies

- `std::launch::async` - **Forces execution on a new thread**
- `std::launch::deferred` - Executes when `.get()` or `.wait()` is called (lazy evaluation)
- `std::launch::async | std::launch::deferred` - Implementation chooses (default)

In your current code, you're using it correctly:

```cpp
futures.emplace_back(std::async(std::launch::async,
    [&](const fs::path& path_) -> std::uintmax_t {
        return walk(path_, db, newParent, ignoreFiles);
    }, p));
```

## Creating a Thread Pool

However, `std::async` creates a new thread for each call, which can be inefficient. Here's a simple thread pool implementation:

**ThreadPool.h**

```cpp
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool {
public:
    ThreadPool(size_t threads = std::thread::hardware_concurrency());
    
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>;
    
    ~ThreadPool();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// Constructor
inline ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for(size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] {
            for(;;) {
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });
                    
                    if(this->stop && this->tasks.empty())
                        return;
                    
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                
                task();
            }
        });
    }
}

// Enqueue task
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type> {
    
    using return_type = typename std::result_of<F(Args...)>::type;
    
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    
    std::future<return_type> res = task->get_future();
    
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        
        tasks.emplace([task](){ (*task)(); });
    }
    
    condition.notify_one();
    return res;
}

// Destructor
inline ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    
    condition.notify_all();
    
    for(std::thread &worker: workers)
        worker.join();
}

#endif // THREADPOOL_H
```

## Modifying Your Code to Use Thread Pool

Here's how you can modify your `walk` function to use the thread pool:

**main.cpp**

```cpp
#include <filesystem>
#include <iostream>
#include <vector>
// #include <omp.h>
#include <unistd.h>
#include <sstream>

#include <fcntl.h>
#include <sys/stat.h>
#include <string>
#include <system_error>


#include <algorithm>
#include <mutex>



#include "ArgumentParser.h"
#include "Database.h"

#include <fstab.h>
#include <future>
#include <sys/mount.h>

#include "fshelper.h"
#include "ThreadPool.h"  // Add this include

// ... existing code ...

std::uintmax_t walk(const fs::path &path, Database& db, const int64_t &parent, const std::vector<std::string> &ignoreFiles, ThreadPool& pool) {

    std::uintmax_t size = 0;
    std::error_code ec;
    if (fs::is_symlink(path, ec)) {
        return size; // Skip symbolic links entirely
    }

    if (fs::is_regular_file(path, ec)) {
        try {
            if (path.string().length() >= PATH_MAX) {
                size = get_file_size_for_long_path(path.string());
            } else {
                size = fs::file_size(path);
            }

            {
                std::lock_guard<std::mutex> lock(db_mutex);
                db.insert_file(path.string(), size, parent);
                print_progress();
            }

            return size;
        } catch (std::filesystem::filesystem_error const& e) {
            std::cerr << e.what() << '\n';
            return size;
        } catch (std::system_error const& e) {
            std::cerr << e.what() << '\n';
            return size;
        }
    }

    if (fs::is_directory(path, ec)) {
        auto newParent = 0;
        {
            std::lock_guard<std::mutex> lock(db_mutex);
            newParent = db.insert_folder(path.string(), parent);
        }

        std::vector<fs::path> paths;
        try {
            for (auto const& dir_entry : fs::directory_iterator{path, fs::directory_options::skip_permission_denied | fs::directory_options::follow_directory_symlink, ec}) {
                if (!fs::is_symlink(dir_entry.path(), ec)) {
                    if (std::ranges::find(ignoreFiles, dir_entry.path().string()) == ignoreFiles.end()) {
                        paths.push_back(dir_entry.path());
                    }
                }
            }
        } catch (fs::filesystem_error const& e) {
            std::cerr << "Error accessing directory: " << e.what() << '\n';
            return size;
        }

        // Use thread pool instead of std::async
        std::vector<std::future<std::uintmax_t>> futures;
        futures.reserve(paths.size());

        for (const auto& p : paths) {
            futures.emplace_back(pool.enqueue([&, p](const fs::path& path_) -> std::uintmax_t {
                return walk(path_, db, newParent, ignoreFiles, pool);
            }, p));
        }
        
        for (auto& future : futures) {
            size += future.get();
        }

        {
            std::lock_guard<std::mutex> lock(db_mutex);
            db.update_folder_size(newParent, size);
        }
    }

    return size;
}

int main(int argc, char* argv[]) {
    // ... existing code ...

    const fs::path start_path{options.start_path};
    Database db;
    
    // Create thread pool with optimal number of threads
    ThreadPool pool(std::thread::hardware_concurrency());
    
    walk(start_path, db, 0, mountFolders, pool);
    std::cout << std::endl;

    // ... rest of existing code ...
}
```

## Key Differences and Benefits

### `std::async` vs Thread Pool:

1. **`std::async`**:
   - Creates a new thread for each task
   - Simple to use
   - Can be inefficient with many small tasks
   - No control over thread creation

2. **Thread Pool**:
   - Reuses a fixed number of threads
   - More efficient for many tasks
   - Better resource management
   - Configurable thread count

### Best Practices:

1. **Use `std::launch::async`** if you want to guarantee execution on a separate thread
2. **Use thread pools** for scenarios with many tasks (like your directory traversal)
3. **Consider the overhead** - thread pools are better for many small tasks
4. **Handle exceptions** properly in both approaches

The thread pool approach will be more efficient for your use case since you're potentially creating many tasks for directory traversal, and it gives you better control over resource usage.

==== Message #3 ====
User: 
`std::result_of` was removed in c++20. What should I use instead?

==== Message #4 ====
Assistant: 
You're absolutely right! `std::result_of` was deprecated in C++17 and removed in C++20. You should use `std::invoke_result` instead, which is the modern replacement.

Here's the updated ThreadPool code with the C++20 compatible changes:

**ThreadPool.h**

```cpp
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <type_traits>  // Add this for std::invoke_result

class ThreadPool {
public:
    ThreadPool(size_t threads = std::thread::hardware_concurrency());
    
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::invoke_result<F, Args...>::type>;
    
    ~ThreadPool();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// Constructor
inline ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for(size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] {
            for(;;) {
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });
                    
                    if(this->stop && this->tasks.empty())
                        return;
                    
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                
                task();
            }
        });
    }
}

// Enqueue task
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) 
    -> std::future<typename std::invoke_result<F, Args...>::type> {
    
    using return_type = typename std::invoke_result<F, Args...>::type;
    
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    
    std::future<return_type> res = task->get_future();
    
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        
        tasks.emplace([task](){ (*task)(); });
    }
    
    condition.notify_one();
    return res;
}

// Destructor
inline ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    
    condition.notify_all();
    
    for(std::thread &worker: workers)
        worker.join();
}

#endif // THREADPOOL_H
```

## Key Changes for C++20:

1. **`std::result_of<F(Args...)>::type`** → **`std::invoke_result<F, Args...>::type`**
2. **Added `#include <type_traits>`** for `std::invoke_result`

## Alternative: Using `std::invoke_result_t` (C++14+)

You can also use the shorter alias:

```cpp
template<class F, class... Args>
auto enqueue(F&& f, Args&&... args) 
    -> std::future<std::invoke_result_t<F, Args...>> {
    
    using return_type = std::invoke_result_t<F, Args...>;
    
    // ... rest of the function
}
```

## Even More Modern: Auto Return Type Deduction (C++14+)

For an even cleaner approach, you can let the compiler deduce the return type:

```cpp
template<class F, class... Args>
auto enqueue(F&& f, Args&&... args) {
    using return_type = std::invoke_result_t<F, Args...>;
    
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    
    std::future<return_type> res = task->get_future();
    
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        
        tasks.emplace([task](){ (*task)(); });
    }
    
    condition.notify_one();
    return res;
}
```

The `std::invoke_result` is more flexible than `std::result_of` because it properly handles callable objects that aren't just function pointers, including lambdas, function objects, and member function pointers.


==== Message #5 ====
User: 
In file included from /Users/zaval/projects/qt/fsCleaner/fsscan/main.cpp:27:
/Users/zaval/projects/qt/fsCleaner/fsscan/threadpool.h:25:10: error: function 'ThreadPool::enqueue<(lambda at /Users/zaval/projects/qt/fsCleaner/fsscan/main.cpp:153:47), const std::filesystem::path &>' is used but not defined in this translation unit, and cannot be defined in any other translation unit because its type does not have linkage
   25 |     auto enqueue(F&& f, Args&&... args)
      |          ^


==== Message #6 ====
Assistant: 
The issue is that **template functions must be defined in the header file**, not in a separate `.cpp` file. This is because templates are instantiated at compile time, and the compiler needs to see the full definition when instantiating the template.

Here's how to fix it:

## Solution 1: Move the template definition to the header

**threadpool.h**
```cpp
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <type_traits>

class ThreadPool {
public:
    ThreadPool(size_t threads = std::thread::hardware_concurrency());
    
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::invoke_result<F, Args...>::type> {
        
        using return_type = typename std::invoke_result<F, Args...>::type;
        
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<return_type> res = task->get_future();
        
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            
            if(stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");
            
            tasks.emplace([task](){ (*task)(); });
        }
        
        condition.notify_one();
        return res;
    }
    
    ~ThreadPool();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

#endif // THREADPOOL_H
```

<llm-snippet-file>threadpool.cpp</llm-snippet-file>
```cpp
#include "threadpool.h"

ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for(size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] {
            for(;;) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });

                    if(this->stop && this->tasks.empty())
                        return;

                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }

                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    
    condition.notify_all();
    
    for(std::thread &worker: workers)
        worker.join();
}
```

## Alternative Solution: Header-only implementation

You can also make it completely header-only by moving everything to the header and removing the `.cpp` file:

**threadpool.h**

```cpp
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <type_traits>

class ThreadPool {
public:
    ThreadPool(size_t threads = std::thread::hardware_concurrency()) : stop(false) {
        for(size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this] {
                for(;;) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });

                        if(this->stop && this->tasks.empty())
                            return;

                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task();
                }
            });
        }
    }
    
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::invoke_result<F, Args...>::type> {
        
        using return_type = typename std::invoke_result<F, Args...>::type;
        
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<return_type> res = task->get_future();
        
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            
            if(stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");
            
            tasks.emplace([task](){ (*task)(); });
        }
        
        condition.notify_one();
        return res;
    }
    
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        
        condition.notify_all();
        
        for(std::thread &worker: workers)
            worker.join();
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

#endif // THREADPOOL_H
```

If you go with the header-only approach, you'll need to update your CMakeLists.txt:

**CMakeLists.txt**

```cmake
set(CMAKE_CXX_STANDARD 20)

add_executable(fsscan
        ../main.cpp
        Database.cpp
        ArgumentParser.cpp
        ArgumentParser.h
        fshelper.cpp
        fshelper.h
        threadpool.h
        # Remove threadpool.cpp from here if going header-only
)

target_link_libraries(fsscan PRIVATE
        SQLite3
)
```

## Why This Happens

The error occurs because:

1. **Templates are instantiated at compile-time** - the compiler needs to see the full definition when creating specific instances
2. **Linkage issues** - when templates are defined in `.cpp` files, they don't have external linkage for all possible instantiations
3. **Translation unit separation** - each `.cpp` file is compiled separately, so the template definition in `threadpool.cpp` isn't visible when compiling `main.cpp`

I recommend going with **Solution 1** (moving just the template to the header) to keep the non-template parts in the `.cpp` file for faster compilation times.


==== Message #7 ====
User: 
suggest me how to make function walk working faster on large amount of files


==== Message #8 ====
Assistant: 
Here are several optimization strategies to make your `walk` function significantly faster when processing large amounts of files:

## 1. **Reduce Database Lock Contention (Batch Processing)**

Currently, you're acquiring the database lock for every single file/folder. This creates a bottleneck:

```cpp
struct FileEntry {
    std::string path;
    std::uintmax_t size;
    int64_t parent;
};

struct FolderEntry {
    std::string path;
    int64_t parent;
    std::uintmax_t size;
};

thread_local std::vector<FileEntry> file_batch;
thread_local std::vector<FolderEntry> folder_batch;
constexpr size_t BATCH_SIZE = 1000;

void flush_batches(Database& db) {
    if (!file_batch.empty() || !folder_batch.empty()) {
        std::lock_guard<std::mutex> lock(db_mutex);
        
        // Batch insert files
        for (const auto& entry : file_batch) {
            db.insert_file(entry.path, entry.size, entry.parent);
        }
        
        // Batch insert folders
        for (const auto& entry : folder_batch) {
            db.insert_folder_with_size(entry.path, entry.parent, entry.size);
        }
        
        file_batch.clear();
        folder_batch.clear();
    }
}

std::uintmax_t walk_optimized(const fs::path &path, Database& db, const int64_t &parent, 
                              const std::vector<std::string> &ignoreFiles) {
    
    std::uintmax_t size = 0;
    std::error_code ec;
    
    if (fs::is_symlink(path, ec)) {
        return size;
    }

    if (fs::is_regular_file(path, ec)) {
        try {
            if (path.string().length() >= PATH_MAX) {
                size = get_file_size_for_long_path(path.string());
            } else {
                size = fs::file_size(path);
            }

            // Batch instead of immediate insert
            file_batch.push_back({path.string(), size, parent});
            
            if (file_batch.size() >= BATCH_SIZE) {
                flush_batches(db);
            }
            
            processed_files++;
            return size;
            
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            return size;
        }
    }

    if (fs::is_directory(path, ec)) {
        // Process directory entries...
        // At the end, add to folder batch instead of immediate insert
        folder_batch.push_back({path.string(), parent, size});
        
        if (folder_batch.size() >= BATCH_SIZE) {
            flush_batches(db);
        }
    }

    return size;
}
```

## 2. **Optimize Directory Traversal**

Use `directory_entry` status caching and avoid multiple filesystem calls:

```cpp
std::uintmax_t walk_fast(const fs::path &path, Database& db, const int64_t &parent, 
                         const std::unordered_set<std::string> &ignoreFiles) {  // Use unordered_set
    
    std::uintmax_t size = 0;
    std::error_code ec;
    
    if (fs::is_directory(path, ec)) {
        // Pre-allocate and reserve space
        std::vector<fs::path> directories;
        std::vector<std::pair<fs::path, std::uintmax_t>> files;
        directories.reserve(100);  // Estimate
        files.reserve(1000);       // Estimate
        
        try {
            for (auto const& dir_entry : fs::directory_iterator{
                path, 
                fs::directory_options::skip_permission_denied, 
                ec}) {
                
                // Cache the status to avoid multiple filesystem calls
                auto file_status = dir_entry.status(ec);
                if (ec) continue;
                
                if (fs::is_symlink(file_status)) continue;
                
                const auto& entry_path = dir_entry.path();
                if (ignoreFiles.count(entry_path.string())) continue;
                
                if (fs::is_directory(file_status)) {
                    directories.push_back(entry_path);
                } else if (fs::is_regular_file(file_status)) {
                    // Get size from cached status if possible
                    std::uintmax_t file_size = 0;
                    try {
                        if (entry_path.string().length() >= PATH_MAX) {
                            file_size = get_file_size_for_long_path(entry_path.string());
                        } else {
                            file_size = dir_entry.file_size(ec);
                            if (ec) file_size = fs::file_size(entry_path);
                        }
                        files.emplace_back(entry_path, file_size);
                    } catch (const std::exception& e) {
                        std::cerr << "Error getting file size: " << e.what() << '\n';
                    }
                }
            }
        } catch (fs::filesystem_error const& e) {
            std::cerr << "Error accessing directory: " << e.what() << '\n';
            return size;
        }
        
        // Process files (no recursion needed)
        for (const auto& [file_path, file_size] : files) {
            file_batch.push_back({file_path.string(), file_size, parent});
            size += file_size;
        }
        
        // Process directories recursively
        if (!directories.empty()) {
            std::vector<std::future<std::uintmax_t>> futures;
            futures.reserve(directories.size());
            
            for (const auto& dir_path : directories) {
                futures.emplace_back(std::async(std::launch::async,
                    [&](const fs::path& path_) -> std::uintmax_t {
                        return walk_fast(path_, db, parent, ignoreFiles);
                    }, dir_path));
            }
            
            for (auto& future : futures) {
                size += future.get();
            }
        }
    }
    
    return size;
}
```

## 3. **Use Thread Pool with Work Stealing**

Implement a more sophisticated threading strategy:

```cpp
class WorkStealingPool {
private:
    static constexpr size_t MIN_WORK_SIZE = 10;  // Don't create threads for small directories
    
public:
    template<typename F>
    auto submit_if_worthwhile(const std::vector<fs::path>& paths, F&& func) {
        if (paths.size() < MIN_WORK_SIZE) {
            // Process synchronously for small workloads
            std::uintmax_t total = 0;
            for (const auto& path : paths) {
                total += func(path);
            }
            return std::async(std::launch::deferred, [total]() { return total; });
        } else {
            // Process asynchronously for large workloads
            return std::async(std::launch::async, [paths, func]() {
                std::uintmax_t total = 0;
                for (const auto& path : paths) {
                    total += func(path);
                }
                return total;
            });
        }
    }
};
```

## 4. **Memory Optimization**

Reduce memory allocations and string operations:

```cpp
// Use string_view for path comparisons
bool should_ignore(const fs::path& path, const std::unordered_set<std::string_view>& ignoreFiles) {
    return ignoreFiles.count(path.native());  // Use native() to avoid string conversion
}

// Pre-allocate progress reporting
std::atomic<int> processed_files_atomic{0};
void print_progress_atomic() {
    int current = processed_files_atomic.fetch_add(1);
    if (current % 1000 == 0) {  // Update less frequently
        if (is_terminal()) {
            std::cout << "\r\033[K" << "Processing " << current << " files" << std::flush;
        }
    }
}
```

## 5. **Complete Optimized Implementation**

<llm-snippet-file>main.cpp</llm-snippet-file>
```cpp
// ... existing includes ...
#include <unordered_set>
#include <atomic>

std::mutex db_mutex;
std::atomic<int> processed_files_atomic{0};

// Batch processing structures
struct FileEntry {
    std::string path;
    std::uintmax_t size;
    int64_t parent;
};

thread_local std::vector<FileEntry> file_batch;
constexpr size_t BATCH_SIZE = 1000;

void flush_file_batch(Database& db) {
    if (!file_batch.empty()) {
        std::lock_guard<std::mutex> lock(db_mutex);
        for (const auto& entry : file_batch) {
            db.insert_file(entry.path, entry.size, entry.parent);
        }
        processed_files_atomic.fetch_add(file_batch.size());
        file_batch.clear();
    }
}

void print_progress_optimized() {
    static std::atomic<int> last_reported{0};
    int current = processed_files_atomic.load();
    int last = last_reported.load();
    
    if (current - last >= 1000) {  // Report every 1000 files
        if (last_reported.compare_exchange_weak(last, current)) {
            if (is_terminal()) {
                std::cout << "\r\033[K" << "Processing " << current << " files" << std::flush;
            }
        }
    }
}

std::uintmax_t walk_optimized(const fs::path &path, Database& db, const int64_t &parent, 
                              const std::unordered_set<std::string> &ignoreFiles) {
    
    std::uintmax_t size = 0;
    std::error_code ec;
    
    if (fs::is_symlink(path, ec)) {
        return size;
    }

    if (fs::is_regular_file(path, ec)) {
        try {
            if (path.string().length() >= PATH_MAX) {
                size = get_file_size_for_long_path(path.string());
            } else {
                size = fs::file_size(path);
            }

            file_batch.push_back({path.string(), size, parent});
            
            if (file_batch.size() >= BATCH_SIZE) {
                flush_file_batch(db);
            }
            
            print_progress_optimized();
            return size;
            
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            return size;
        }
    }

    if (fs::is_directory(path, ec)) {
        auto newParent = 0;
        {
            std::lock_guard<std::mutex> lock(db_mutex);
            newParent = db.insert_folder(path.string(), parent);
        }

        std::vector<fs::path> directories;
        std::vector<std::pair<fs::path, std::uintmax_t>> files;
        
        try {
            for (auto const& dir_entry : fs::directory_iterator{
                path, 
                fs::directory_options::skip_permission_denied | fs::directory_options::follow_directory_symlink, 
                ec}) {
                
                auto file_status = dir_entry.status(ec);
                if (ec || fs::is_symlink(file_status)) continue;
                
                const auto& entry_path = dir_entry.path();
                if (ignoreFiles.count(entry_path.string())) continue;
                
                if (fs::is_directory(file_status)) {
                    directories.push_back(entry_path);
                } else if (fs::is_regular_file(file_status)) {
                    try {
                        std::uintmax_t file_size;
                        if (entry_path.string().length() >= PATH_MAX) {
                            file_size = get_file_size_for_long_path(entry_path.string());
                        } else {
                            file_size = dir_entry.file_size(ec);
                            if (ec) file_size = fs::file_size(entry_path);
                        }
                        files.emplace_back(entry_path, file_size);
                    } catch (const std::exception& e) {
                        // Skip files we can't read
                    }
                }
            }
        } catch (fs::filesystem_error const& e) {
            std::cerr << "Error accessing directory: " << e.what() << '\n';
            return size;
        }

        // Process files in batch
        for (const auto& [file_path, file_size] : files) {
            file_batch.push_back({file_path.string(), file_size, newParent});
            size += file_size;
            
            if (file_batch.size() >= BATCH_SIZE) {
                flush_file_batch(db);
            }
        }

        // Process directories with adaptive threading
        if (!directories.empty()) {
            constexpr size_t MIN_ASYNC_SIZE = 5;
            
            if (directories.size() < MIN_ASYNC_SIZE) {
                // Process small directories synchronously
                for (const auto& dir_path : directories) {
                    size += walk_optimized(dir_path, db, newParent, ignoreFiles);
                }
            } else {
                // Process large directories asynchronously
                std::vector<std::future<std::uintmax_t>> futures;
                futures.reserve(directories.size());
                
                for (const auto& dir_path : directories) {
                    futures.emplace_back(std::async(std::launch::async,
                        [&](const fs::path& path_) -> std::uintmax_t {
                            return walk_optimized(path_, db, newParent, ignoreFiles);
                        }, dir_path));
                }
                
                for (auto& future : futures) {
                    size += future.get();
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(db_mutex);
            db.update_folder_size(newParent, size);
        }
    }

    return size;
}

int main(int argc, char* argv[]) {
    // ... existing code ...

    // Convert to unordered_set for faster lookups
    std::unordered_set<std::string> mountSet(mountFolders.begin(), mountFolders.end());

    const fs::path start_path{options.start_path};
    Database db;

    walk_optimized(start_path, db, 0, mountSet);
    
    // Flush any remaining batches
    flush_file_batch(db);
    
    std::cout << std::endl;

    // ... rest of existing code ...
}
```

## Key Performance Improvements:

1. **Batch Database Operations**: Reduces lock contention by 1000x
2. **Cached Directory Status**: Avoids redundant filesystem calls
3. **Adaptive Threading**: Only creates threads for larger workloads
4. **Optimized Data Structures**: Uses `unordered_set` for O(1) lookups
5. **Reduced Progress Reporting**: Updates less frequently
6. **Memory Pre-allocation**: Reserves space for containers

These optimizations should significantly improve performance, especially on large directory trees.

