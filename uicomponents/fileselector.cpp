#include "fileselector.h"
#include <QFileDialog>

FileSelector::FileSelector(QObject *parent):
    QObject(parent),
    m_filter(""),
    m_caption(""),
    m_folder("")
{}

QString FileSelector::caption() const {
    return m_caption;
}

void FileSelector::setCaption(const QString &newCaption) {
    if (m_caption == newCaption)
        return;
    m_caption = newCaption;
    emit captionChanged();

}

QString FileSelector::folder() const {
    return m_folder;
}

void FileSelector::setFolder(const QString &newFolder) {
    if (m_folder == newFolder)
        return;
    m_folder = newFolder;
    emit folderChanged();

}

QString FileSelector::filter() const {
    return m_filter;
}

void FileSelector::setFilter(const QString &newFilter) {
    if (m_filter == newFilter)
        return;
    m_filter = newFilter;
    emit filterChanged();

}

void FileSelector::showFolderSelector() {
    if (const auto dir = QFileDialog::getExistingDirectory(nullptr, m_caption, m_folder); !dir.isEmpty())
        emit accepted(dir);
}

