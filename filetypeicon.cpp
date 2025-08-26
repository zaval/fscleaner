//
// Created by Dmytrii Zavalnyi on 10.08.2025..
//

#include "filetypeicon.h"
#include <QFileInfo>


QString FileTypeIcon::getIcon(const QString &filePath) const {
    QFileInfo file(filePath);
    const auto extension = file.suffix().toLower();
    if (m_icons.contains(extension)) {
        return m_icons[extension];
    }
    return m_icons[""];

}
