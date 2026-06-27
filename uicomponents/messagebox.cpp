#include "messagebox.h"

#include <QMessageBox>
#include <QInputDialog>

MessageBox::MessageBox(QObject *parent) : QObject(parent), m_buttons(NoButton) {
}

MessageBox::StandardButton MessageBox::buttons() const {
    return m_buttons;
}

void MessageBox::setButtons(const StandardButton &newButtons) {
    if (m_buttons == newButtons)
        return;
    m_buttons = newButtons;
    emit buttonsChanged();

}

QString MessageBox::text() const {
    return m_text;
}

void MessageBox::setText(const QString &newText) {
    if (m_text == newText)
        return;
    m_text = newText;
    emit textChanged();

}

QString MessageBox::title() const {
    return m_title;
}

void MessageBox::setTitle(const QString &newTitle) {
    if (m_title == newTitle)
        return;
    m_title = newTitle;
    emit titleChanged();

}

QString MessageBox::textPlaceholder() const {
    return m_textPlaceholder;
}

void MessageBox::setTextPlaceholder(const QString &newTextPlaceholder) {
    if (m_textPlaceholder == newTextPlaceholder)
        return;
    m_textPlaceholder = newTextPlaceholder;
    emit textPlaceholderChanged();

}

void MessageBox::showCritical() {
    const auto res = QMessageBox::critical(nullptr, m_title, m_text, static_cast<QMessageBox::StandardButtons>(m_buttons));
    processDialogResult(static_cast<StandardButton>(res));
}

void MessageBox::showInformation() {
    const auto res = QMessageBox::information(nullptr, m_title, m_text, static_cast<QMessageBox::StandardButtons>(m_buttons));
    processDialogResult(static_cast<StandardButton>(res));
}

void MessageBox::showQuestion() {
    const auto res = QMessageBox::question(nullptr, m_title, m_text, static_cast<QMessageBox::StandardButtons>(m_buttons));
    processDialogResult(static_cast<StandardButton>(res));
}

void MessageBox::showWarning() {
    const auto res = QMessageBox::warning(nullptr, m_title, m_text, static_cast<QMessageBox::StandardButtons>(m_buttons));
    processDialogResult(static_cast<StandardButton>(res));
}

void MessageBox::showTextInput() {
    bool ok;
    const auto res =  QInputDialog::getText(nullptr, m_title, m_text, QLineEdit::Normal, m_textPlaceholder, &ok);

    if (ok) {
        emit textInputAccepted(res);
    }
}

void MessageBox::processDialogResult(const StandardButton &button) {

    switch (button) {
        case Ok:
        case Save:
        case SaveAll:
        case Open:
        case Yes:
        case YesToAll:
        case Retry:
        case Close:
        case Apply:
            emit accepted();
            break;
        case Cancel:
        case No:
        case NoToAll:
        case Abort:
        case Discard:
        case Reset:
            emit rejected();
            break;
        default:
            break;
    }

}
