qt_add_library(uicomponents STATIC)

#set_target_properties(uicomponents PROPERTIES
#        QT_QMLCACHEGEN_ARGUMENTS "--verbose"
#)

#set_source_files_properties("Card.qml"
#        PROPERTIES QT_DISCARD_FILE_CONTENTS TRUE
#)

qt_add_qml_module(uicomponents
    URI uicomponents
    VERSION 1.0
    QML_FILES
        Card.qml
        LeftMenu.qml
        LeftMenuItem.qml
        LeftMenuItemDelegate.qml
        MainWindow.qml
        SettingsWindow.qml
        FolderSettingsRowDelegate.qml
        SelectFolderScreen.qml
        DashboardWindow.qml
        ScanScreen.qml
        ScanResultScreen.qml
        FileProperties.qml
        MetadataRow.qml
        HashMetadataRow.qml
        DeleteScanResultConfirmation.qml
        FileViewerArchive.qml
        SearchTextField.qml
        FileViewerText.qml
        FullTextArea.qml
    SOURCES
        messagebox.cpp
        messagebox.h
        foldersettingslistmodel.cpp
        foldersettingslistmodel.h
        fileselector.cpp
        fileselector.h
        fsmodel.cpp
        fsmodel.h
        basefsmodelloader.cpp
        basefsmodelloader.h
        sqlitefsmodelloader.cpp
        sqlitefsmodelloader.h
        archivefsmodelloader.cpp
        archivefsmodelloader.h
        fulltextareaengine.cpp
        fulltextareaengine.h
        processrunner.cpp
        processrunner.h
        filemetadata.h
        filemetadata.cpp
    DEPENDENCIES
            QtQuick
)

target_link_libraries(uicomponents PRIVATE
    Qt6::Widgets
    Qt6::Quick
    Qt6::Qml
    Qt6::QuickControls2
    Qt6::Sql
    KF6::Archive
    core
)

if(APPLE)
    set(BUNDLE_NAME $<TARGET_FILE_NAME:appfsCleaner>)
    install(TARGETS uicomponents
            LIBRARY DESTINATION ${BUNDLE_NAME}.app/Contents/Frameworks
    )
else()
    install(TARGETS uicomponents
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()
