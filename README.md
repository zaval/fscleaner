# fsCleaner

fsCleaner is a Qt-based desktop application for inspecting and cleaning file system content. It provides a Qt Quick (Qt Quick Controls 2) user interface that lets you browse folders, inspect files, and run scan/cleanup operations.

The project is written in C++20 and QML and targets Qt 6.

---

## Features

- Qt Quick–based UI with a native-looking macOS style.
- Tree view of folders and files.
- Predefined folder list (Home, Documents, Downloads, cache paths, etc.).
- Configurable ignore / include settings (e.g. ignore mounted directories).
- Uses a separate scanning helper (`fsscan`) and a core library for media and file metadata.
- Uses SQLite for storing metadata.
- Uses FFmpeg (`libavformat`, `libavcodec`, `libavutil`) for media information.
- Uses KF6Archive for archive handling.

---

## Requirements

### Build-time

- CMake ≥ 3.16
- C++20-compatible compiler (Clang or GCC)
- Qt 6 (tested with Qt 6.10.x), with the following modules:
    - `Qt6::Quick`
    - `Qt6::QuickControls2`
    - `Qt6::Qml`
    - `Qt6::Core`
    - `Qt6::Sql`
    - `Qt6::Widgets`
- SQLite3 development files
- FFmpeg / libav:
    - `libavformat`
    - `libavcodec`
    - `libavutil`
- Extra CMake Modules (ECM) 6.13 (fetched automatically if not found)
- KF6Archive 6.13 (fetched and built automatically if not found)

On **macOS** you additionally need:

- Python 3 (for the `find_libav_deps.py` helper script)
- A recent Xcode toolchain or Command Line Tools

---

## Building

Clone the repository and configure a build directory:

```bash
git clone https://github.com/zaval/fscleaner
cd fsCleaner

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="<path-to-Qt>"
cmake --build build
```

Replace `<path-to-Qt>` with the directory where Qt is installed (the parent of `bin`, `lib`, `qml`, etc.).

### Notes

- On macOS the target is built as a `.app` bundle (`appfsCleaner.app`).
- The build system downloads and builds ECM and KF6Archive if they are not available on your system.
- `fsscan` is built as a separate target and copied into the application bundle on macOS.
- To build a signed  release build for MacOS, set `-DCMAKE_CODESIGN_IDENTITY=<identity>`. To find a correct identity, run `security find-identity -v -p codesigning`.

---

## Running

### From the build directory

On **macOS**:

```bash
cd build
open appfsCleaner.app
```

Or run the executable directly:

```bash
./appfsCleaner.app/Contents/MacOS/appfsCleaner
```

On **Linux**:

```bash
cd build
./appfsCleaner
```

(Ensure that the Qt and FFmpeg libraries are locatable via `LD_LIBRARY_PATH` / `rpath` or system installation.)

---

## Logging and Debugging

fsCleaner uses Qt’s logging categories. You can enable verbose logging (including QML and dialog internals) through the `QT_LOGGING_RULES` environment variable.

Example (bash / zsh):

```bash
QT_LOGGING_RULES="qt.qml.*=true;qt.quick.*=true;qt.quick.dialogs.*=true" \
  ./appfsCleaner.app/Contents/MacOS/appfsCleaner
```

Useful categories:

- `qt.qml.*` – QML engine, bindings, type loading.
- `qt.quick.*` – Qt Quick scene graph and controls.
- `qt.quick.dialogs.*` – Qt Quick dialogs (message/file/folder).
- `qt.qpa.*` – Platform integration (Cocoa/xcb/etc.).

---


## Packaging

The project uses CPack for packaging.

On macOS:

- A DMG can be produced (see `CPack` configuration in `CMakeLists.txt`).
- Some cleanup is performed before packaging to reduce bundle size (removing unneeded `share` and `lib` directories from the staging area).
- Qt’s `qt_generate_deploy_qml_app_script` is used to deploy QML modules into the application bundle.
- FFmpeg (`libav*`) dependencies are discovered by the `find_libav_deps.py` helper and installed into the bundle.

Example:

```bash
cd cmake-build-debug
cpack
```

The resulting DMG will be in the build directory (e.g. `fscleaner-1.0.0-Darwin.dmg`).

---

## Flatpak build (Linux)

On Linux, you can build and test fsCleaner in a Flatpak sandbox using the provided Flatpak manifest `me.zaval.fsclean.yaml` in the project root.

### Prerequisites

- Flatpak
- `flatpak-builder`
- Appropriate Flatpak SDK/runtime referenced by `me.zaval.fsclean.yaml` installed

For example, to install a typical Qt/KDE runtime (adjust if your manifest uses different IDs):

```bash
flatpak remote-add --if-not-exists --user flathub https://dl.flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.kde.Platform//6.10
flatpak install flathub org.kde.Sdk//6.10
```

### Building the Flatpak

From the project root:

```bash
mkdir -p flatpak-build

flatpak-builder \
  --force-clean \
  --user \
  --install-deps-from=flathub \
  --repo=repo \
  flatpak-build \
  me.zaval.fsclean.yaml 
```

This will:

- Use `me.zaval.fsclean.yaml` to drive the build (fetch dependencies, run CMake, etc.).
- Produce a Flatpak repo in `flatpak-build`.
- Generate a Flatpak for the application (and optionally install it into a local repository if configured in the manifest).

### Generate the Standalone File

```bash
flatpak build-bundle repo fscleaner.flatpak me.zaval.fscleaner --runtime-repo=https://flathub.org/repo/flathub.flatpakrepo
```

###  Install the Standalone File

```bash
flatpak install --user fscleaner.flatpak
```

### Installing and running the Flatpak locally

To install the local build:

```bash
flatpak-builder --user --install --force-clean flatpak-build me.zaval.fsclean.yaml
```

Then run fsCleaner (replace the app ID with the one defined in `me.zaval.fsclean.yaml`):

```bash
flatpak run me.zaval.fsclean
```

Check the `app-id` field inside `me.zaval.fsclean.yaml` if the above ID does not match.

---


## Project Structure

Top-level layout:

- `main.cpp` – Application entry point, QML engine setup.
- `Main.qml` – Main application window and top-level UI.
- `SettingsPage.qml`, `SelectFolderPage.qml`, `FilesTreeView.qml`, etc. – QML UI components.
- `core/` – Core C++ logic (helpers, media information, hashing, etc.).
- `fsscan/` – Scanner helper tool.
- `icons/` – Icon theme resources.
- `resources/` – Application icons (for bundle icon, desktop entry, etc.).
- `FSCleanerStyle/` – Style-related resources.
- `CMakeLists.txt` – Top-level CMake build configuration.

---

## License

See [`LICENSE`](LICENSE) for licensing information.