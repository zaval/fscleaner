//
// Created by Dmytrii Zavalnyi on 27.11.2025..
//

#include "confirmationdialog.h"
#include <QMessageBox>

ConfirmationDialog::ConfirmationDialog(QObject *parent): QObject(parent) {
}

void ConfirmationDialog::open() {
    const auto res = QMessageBox::question(nullptr, m_text, m_informativeText, QMessageBox::Yes | QMessageBox::No);
    if (res == QMessageBox::Yes) {
        emit accepted();
    } else {
        emit rejected();
    }
}
