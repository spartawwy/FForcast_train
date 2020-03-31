/********************************************************************************
** Form generated from reading UI file 'cfgtrain.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CFGTRAIN_H
#define UI_CFGTRAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CfgTrainForm
{
public:
    QLabel *label;
    QDoubleSpinBox *dbspbBegCapital;
    QDoubleSpinBox *dbspbFeeOpen;
    QLabel *label_9;
    QLabel *label_3;
    QDoubleSpinBox *dbspbFeeOpen_2;

    void setupUi(QWidget *CfgTrainForm)
    {
        if (CfgTrainForm->objectName().isEmpty())
            CfgTrainForm->setObjectName(QStringLiteral("CfgTrainForm"));
        CfgTrainForm->resize(400, 300);
        label = new QLabel(CfgTrainForm);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(60, 90, 61, 21));
        QFont font;
        font.setPointSize(10);
        label->setFont(font);
        dbspbBegCapital = new QDoubleSpinBox(CfgTrainForm);
        dbspbBegCapital->setObjectName(QStringLiteral("dbspbBegCapital"));
        dbspbBegCapital->setGeometry(QRect(130, 90, 111, 31));
        dbspbBegCapital->setMaximum(12000);
        dbspbBegCapital->setValue(12000);
        dbspbFeeOpen = new QDoubleSpinBox(CfgTrainForm);
        dbspbFeeOpen->setObjectName(QStringLiteral("dbspbFeeOpen"));
        dbspbFeeOpen->setGeometry(QRect(130, 140, 81, 31));
        dbspbFeeOpen->setDecimals(4);
        dbspbFeeOpen->setMaximum(500);
        dbspbFeeOpen->setSingleStep(100);
        dbspbFeeOpen->setValue(100);
        label_9 = new QLabel(CfgTrainForm);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(240, 140, 71, 21));
        label_9->setFont(font);
        label_3 = new QLabel(CfgTrainForm);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(50, 140, 71, 21));
        label_3->setFont(font);
        dbspbFeeOpen_2 = new QDoubleSpinBox(CfgTrainForm);
        dbspbFeeOpen_2->setObjectName(QStringLiteral("dbspbFeeOpen_2"));
        dbspbFeeOpen_2->setGeometry(QRect(320, 140, 81, 31));
        dbspbFeeOpen_2->setDecimals(4);
        dbspbFeeOpen_2->setMaximum(500);
        dbspbFeeOpen_2->setSingleStep(100);
        dbspbFeeOpen_2->setValue(300);

        retranslateUi(CfgTrainForm);

        QMetaObject::connectSlotsByName(CfgTrainForm);
    } // setupUi

    void retranslateUi(QWidget *CfgTrainForm)
    {
        CfgTrainForm->setWindowTitle(QApplication::translate("CfgTrainForm", "Form", 0));
        label->setText(QApplication::translate("CfgTrainForm", "\345\210\235\345\247\213\350\265\204\351\207\221:", 0));
        label_9->setText(QApplication::translate("CfgTrainForm", "\345\271\263\344\273\223\346\211\213\347\273\255\350\264\271:", 0));
        label_3->setText(QApplication::translate("CfgTrainForm", "\345\274\200\344\273\223\346\211\213\347\273\255\350\264\271:", 0));
    } // retranslateUi

};

namespace Ui {
    class CfgTrainForm: public Ui_CfgTrainForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CFGTRAIN_H
