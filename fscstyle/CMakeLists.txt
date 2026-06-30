qt_add_library(fscstyle STATIC)

set_source_files_properties(Theme.qml PROPERTIES
        QT_QML_SINGLETON_TYPE TRUE
)

file(GLOB QML_FILES
        CONFIGURE_DEPENDS
        RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
        "*.qml"
)

set_source_files_properties(${QML_FILES}
        PROPERTIES QT_DISCARD_FILE_CONTENTS TRUE
)

qt_add_qml_module(fscstyle
    URI fscstyle
    VERSION 1.0
    QML_FILES
        ${QML_FILES}
#        Theme.qml
#        Button.qml
#        Label.qml
#        Switch.qml
#        CheckBox.qml
#        ToolBar.qml
#        ProgressBar.qml
#        HorizontalHeaderView.qml
#        TreeViewDelegate.qml
#        TextField.qml
#        TextArea.qml
#        BusyIndicator.qml
#        Menu.qml
#        MenuItem.qml
    DEPENDENCIES
        QtQuick
        QtQuick.Controls.Basic
)


qt_add_resources(fscstyle
        PREFIX "/"
        FILES
        fonts/Inter.ttf
        fonts/JetBrainsMono.ttf
)
