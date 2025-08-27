cmake_minimum_required(VERSION 3.16)

# Settings

#if (APPLE)
#    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/app) # for testing
#endif ()

project(fsCleaner VERSION 1.0.0 LANGUAGES CXX)
cmake_policy(SET CMP0169 OLD)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
include(FeatureSummary)

# Dependencies

find_package(SQLite3 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
        libavformat
        libavcodec
        libavutil
)

find_package(Qt6 REQUIRED COMPONENTS Quick Sql Core Qml QuickControls2 QmlCompiler Widgets)

include(FetchContent)

find_package(ECM 6.13.0 NO_MODULE)
if(NOT ECM_FOUND)


    set(APPLE_SUPPRESS_X11_WARNING ON)
    FetchContent_Declare(
            ECM
            GIT_REPOSITORY https://invent.kde.org/frameworks/extra-cmake-modules.git
            GIT_TAG v6.13.0
            SYSTEM
    )

    FetchContent_GetProperties(ECM)
    if(NOT ecm_POPULATED)
        FetchContent_Populate(ECM)
        execute_process(
                COMMAND ${CMAKE_COMMAND}
                -S ${ecm_SOURCE_DIR}
                -B ${ecm_SOURCE_DIR}/build
                -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/ecm_install
                -DSHARE_INSTALL_DIR=${CMAKE_CURRENT_BINARY_DIR}/ecm_install
                -DCMAKE_POLICY_DEFAULT_CMP0177=NEW
                -DBUILD_DOC=OFF
                -DBUILD_TESTING=OFF
                RESULT_VARIABLE config_result
        )
        execute_process(
                COMMAND ${CMAKE_COMMAND} --build ${ecm_SOURCE_DIR}/build
                RESULT_VARIABLE build_result
        )
        execute_process(
                COMMAND ${CMAKE_COMMAND} --install ${ecm_SOURCE_DIR}/build
                RESULT_VARIABLE install_result
        )
    endif()
    FetchContent_MakeAvailable(ECM)
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_BINARY_DIR}/ecm_install")
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR}/ecm_install/ECM/modules)


    unset(CMAKE_INSTALL_PREFIX)
else ()
    set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
endif ()

#set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH_BACKUP})
#set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX_BACKUP})

if (APPLE)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/app) # for testing
endif ()

find_package(KF6Archive)
if(NOT KF6Archive_FOUND)

FetchContent_Declare(
        KArchive
        GIT_REPOSITORY https://invent.kde.org/frameworks/karchive.git
        GIT_TAG v6.17.0
        CMAKE_ARGS
            -DECM_DIR=${CMAKE_CURRENT_BINARY_DIR}/ecm_install
            -DBUILD_TESTING=OFF
            -DBUILD_DOC=OFF
#            -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_MACOSX_RPATH=ON
            -DCMAKE_INSTALL_NAME_DIR=@rpath

)

FetchContent_MakeAvailable(KArchive)
endif ()

qt_standard_project_setup(REQUIRES 6.8)

# executable

qt_add_executable(appfsCleaner
    main.cpp
)

set_source_files_properties(AppSettings.qml PROPERTIES
        QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(appfsCleaner
    URI fsCleaner
    VERSION 1.0
    QML_FILES
        Main.qml

        # ui

        FilesTreeView.qml
        FilesTreeMenu.qml
        FilesTreeHeader.qml
        SettingsPage.qml
        SelectFolderPage.qml
        SelectFolderButton.qml
        FoldersTreeViewDelegate.qml
        AppSettings.qml
        FileInfo.qml
        ArchiveViewerWindow.qml
    SOURCES
        iconimageprovider.h
        iconimageprovider.cpp

        # ui

        filesystemmodel.h
        filesystemmodel.cpp
        processrunner.h
        processrunner.cpp
        filemetadata.h
        filemetadata.cpp
        uihelper.h
        uihelper.cpp
        filetypeicon.h
        filetypeicon.cpp
        archivemodel.cpp
        archivemodel.h
)

#add_subdirectory(ui)
add_subdirectory(fsscan)
add_subdirectory(core)

# Resources

file(GLOB_RECURSE ICON_FILES_ABS "icons/dataicontheme/*x*/*.png")

set(ICON_FILES)

foreach(file ${ICON_FILES_ABS})
    file(RELATIVE_PATH rel_file ${CMAKE_CURRENT_SOURCE_DIR} ${file})
    list(APPEND ICON_FILES ${rel_file})
endforeach()

list(APPEND ICON_FILES
        "icons/dataicontheme/index.theme"
        "qtquickcontrols2.conf"
)

qt_add_resources(
    appfsCleaner
    "fsCleaner"
    PREFIX "/"
    FILES
        ${ICON_FILES}
)

if (APPLE OR WINDOWS)

include(ECMAddAppIcon)
set(
        ICONS
        resources/icons/16-icon.png
        resources/icons/32-icon.png
        resources/icons/64-icon.png
        resources/icons/128-icon.png
        resources/icons/256-icon.png
        resources/icons/512-icon.png
        resources/icons/1024-icon.png
)

ecm_add_app_icon(appfsCleaner ICONS ${ICONS} OUTFILE_BASENAME "Appicon")
endif ()
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appfsCleaner PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER me.zaval.fscleaner
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if (${CMAKE_BUILD_TYPE})
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        target_compile_definitions(appfsCleaner PRIVATE QT_QML_DEBUG)
    endif ()
endif ()

target_link_libraries(appfsCleaner
    PRIVATE
    Qt6::Quick
    Qt6::Core
    Qt6::Sql
    Qt6::Qml
    Qt6::QuickControls2
    Qt6::QmlCompiler
    Qt6::Widgets
    core
    KF6::Archive
)

if(APPLE)
    add_custom_command(TARGET appfsCleaner POST_BUILD
#    add_custom_command(TARGET fsscan POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:fsscan>
        $<TARGET_BUNDLE_CONTENT_DIR:appfsCleaner>/MacOS/
        COMMENT "Copying fsscan into app bundle"
#        DEPENDS fsscan
    )
endif()



if (APPLE)
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    # Set the install rpath for appfsCleaner to look in @rpath
#    set_target_properties(appfsCleaner PROPERTIES
#            INSTALL_RPATH "@rpath;@executable_path/../Frameworks"
#            BUILD_WITH_INSTALL_RPATH FALSE
#            INSTALL_RPATH_USE_LINK_PATH TRUE
#    )

    # Configure KF6Archive to be built with @rpath
    set_target_properties(KF6Archive PROPERTIES
            INSTALL_NAME_DIR "@rpath"
            BUILD_WITH_INSTALL_RPATH FALSE
    )

    #    if (IOS)
#        set_target_properties(appfsCleaner PROPERTIES
#                MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.cmake.ios.plist"
#        )
#    else()
#        set_target_properties(appfsCleaner PROPERTIES
#                MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.cmake.macos.plist"
#        )
#    endif()

    set(deploy_tool_options_arg -qmldir=${CMAKE_SOURCE_DIR})
    if (DEFINED CMAKE_CODESIGN_IDENTITY)
        set(deploy_tool_options_arg "${deploy_tool_options_arg} -codesign=${CMAKE_CODESIGN_IDENTITY}")
    endif ()
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/app)
endif()


# Install



include(GNUInstallDirs)
install(TARGETS appfsCleaner
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(UNIX AND NOT APPLE)
    include(ECMInstallIcons)
    ecm_install_icons( ICONS ${ICONS} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons )
endif()

if(APPLE)
    set(BUNDLE_NAME $<TARGET_FILE_NAME:appfsCleaner>)
    install(TARGETS KF6Archive
            LIBRARY DESTINATION ${BUNDLE_NAME}.app/Contents/Frameworks
    )
else()
    if (DEFINED KF6Archive)
    install(TARGETS KF6Archive
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    endif ()
endif()


if(APPLE)
    # Create a custom script to find libav dependencies recursively
    configure_file(
            ${CMAKE_SOURCE_DIR}/find_libav_deps.py.in
            ${CMAKE_BINARY_DIR}/find_libav_deps.py
            @ONLY
    )

    # Execute the script to get dependency list
    execute_process(
            COMMAND python3 ${CMAKE_BINARY_DIR}/find_libav_deps.py
            OUTPUT_VARIABLE LIBAV_DEPS_REGEXES
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Convert to CMake list
    string(REPLACE ";" "\\;" LIBAV_DEPS_REGEXES "${LIBAV_DEPS_REGEXES}")
    string(REPLACE "\n" ";" LIBAV_DEPS_REGEXES "${LIBAV_DEPS_REGEXES}")

    file(READ ${CMAKE_BINARY_DIR}/libav_installs.txt LIBAV_ADDITIONAL_INSTALLS)
    string(REPLACE ";" "\\;" LIBAV_ADDITIONAL_INSTALLS "${LIBAV_ADDITIONAL_INSTALLS}")
    string(REPLACE "\n" ";" LIBAV_ADDITIONAL_INSTALLS "${LIBAV_ADDITIONAL_INSTALLS}")

    foreach (file ${LIBAV_ADDITIONAL_INSTALLS})
        get_filename_component(REAL_FILE_PATH ${file} REALPATH)
        install(FILES ${REAL_FILE_PATH}
                DESTINATION ${BUNDLE_NAME}.app/Contents/Frameworks
        )

    endforeach ()
endif()


# if(UNIX AND NOT APPLE)
#    # Skip RPATH modification for Qt QML plugins that don't support it
#    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
#    set(CMAKE_SKIP_INSTALL_RPATH TRUE)
# endif()

if(APPLE)
qt_generate_deploy_qml_app_script(
        TARGET appfsCleaner
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
        DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
        MACOS_BUNDLE_POST_BUILD
        DEPLOY_TOOL_OPTIONS ${deploy_tool_options_arg}
        PRE_INCLUDE_REGEXES
            ${LIBAV_DEPS_REGEXES}
            ".*libKF6Archive.*\\.dylib"
            ".*fsscan"
)
install(SCRIPT ${deploy_script})

endif()

set(CPACK_PACKAGE_NAME fscleaner)
if(APPLE)
    set(CPACK_GENERATOR DragNDrop)
    set(CPACK_DMG_VOLUME_NAME ${CPACK_PACKAGE_NAME}-${VERSION})
#    set(CPACK_DMG_DS_STORE_SETUP_SCRIPT ${CMAKE_BINARY_DIR}/misc/setup_dmg.scpt)
#    set(CPACK_DMG_BACKGROUND_IMAGE ${CMAKE_SOURCE_DIR}/misc/background.png)
#    configure_file(${PROJECT_SOURCE_DIR}/misc/setup_dmg.scpt.in ${CMAKE_BINARY_DIR}/misc/setup_dmg.scpt)

    set(CLEANUP_SCRIPT "${CMAKE_BINARY_DIR}/cleanup_before_pack.cmake")
    file(WRITE ${CLEANUP_SCRIPT} "
# Remove unnecessary directories before packaging
message(STATUS \"Cleaning up unnecessary directories before packaging...\")
message(STATUS \"Current working directory: \${CMAKE_CURRENT_BINARY_DIR}\")

# set(CURRENT_DIR \"\${CMAKE_BINARY_DIR}/_CPack_Packages/\${CPACK_SYSTEM_NAME}/\${CPACK_GENERATOR}/\${CPACK_PACKAGE_NAME}-\${CPACK_PACKAGE_VERSION}-\${CPACK_SYSTEM_NAME}/ALL_IN_ONE\")
set(CURRENT_DIR \"\${CPACK_PACKAGE_DIRECTORY}/_CPack_Packages/\${CPACK_SYSTEM_NAME}/\${CPACK_GENERATOR}/\${CPACK_PACKAGE_NAME}-\${CPACK_PACKAGE_VERSION}-\${CPACK_SYSTEM_NAME}/ALL_IN_ONE\")

# Get the install directory
get_filename_component(INSTALL_DIR \"\${CURRENT_DIR}\" ABSOLUTE)
message(STATUS \"Install directory (CPack staging): \${INSTALL_DIR}\")

# List contents before cleanup
file(GLOB ALL_ITEMS \"\${INSTALL_DIR}/*\")
message(STATUS \"Contents before cleanup: \${ALL_ITEMS}\")


# Remove share directories
file(GLOB SHARE_DIRS \"\${INSTALL_DIR}/share*\")
foreach(dir \${SHARE_DIRS})
    if(EXISTS \"\${dir}\")
        message(STATUS \"Removing: \${dir}\")
        file(REMOVE_RECURSE \"\${dir}\")
    endif()
endforeach()

# Remove lib directories (if you don't need them in the final package)
file(GLOB LIB_DIRS \"\${INSTALL_DIR}/lib*\")
foreach(dir \${LIB_DIRS})
 if(EXISTS \"\${dir}\")
     message(STATUS \"Removing: \${dir}\")
     file(REMOVE_RECURSE \"\${dir}\")
 endif()
endforeach()


message(STATUS \"Cleanup completed.\")
")
    set(CPACK_PRE_BUILD_SCRIPTS ${CLEANUP_SCRIPT})



endif()

include(CPack)

get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
list(REMOVE_ITEM CPACK_COMPONENTS_ALL "_install_html_docs_KF6Archive" "_install_qch_docs_KF6Archive")

feature_summary(
        FATAL_ON_MISSING_REQUIRED_PACKAGES
        WHAT ALL
)


# cpack -DCPACK_COMPONENTS_ALL="KArchive;Unspecified"
