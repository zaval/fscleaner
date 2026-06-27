#ifndef FSCLEANER_MESSAGEBOX_H
#define FSCLEANER_MESSAGEBOX_H
#include <QObject>
#include <qqmlintegration.h>


class MessageBox: public QObject {
    Q_OBJECT

    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(StandardButton buttons READ buttons WRITE setButtons NOTIFY buttonsChanged)
    Q_PROPERTY(QString textPlaceholder READ textPlaceholder WRITE setTextPlaceholder NOTIFY textPlaceholderChanged)


    QML_ELEMENT
public:
    enum StandardButton {
        NoButton           = 0x00000000,
        Ok                 = 0x00000400,
        Save               = 0x00000800,
        SaveAll            = 0x00001000,
        Open               = 0x00002000,
        Yes                = 0x00004000,
        YesToAll           = 0x00008000,
        No                 = 0x00010000,
        NoToAll            = 0x00020000,
        Abort              = 0x00040000,
        Retry              = 0x00080000,
        Ignore             = 0x00100000,
        Close              = 0x00200000,
        Cancel             = 0x00400000,
        Discard            = 0x00800000,
        Help               = 0x01000000,
        Apply              = 0x02000000,
        Reset              = 0x04000000,
        RestoreDefaults    = 0x08000000,
    };
    Q_ENUM(StandardButton)

    explicit MessageBox(QObject *parent = nullptr);

    StandardButton buttons() const;

    void setButtons(const StandardButton &newButtons);

    QString text() const;

    void setText(const QString &newText);

    QString title() const;

    void setTitle(const QString &newTitle);

    QString textPlaceholder() const;

    void setTextPlaceholder(const QString &newTextPlaceholder);

    Q_INVOKABLE void showCritical();
    Q_INVOKABLE void showInformation();
    Q_INVOKABLE void showQuestion();
    Q_INVOKABLE void showWarning();
    Q_INVOKABLE void showTextInput();

signals:
    void titleChanged();
    void textChanged();
    void buttonsChanged();
    void accepted();
    void rejected();
    void textInputAccepted(const QString &inputText);
    void textPlaceholderChanged();


private:
    StandardButton m_buttons;
    QString m_text;
    QString m_title;
    QString m_textPlaceholder;

    void processDialogResult(const StandardButton &button);
};


#endif //FSCLEANER_MESSAGEBOX_H
