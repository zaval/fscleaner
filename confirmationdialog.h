//
// Created by Dmytrii Zavalnyi on 27.11.2025..
//

#ifndef FSCLEANER_CONFIRMATIONDIALOG_H
#define FSCLEANER_CONFIRMATIONDIALOG_H
#include <QObject>
#include <QQmlEngine>


class ConfirmationDialog: public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString informativeText READ informativeText WRITE setInformativeText NOTIFY informativeTextChanged)

private:
    QString m_informativeText;
    QString m_text;

public:
    explicit ConfirmationDialog(QObject *parent = nullptr);
    Q_INVOKABLE void open();

    QString informativeText() const { return m_informativeText; }

    void setInformativeText(const QString &newInformativeText) {
        if (m_informativeText == newInformativeText)
            return;
        m_informativeText = newInformativeText;
        emit informativeTextChanged();
    }

    QString text() const { return m_text; }

    void setText(const QString &newText) {
        if (m_text == newText)
            return;
        m_text = newText;
        emit textChanged();
    }

signals:
    void informativeTextChanged();
    void textChanged();
    void accepted();
    void rejected();


};




#endif //FSCLEANER_CONFIRMATIONDIALOG_H
