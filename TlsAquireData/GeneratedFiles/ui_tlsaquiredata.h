/********************************************************************************
** Form generated from reading UI file 'tlsaquiredata.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TLSAQUIREDATA_H
#define UI_TLSAQUIREDATA_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TlsAquireDataClass
{
public:
    QPushButton *btnGetHisData;
    QLabel *lab_information;

    void setupUi(QWidget *TlsAquireDataClass)
    {
        if (TlsAquireDataClass->objectName().isEmpty())
            TlsAquireDataClass->setObjectName(QStringLiteral("TlsAquireDataClass"));
        TlsAquireDataClass->resize(600, 400);
        btnGetHisData = new QPushButton(TlsAquireDataClass);
        btnGetHisData->setObjectName(QStringLiteral("btnGetHisData"));
        btnGetHisData->setGeometry(QRect(430, 60, 75, 23));
        lab_information = new QLabel(TlsAquireDataClass);
        lab_information->setObjectName(QStringLiteral("lab_information"));
        lab_information->setGeometry(QRect(80, 200, 381, 71));
        QFont font;
        font.setPointSize(17);
        lab_information->setFont(font);

        retranslateUi(TlsAquireDataClass);

        QMetaObject::connectSlotsByName(TlsAquireDataClass);
    } // setupUi

    void retranslateUi(QWidget *TlsAquireDataClass)
    {
        TlsAquireDataClass->setWindowTitle(QApplication::translate("TlsAquireDataClass", "TlsAquireData", 0));
        btnGetHisData->setText(QApplication::translate("TlsAquireDataClass", "GetHisData", 0));
        lab_information->setText(QApplication::translate("TlsAquireDataClass", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class TlsAquireDataClass: public Ui_TlsAquireDataClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TLSAQUIREDATA_H
