#include "fulltextareaengine.h"
#include <QPainter>
#include <QFile>
#include <QDebug>
#include <QtMath>
#include <QFontMetrics>

FileIndexerThread::FileIndexerThread(const QString &filePath, QObject *parent)
    : QThread(parent), m_filePath(filePath) {}

void FileIndexerThread::run() {
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit errorOccurred("Failed to open file: " + m_filePath);
        return;
    }

    // Auto-detect line endings
    char buf[4096];
    const qint64 readBytes = file.peek(buf, sizeof(buf));
    auto ending = LineEnding::LF;
    if (readBytes > 0) {
        bool hasCR = false;
        bool hasLF = false;
        for (int i = 0; i < readBytes - 1; ++i) {
            if (buf[i] == '\r' && buf[i+1] == '\n') {
                ending = LineEnding::CRLF;
                break;
            }
            if (buf[i] == '\n') hasLF = true;
            if (buf[i] == '\r') hasCR = true;
        }
        if (ending != LineEnding::CRLF) {
            // if (readBytes > 0 && buf[readBytes-1] == '\n') hasLF = true;
            // if (readBytes > 0 && buf[readBytes-1] == '\r') hasCR = true;
            if (buf[readBytes-1] == '\n') hasLF = true;
            if (buf[readBytes-1] == '\r') hasCR = true;
            if (hasLF) ending = LineEnding::LF;
            else if (hasCR) ending = LineEnding::CR;
        }
    }

    QList<qint64> offsets;
    offsets.append(0); // Line 0 start offset

    constexpr int bufferSize = 65536;
    QByteArray buffer(bufferSize, 0);
    qint64 bytesRead = 0;
    qint64 totalOffset = 0;
    int maxLineLength = 0;
    qint64 lastLineStart = 0;

    while (!isInterruptionRequested() && (bytesRead = file.read(buffer.data(), bufferSize)) > 0) {
        const char *data = buffer.constData();
        for (int i = 0; i < bytesRead; ++i) {
            bool isLineEnd = false;
            int lineEndLen = 1;
            if (ending == LineEnding::CR) {
                if (data[i] == '\r') {
                    isLineEnd = true;
                }
            } else { // LF or CRLF
                if (data[i] == '\n') {
                    isLineEnd = true;
                    if (ending == LineEnding::CRLF) {
                        lineEndLen = 2;
                    }
                }
            }

            if (isLineEnd) {
                const qint64 lineEndOffset = totalOffset + i;
                const qint64 printableLength = qMax(0LL, lineEndOffset - lastLineStart - (lineEndLen - 1));
                if (printableLength > maxLineLength) {
                    maxLineLength = printableLength;
                }
                offsets.append(lineEndOffset + 1);
                lastLineStart = lineEndOffset + 1;
            }
        }
        totalOffset += bytesRead;
    }

    // Handle last line if it doesn't end with a newline delimiter
    if (!isInterruptionRequested()) {
        const qint64 printableLength = totalOffset - lastLineStart;
        if (printableLength > maxLineLength) {
            maxLineLength = printableLength;
        }
        emit indexingFinished(offsets, maxLineLength, ending);
    }
}

FullTextAreaEngine::FullTextAreaEngine(QQuickItem *parent)
    : QQuickPaintedItem(parent) {
    setFlag(ItemHasContents, true);
    m_document.setUndoRedoEnabled(false);

    QTextOption option = m_document.defaultTextOption();
    option.setWrapMode(QTextOption::NoWrap);
    m_document.setDefaultTextOption(option);
}

QString FullTextAreaEngine::filePath() const {
    return m_filePath;
}

void FullTextAreaEngine::setFilePath(const QString &path) {
    if (m_filePath == path) return;
    m_filePath = path;
    emit filePathChanged();
    startIndexing();
}

bool FullTextAreaEngine::isIndexing() const {
    return m_isIndexing;
}

void FullTextAreaEngine::startIndexing() {
    if (m_indexerThread) {
        m_indexerThread->requestInterruption();
        m_indexerThread->disconnect(this);
        m_indexerThread = nullptr;
    }

    if (m_filePath.isEmpty()) {
        m_lineOffsets.clear();
        m_contentHeight = 0;
        m_contentWidth = 0;
        m_isIndexing = false;
        emit isIndexingChanged();
        emit contentSizeChanged();
        update();
        return;
    }

    m_isIndexing = true;
    emit isIndexingChanged();

    m_indexerThread = new FileIndexerThread(m_filePath, nullptr);
    connect(m_indexerThread, &FileIndexerThread::indexingFinished, this, &FullTextAreaEngine::onIndexingFinished);
    connect(m_indexerThread, &FileIndexerThread::errorOccurred, this, &FullTextAreaEngine::onIndexingError);
    connect(m_indexerThread, &QThread::finished, m_indexerThread, &QObject::deleteLater);

    m_indexerThread->start();
}

void FullTextAreaEngine::onIndexingFinished(const QList<qint64> &offsets, int maxLineLength, LineEnding ending) {
    m_lineOffsets = offsets;
    m_maxLineLength = maxLineLength;
    m_lineEnding = ending;

    m_font = QFont("JetBrains Mono", 12);
    // m_font = QFont("Inter", 14);
    const QFontMetrics metrics(m_font);
    m_lineHeight = metrics.lineSpacing();

    m_contentHeight = static_cast<qreal>(m_lineOffsets.size()) * m_lineHeight + 12;
    m_contentWidth = m_maxLineLength * metrics.averageCharWidth() + 40;

    m_isIndexing = false;
    m_indexerThread = nullptr;

    emit isIndexingChanged();
    emit contentSizeChanged();

    m_loadedStartLine = -1;
    m_loadedEndLine = -1;
    updateVisibleChunk();
}

void FullTextAreaEngine::onIndexingError(const QString &errorMessage) {
    qWarning() << "Indexing error:" << errorMessage;
    m_isIndexing = false;
    m_indexerThread = nullptr;
    emit isIndexingChanged();

    m_lineOffsets.clear();
    m_contentHeight = 0;
    m_contentWidth = 0;
    emit contentSizeChanged();
    update();
}

void FullTextAreaEngine::updateVisibleChunk() {
    if (m_lineOffsets.isEmpty() || m_filePath.isEmpty() || m_isIndexing) return;

    int totalLines = m_lineOffsets.size();
    int firstVisibleLine = qFloor(m_scrollY / m_lineHeight);
    int lastVisibleLine = qCeil((m_scrollY + height()) / m_lineHeight);

    const int buffer = 20;
    int startLine = qMax(0, firstVisibleLine - buffer);
    int endLine = qMin(totalLines - 1, lastVisibleLine + buffer);

    if (m_loadedStartLine <= startLine && m_loadedEndLine >= endLine && m_loadedStartLine != -1) {
        return; 
    }

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    qint64 startOffset = m_lineOffsets[startLine];
    qint64 endOffset = (endLine + 1 < totalLines) ? m_lineOffsets[endLine + 1] : file.size();

    if (!file.seek(startOffset)) return;

    QByteArray chunkData = file.read(endOffset - startOffset);
    QString textChunk = QString::fromUtf8(chunkData);

    m_document.setPlainText(textChunk);

    QTextCursor cursor(&m_document);
    cursor.select(QTextCursor::Document);
    QTextCharFormat format;
    format.setForeground(QColor("#1a1b1f"));
    format.setFont(m_font);
    cursor.mergeCharFormat(format);

    m_loadedStartLine = startLine;
    m_loadedEndLine = endLine;

    update();
}

void FullTextAreaEngine::setText(const QString &text) {
    if (!m_tempFile) {
        m_tempFile = QSharedPointer<QTemporaryFile>::create();
        m_tempFile->setAutoRemove(true);
    }
    if (m_tempFile->open()) {
        m_tempFile->resize(0);
        m_tempFile->write(text.toUtf8());
        m_tempFile->close();
        setFilePath(m_tempFile->fileName());
    }
}

qreal FullTextAreaEngine::scrollX() const {
    return m_scrollX;
}

void FullTextAreaEngine::setScrollX(qreal x) {
    if (m_scrollX != x) {
        m_scrollX = x;
        emit scrollXChanged();
        update();
    }
}

qreal FullTextAreaEngine::scrollY() const {
    return m_scrollY;
}

void FullTextAreaEngine::setScrollY(qreal y) {
    if (m_scrollY != y) {
        m_scrollY = y;
        emit scrollYChanged();
        updateVisibleChunk();
        update();
    }
}

qreal FullTextAreaEngine::contentWidth() const {
    return m_contentWidth;
}

qreal FullTextAreaEngine::contentHeight() const {
    return m_contentHeight;
}

void FullTextAreaEngine::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) {
    QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);
    if (newGeometry.height() != oldGeometry.height()) {
        updateVisibleChunk();
    }
}

void FullTextAreaEngine::paint(QPainter *painter) {
    if (!painter || m_loadedStartLine == -1 || m_isIndexing) return;

    const qreal chunkOffset = m_loadedStartLine * m_lineHeight;

    painter->save();
    painter->translate(-m_scrollX, chunkOffset - m_scrollY);

    const QRectF docClipRect(m_scrollX, m_scrollY - chunkOffset, width(), height());
    m_document.drawContents(painter, docClipRect);
    painter->restore();
}

