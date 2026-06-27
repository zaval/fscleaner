# fsscan

A fast, multithreaded filesystem scanner that walks a directory tree and stores the results in a SQLite database. It is used as the backend data-collection tool for the fsCleaner application.

## How it works

### 1. Argument parsing

On startup `ArgumentParser` processes CLI arguments to produce a `ProgramOptions` struct:

| Option | Description |
|---|---|
| `path` (positional) | Root directory to scan (default: current directory) |
| `-d / --database <path>` | Output database file (default: `filesystem_stats.db`) |
| `-v / --verbose` | Print start path and database path before scanning |
| `-i / --ignore-mounts` | Skip mounted/external volumes |
| `-h / --help` | Print help and exit |

### 2. Mount-point detection

`FSHelper::getMountedFolders()` returns a list of paths that represent external or network mount points. Unless `-i / --ignore-mounts` is passed, those paths are loaded into an `unordered_set` so that the walker can skip them during traversal.

### 3. In-memory tree construction

Before any I/O begins, a root `TreeEntry` node is created for the start path. `TreeEntry` is an in-memory tree where each node tracks:

- `rowid` — a globally unique, atomically assigned integer ID
- `path` — the absolute path string
- `size` — the accumulated byte size of all descendants
- `is_folder` — whether the node is a directory
- `parent` — raw pointer to the parent node

When `add_child()` is called it immediately propagates the child's size upward through the ancestor chain, so by the time the walk is complete every folder already holds its correct total size — no second pass is needed.

### 4. Breadth-first multithreaded walk

The scanner uses a `ThreadPool` sized to `std::thread::hardware_concurrency()`. The main loop processes one level of the tree at a time:

1. For every directory in the current `records` list, a `walk()` task is enqueued on the thread pool.
2. `walk()` iterates the directory with `std::filesystem::directory_iterator`, skipping symlinks and paths in the ignore set, and returns a `WalkResult` containing:
   - `folders` — subdirectories found at this level
   - `files` — regular files with their sizes
3. The main thread collects all futures, adds new `TreeEntry` nodes to the tree, and pushes the newly discovered subdirectories into the next iteration's `records` list.
4. The loop repeats until no more directories remain.

**File size measurement:**

- **macOS** — `stat()` is called and `st_blocks × 512` is returned, giving the actual on-disk allocation rather than the logical file size.
- **Linux/other** — `dir_entry.file_size()` is used; for paths longer than `PATH_MAX`, a raw `open()` + `fstat()` fallback is used instead.

### 5. Progress reporting

Every 100 files processed, the scanner prints a running count to stdout. When stdout is a terminal the line is overwritten in-place (`\r\033[K`); when piped or redirected, each update is printed on its own line.

### 6. Persisting results to SQLite

After the walk finishes, the tree is serialised to an in-memory SQLite database in a single depth-first traversal of the `TreeEntry` tree (`save_to_db`):

- Folder nodes are written with `insert_folder()`, including their pre-calculated total size.
- File nodes are written with `insert_file()`.
- Inserts are batched inside explicit transactions (committed every 100 inserts) to maximise write throughput.

Once the in-memory database is fully populated, it is written to the output file via `Database::save_to_file()`.

## Building

`fsscan` is a sub-project of fsCleaner and is built via the parent CMake configuration. It requires:

- C++20
- SQLite3

```bash
cmake -B build -S .
cmake --build build --target fsscan
```

## Usage

```
fsSize [options] [path]

Options:
  -h, --help            Show this help message
  -v, --verbose         Enable verbose output
  -d, --database <path> Output database path (default: filesystem_stats.db)
  -i, --ignore-mounts   Skip mounted/external volumes

Arguments:
  path                  Starting directory (default: current directory)
```

**Examples:**

```bash
# Scan the home directory
fsscan ~

# Scan /var and save results to a specific file
fsscan -d /tmp/var_scan.db /var

# Verbose scan, skip external drives
fsscan -v -i /
```
