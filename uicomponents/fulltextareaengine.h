#ifndef FSCLEANER_FULLTEXTAREAENGINE_H
#define FSCLEANER_FULLTEXTAREAENGINE_H

#include <QQuickPaintedItem>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QThread>
#include <QFile>
#include <QSharedPointer>
#include <QTemporaryFile>

enum class LineEnding {
    LF,
    CRLF,
    CR
};

class FileIndexerThread : public QThread {
    Q_OBJECT
public:
    explicit FileIndexerThread(const QString &filePath, QObject *parent = nullptr);
protected:
    void run() override;

signals:
    void indexingFinished(const QList<qint64> &offsets, int maxLineLength, LineEnding ending);
    void errorOccurred(const QString &errorMessage);

private:
    QString m_filePath;
};

class FullTextAreaEngine : public QQuickPaintedItem {
    Q_OBJECT

    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(bool isIndexing READ isIndexing NOTIFY isIndexingChanged)

    // Properties exposed to QML for viewport scrolling sync
    Q_PROPERTY(qreal scrollX READ scrollX WRITE setScrollX NOTIFY scrollXChanged)
    Q_PROPERTY(qreal scrollY READ scrollY WRITE setScrollY NOTIFY scrollYChanged)

    // Properties to tell QML Flickable how large the total content area is
    Q_PROPERTY(qreal contentWidth READ contentWidth NOTIFY contentSizeChanged)
    Q_PROPERTY(qreal contentHeight READ contentHeight NOTIFY contentSizeChanged)

    QML_ELEMENT

public:
    explicit FullTextAreaEngine(QQuickItem *parent = nullptr);
    ~FullTextAreaEngine() override = default;

    // Overridden painting function from QQuickPaintedItem
    void paint(QPainter *painter) override;

    // QML Property Getters and Setters
    QString filePath() const;
    void setFilePath(const QString &path);

    bool isIndexing() const;

    qreal scrollX() const;
    void setScrollX(qreal x);

    qreal scrollY() const;
    void setScrollY(qreal y);

    qreal contentWidth() const;
    qreal contentHeight() const;

    // Invokable method to load massive text blocks asynchronously or safely
    Q_INVOKABLE void setText(const QString &text);

signals:
    void filePathChanged();
    void isIndexingChanged();
    void scrollXChanged();
    void scrollYChanged();
    void contentSizeChanged();

protected:
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private slots:
    void onIndexingFinished(const QList<qint64> &offsets, int maxLineLength, LineEnding ending);
    void onIndexingError(const QString &errorMessage);

private:
    void startIndexing();
    void updateVisibleChunk();

    QString m_filePath;
    bool m_isIndexing = false;
    FileIndexerThread *m_indexerThread = nullptr;

    QList<qint64> m_lineOffsets;
    LineEnding m_lineEnding = LineEnding::LF;
    int m_maxLineLength = 0;

    QTextDocument m_document;
    QFont m_font;
    qreal m_lineHeight = 20.0;
    qreal m_contentWidth = 0.0;
    qreal m_contentHeight = 0.0;

    int m_loadedStartLine = -1;
    int m_loadedEndLine = -1;

    qreal m_scrollX = 0;
    qreal m_scrollY = 0;

    QSharedPointer<QTemporaryFile> m_tempFile;
};

#endif //FSCLEANER_FULLTEXTAREAENGINE_H

