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
#include <QtWidgets/QComboBox>
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
    QComboBox *cmb_k_type;
    QPushButton *btnGetRecentHisData;
    QPushButton *btnGetDataFromFile;

    void setupUi(QWidget *TlsAquireDataClass)
    {
        if (TlsAquireDataClass->objectName().isEmpty())
            TlsAquireDataClass->setObjectName(QStringLiteral("TlsAquireDataClass"));
        TlsAquireDataClass->resize(600, 400);
        btnGetHisData = new QPushButton(TlsAquireDataClass);
        btnGetHisData->setObjectName(QStringLiteral("btnGetHisData"));
        btnGetHisData->setGeometry(QRect(370, 40, 101, 23));
        lab_information = new QLabel(TlsAquireDataClass);
        lab_information->setObjectName(QStringLiteral("lab_information"));
        lab_information->setGeometry(QRect(80, 200, 381, 71));
        QFont font;
        font.setPointSize(17);
        lab_information->setFont(font);
        cmb_k_type = new QComboBox(TlsAquireDataClass);
        cmb_k_type->setObjectName(QStringLiteral("cmb_k_type"));
        cmb_k_type->setGeometry(QRect(80, 60, 261, 31));
        btnGetRecentHisData = new QPushButton(TlsAquireDataClass);
        btnGetRecentHisData->setObjectName(QStringLiteral("btnGetRecentHisData"));
        btnGetRecentHisData->setGeometry(QRect(370, 80, 101, 23));
        btnGetDataFromFile = new QPushButton(TlsAquireDataClass);
        btnGetDataFromFile->setObjectName(QStringLiteral("btnGetDataFromFile"));
        btnGetDataFromFile->setGeometry(QRect(370, 160, 101, 23));

        retranslateUi(TlsAquireDataClass);

        QMetaObject::connectSlotsByName(TlsAquireDataClass);
    } // setupUi

    void retranslateUi(QWidget *TlsAquireDataClass)
    {
        TlsAquireDataClass->setWindowTitle(QApplication::translate("TlsAquireDataClass", "TlsAquireData", 0));
        btnGetHisData->setText(QApplication::translate("TlsAquireDataClass", "GetAllHisData", 0));
        lab_information->setText(QApplication::translate("TlsAquireDataClass", "TextLabel", 0));
        btnGetRecentHisData->setText(QApplication::translate("TlsAquireDataClass", "GeRecentHisData", 0));
        btnGetDataFromFile->setText(QApplication::translate("TlsAquireDataClass", "GetDataFromFile", 0));
    } // retranslateUi

};

namespace Ui {
    class TlsAquireDataClass: public Ui_TlsAquireDataClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TLSAQUIREDATA_H
