/********************************************************************************
** Form generated from reading UI file 'traindlg.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRAINDLG_H
#define UI_TRAINDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TrainDlgForm
{
public:
    QLabel *label;
    QLabel *label_2;
    QDoubleSpinBox *dbspbBegCapital;
    QPushButton *pbtnBuy;
    QPushButton *pbtnSell;
    QPushButton *pbtnNextK;
    QLabel *label_3;
    QDoubleSpinBox *dbspbFeeOpen;
    QTableView *table_view_record;
    QLabel *label_5;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *pbtnStop;
    QPushButton *pbtnStart;
    QLabel *label_7;
    QLineEdit *le_long_pos;
    QPlainTextEdit *plain_te_record;
    QLabel *label_9;
    QDoubleSpinBox *dbspbFeeOpen_2;
    QLabel *lab_status;
    QLabel *label_10;
    QLineEdit *le_short_pos;
    QScrollBar *hScrollBar_TrainTimeRange;
    QLabel *lab_start_date;
    QPushButton *pbtnNextStep;
    QTabWidget *tabwid_main;
    QWidget *tab_trade;
    QLabel *lab_price;
    QLabel *lab_v;
    QDoubleSpinBox *dbspb_price;
    QSpinBox *spb_order_num;
    QLabel *lab_h_line;
    QRadioButton *radio_postion_o;
    QRadioButton *radio_position_c;
    QPushButton *pbtn_buy;
    QPushButton *pbtn_sell;
    QTabWidget *tab_detail;
    QWidget *tab_position;
    QTableView *table_view_position;
    QPushButton *pbtn_market_price_c;
    QPushButton *pbtn_clear_close;
    QWidget *tab_fill;
    QWidget *tab_condition_order;
    QCheckBox *checkb_follow_market;
    QWidget *tab_account;
    QLabel *label_8;
    QLineEdit *le_cur_capital;
    QLabel *label_6;
    QLabel *lab_assets;
    QLabel *lab_quote_title;
    QLabel *lab_quote;
    QGroupBox *group_box_capital;
    QLabel *label_4;
    QLabel *label_capital_available;
    QLabel *label_capital;
    QLabel *label_11;
    QLabel *label_12;
    QLabel *label_float_profit;
    QPushButton *pbtn_config;

    void setupUi(QWidget *TrainDlgForm)
    {
        if (TrainDlgForm->objectName().isEmpty())
            TrainDlgForm->setObjectName(QStringLiteral("TrainDlgForm"));
        TrainDlgForm->resize(626, 609);
        label = new QLabel(TrainDlgForm);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 350, 61, 21));
        QFont font;
        font.setPointSize(10);
        label->setFont(font);
        label_2 = new QLabel(TrainDlgForm);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(0, 30, 61, 21));
        label_2->setFont(font);
        dbspbBegCapital = new QDoubleSpinBox(TrainDlgForm);
        dbspbBegCapital->setObjectName(QStringLiteral("dbspbBegCapital"));
        dbspbBegCapital->setGeometry(QRect(80, 350, 111, 31));
        dbspbBegCapital->setMaximum(12000);
        dbspbBegCapital->setValue(12000);
        pbtnBuy = new QPushButton(TrainDlgForm);
        pbtnBuy->setObjectName(QStringLiteral("pbtnBuy"));
        pbtnBuy->setGeometry(QRect(520, 390, 75, 23));
        pbtnBuy->setFont(font);
        pbtnSell = new QPushButton(TrainDlgForm);
        pbtnSell->setObjectName(QStringLiteral("pbtnSell"));
        pbtnSell->setGeometry(QRect(520, 420, 75, 23));
        pbtnSell->setFont(font);
        pbtnNextK = new QPushButton(TrainDlgForm);
        pbtnNextK->setObjectName(QStringLiteral("pbtnNextK"));
        pbtnNextK->setGeometry(QRect(520, 310, 61, 23));
        pbtnNextK->setFont(font);
        label_3 = new QLabel(TrainDlgForm);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(210, 350, 71, 21));
        label_3->setFont(font);
        dbspbFeeOpen = new QDoubleSpinBox(TrainDlgForm);
        dbspbFeeOpen->setObjectName(QStringLiteral("dbspbFeeOpen"));
        dbspbFeeOpen->setGeometry(QRect(290, 350, 81, 31));
        dbspbFeeOpen->setDecimals(4);
        dbspbFeeOpen->setMaximum(500);
        dbspbFeeOpen->setSingleStep(100);
        dbspbFeeOpen->setValue(100);
        table_view_record = new QTableView(TrainDlgForm);
        table_view_record->setObjectName(QStringLiteral("table_view_record"));
        table_view_record->setGeometry(QRect(10, 560, 601, 20));
        label_5 = new QLabel(TrainDlgForm);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 430, 61, 21));
        label_5->setFont(font);
        layoutWidget = new QWidget(TrainDlgForm);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(370, 30, 158, 28));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        pbtnStop = new QPushButton(layoutWidget);
        pbtnStop->setObjectName(QStringLiteral("pbtnStop"));
        pbtnStop->setFont(font);

        horizontalLayout->addWidget(pbtnStop);

        pbtnStart = new QPushButton(layoutWidget);
        pbtnStart->setObjectName(QStringLiteral("pbtnStart"));
        pbtnStart->setFont(font);

        horizontalLayout->addWidget(pbtnStart);

        label_7 = new QLabel(TrainDlgForm);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(30, 390, 61, 21));
        label_7->setFont(font);
        le_long_pos = new QLineEdit(TrainDlgForm);
        le_long_pos->setObjectName(QStringLiteral("le_long_pos"));
        le_long_pos->setGeometry(QRect(100, 390, 111, 31));
        plain_te_record = new QPlainTextEdit(TrainDlgForm);
        plain_te_record->setObjectName(QStringLiteral("plain_te_record"));
        plain_te_record->setGeometry(QRect(10, 460, 601, 101));
        label_9 = new QLabel(TrainDlgForm);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(400, 350, 71, 21));
        label_9->setFont(font);
        dbspbFeeOpen_2 = new QDoubleSpinBox(TrainDlgForm);
        dbspbFeeOpen_2->setObjectName(QStringLiteral("dbspbFeeOpen_2"));
        dbspbFeeOpen_2->setGeometry(QRect(480, 350, 81, 31));
        dbspbFeeOpen_2->setDecimals(4);
        dbspbFeeOpen_2->setMaximum(500);
        dbspbFeeOpen_2->setSingleStep(100);
        dbspbFeeOpen_2->setValue(300);
        lab_status = new QLabel(TrainDlgForm);
        lab_status->setObjectName(QStringLiteral("lab_status"));
        lab_status->setGeometry(QRect(10, 580, 601, 20));
        lab_status->setFont(font);
        label_10 = new QLabel(TrainDlgForm);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(240, 390, 61, 21));
        label_10->setFont(font);
        le_short_pos = new QLineEdit(TrainDlgForm);
        le_short_pos->setObjectName(QStringLiteral("le_short_pos"));
        le_short_pos->setGeometry(QRect(310, 390, 111, 31));
        hScrollBar_TrainTimeRange = new QScrollBar(TrainDlgForm);
        hScrollBar_TrainTimeRange->setObjectName(QStringLiteral("hScrollBar_TrainTimeRange"));
        hScrollBar_TrainTimeRange->setGeometry(QRect(60, 30, 231, 21));
        hScrollBar_TrainTimeRange->setOrientation(Qt::Horizontal);
        lab_start_date = new QLabel(TrainDlgForm);
        lab_start_date->setObjectName(QStringLiteral("lab_start_date"));
        lab_start_date->setGeometry(QRect(290, 30, 71, 21));
        QFont font1;
        font1.setPointSize(12);
        lab_start_date->setFont(font1);
        lab_start_date->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        pbtnNextStep = new QPushButton(TrainDlgForm);
        pbtnNextStep->setObjectName(QStringLiteral("pbtnNextStep"));
        pbtnNextStep->setGeometry(QRect(450, 0, 75, 23));
        pbtnNextStep->setFont(font);
        tabwid_main = new QTabWidget(TrainDlgForm);
        tabwid_main->setObjectName(QStringLiteral("tabwid_main"));
        tabwid_main->setGeometry(QRect(0, 100, 621, 241));
        tabwid_main->setTabPosition(QTabWidget::West);
        tab_trade = new QWidget();
        tab_trade->setObjectName(QStringLiteral("tab_trade"));
        lab_price = new QLabel(tab_trade);
        lab_price->setObjectName(QStringLiteral("lab_price"));
        lab_price->setGeometry(QRect(0, 20, 41, 16));
        lab_v = new QLabel(tab_trade);
        lab_v->setObjectName(QStringLiteral("lab_v"));
        lab_v->setGeometry(QRect(0, 50, 41, 16));
        dbspb_price = new QDoubleSpinBox(tab_trade);
        dbspb_price->setObjectName(QStringLiteral("dbspb_price"));
        dbspb_price->setGeometry(QRect(40, 20, 62, 22));
        dbspb_price->setMinimum(0.1);
        dbspb_price->setMaximum(999.99);
        spb_order_num = new QSpinBox(tab_trade);
        spb_order_num->setObjectName(QStringLiteral("spb_order_num"));
        spb_order_num->setGeometry(QRect(40, 50, 61, 22));
        spb_order_num->setMinimum(1);
        spb_order_num->setMaximum(10000);
        lab_h_line = new QLabel(tab_trade);
        lab_h_line->setObjectName(QStringLiteral("lab_h_line"));
        lab_h_line->setGeometry(QRect(0, 90, 171, 16));
        radio_postion_o = new QRadioButton(tab_trade);
        radio_postion_o->setObjectName(QStringLiteral("radio_postion_o"));
        radio_postion_o->setGeometry(QRect(0, 110, 41, 16));
        radio_postion_o->setChecked(true);
        radio_position_c = new QRadioButton(tab_trade);
        radio_position_c->setObjectName(QStringLiteral("radio_position_c"));
        radio_position_c->setGeometry(QRect(0, 130, 41, 16));
        pbtn_buy = new QPushButton(tab_trade);
        pbtn_buy->setObjectName(QStringLiteral("pbtn_buy"));
        pbtn_buy->setGeometry(QRect(50, 110, 51, 23));
        pbtn_buy->setFont(font);
        pbtn_sell = new QPushButton(tab_trade);
        pbtn_sell->setObjectName(QStringLiteral("pbtn_sell"));
        pbtn_sell->setGeometry(QRect(110, 110, 51, 23));
        pbtn_sell->setFont(font);
        tab_detail = new QTabWidget(tab_trade);
        tab_detail->setObjectName(QStringLiteral("tab_detail"));
        tab_detail->setGeometry(QRect(170, 0, 421, 231));
        tab_detail->setTabPosition(QTabWidget::South);
        tab_position = new QWidget();
        tab_position->setObjectName(QStringLiteral("tab_position"));
        table_view_position = new QTableView(tab_position);
        table_view_position->setObjectName(QStringLiteral("table_view_position"));
        table_view_position->setGeometry(QRect(0, 30, 401, 171));
        pbtn_market_price_c = new QPushButton(tab_position);
        pbtn_market_price_c->setObjectName(QStringLiteral("pbtn_market_price_c"));
        pbtn_market_price_c->setGeometry(QRect(10, 0, 75, 23));
        pbtn_market_price_c->setFont(font);
        pbtn_clear_close = new QPushButton(tab_position);
        pbtn_clear_close->setObjectName(QStringLiteral("pbtn_clear_close"));
        pbtn_clear_close->setGeometry(QRect(90, 0, 75, 23));
        pbtn_clear_close->setFont(font);
        tab_detail->addTab(tab_position, QString());
        tab_fill = new QWidget();
        tab_fill->setObjectName(QStringLiteral("tab_fill"));
        tab_detail->addTab(tab_fill, QString());
        tab_condition_order = new QWidget();
        tab_condition_order->setObjectName(QStringLiteral("tab_condition_order"));
        tab_detail->addTab(tab_condition_order, QString());
        checkb_follow_market = new QCheckBox(tab_trade);
        checkb_follow_market->setObjectName(QStringLiteral("checkb_follow_market"));
        checkb_follow_market->setGeometry(QRect(110, 20, 71, 18));
        tabwid_main->addTab(tab_trade, QString());
        tab_account = new QWidget();
        tab_account->setObjectName(QStringLiteral("tab_account"));
        label_8 = new QLabel(tab_account);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(300, 40, 41, 21));
        label_8->setFont(font);
        le_cur_capital = new QLineEdit(tab_account);
        le_cur_capital->setObjectName(QStringLiteral("le_cur_capital"));
        le_cur_capital->setGeometry(QRect(130, 40, 111, 31));
        label_6 = new QLabel(tab_account);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(60, 40, 61, 21));
        label_6->setFont(font);
        lab_assets = new QLabel(tab_account);
        lab_assets->setObjectName(QStringLiteral("lab_assets"));
        lab_assets->setGeometry(QRect(340, 40, 101, 21));
        lab_assets->setFont(font);
        tabwid_main->addTab(tab_account, QString());
        lab_quote_title = new QLabel(TrainDlgForm);
        lab_quote_title->setObjectName(QStringLiteral("lab_quote_title"));
        lab_quote_title->setGeometry(QRect(120, 10, 41, 16));
        lab_quote = new QLabel(TrainDlgForm);
        lab_quote->setObjectName(QStringLiteral("lab_quote"));
        lab_quote->setGeometry(QRect(160, 10, 51, 16));
        QFont font2;
        font2.setBold(true);
        font2.setWeight(75);
        lab_quote->setFont(font2);
        group_box_capital = new QGroupBox(TrainDlgForm);
        group_box_capital->setObjectName(QStringLiteral("group_box_capital"));
        group_box_capital->setGeometry(QRect(10, 60, 581, 41));
        label_4 = new QLabel(group_box_capital);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 10, 51, 16));
        label_capital_available = new QLabel(group_box_capital);
        label_capital_available->setObjectName(QStringLiteral("label_capital_available"));
        label_capital_available->setGeometry(QRect(70, 10, 61, 16));
        label_capital = new QLabel(group_box_capital);
        label_capital->setObjectName(QStringLiteral("label_capital"));
        label_capital->setGeometry(QRect(180, 10, 61, 16));
        label_11 = new QLabel(group_box_capital);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(120, 10, 51, 16));
        label_12 = new QLabel(group_box_capital);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(270, 10, 51, 16));
        label_float_profit = new QLabel(group_box_capital);
        label_float_profit->setObjectName(QStringLiteral("label_float_profit"));
        label_float_profit->setGeometry(QRect(330, 10, 61, 16));
        pbtn_config = new QPushButton(TrainDlgForm);
        pbtn_config->setObjectName(QStringLiteral("pbtn_config"));
        pbtn_config->setGeometry(QRect(300, 0, 75, 23));
        pbtn_config->setFont(font);

        retranslateUi(TrainDlgForm);

        tabwid_main->setCurrentIndex(0);
        tab_detail->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(TrainDlgForm);
    } // setupUi

    void retranslateUi(QWidget *TrainDlgForm)
    {
        TrainDlgForm->setWindowTitle(QApplication::translate("TrainDlgForm", "\350\256\255\347\273\203\346\250\241\345\274\217", 0));
        label->setText(QApplication::translate("TrainDlgForm", "\345\210\235\345\247\213\350\265\204\351\207\221:", 0));
        label_2->setText(QApplication::translate("TrainDlgForm", "\345\274\200\345\247\213\346\227\266\351\227\264:", 0));
        pbtnBuy->setText(QApplication::translate("TrainDlgForm", "\345\274\200", 0));
        pbtnSell->setText(QApplication::translate("TrainDlgForm", "\345\271\263", 0));
        pbtnNextK->setText(QApplication::translate("TrainDlgForm", "\344\270\213\344\270\200\346\240\271=>", 0));
        label_3->setText(QApplication::translate("TrainDlgForm", "\345\274\200\344\273\223\346\211\213\347\273\255\350\264\271:", 0));
        label_5->setText(QApplication::translate("TrainDlgForm", "\346\210\220\344\272\244\350\256\260\345\275\225", 0));
        pbtnStop->setText(QApplication::translate("TrainDlgForm", "\347\273\223\346\235\237", 0));
        pbtnStart->setText(QApplication::translate("TrainDlgForm", "\345\274\200\345\247\213", 0));
        label_7->setText(QApplication::translate("TrainDlgForm", "\345\244\232\345\244\264\345\244\264\345\257\270:", 0));
        label_9->setText(QApplication::translate("TrainDlgForm", "\345\271\263\344\273\223\346\211\213\347\273\255\350\264\271:", 0));
        lab_status->setText(QApplication::translate("TrainDlgForm", "status", 0));
        label_10->setText(QApplication::translate("TrainDlgForm", "\347\251\272\345\244\264\345\244\264\345\257\270:", 0));
        le_short_pos->setText(QString());
        lab_start_date->setText(QApplication::translate("TrainDlgForm", "0", 0));
        pbtnNextStep->setText(QApplication::translate("TrainDlgForm", "\346\255\245\350\277\233", 0));
        lab_price->setText(QApplication::translate("TrainDlgForm", "\344\273\267\346\240\274:", 0));
        lab_v->setText(QApplication::translate("TrainDlgForm", "\346\225\260\351\207\217:", 0));
        lab_h_line->setText(QApplication::translate("TrainDlgForm", "----------------------------------", 0));
        radio_postion_o->setText(QApplication::translate("TrainDlgForm", "\345\274\200\344\273\223", 0));
        radio_position_c->setText(QApplication::translate("TrainDlgForm", "\345\271\263\344\273\223", 0));
        pbtn_buy->setText(QApplication::translate("TrainDlgForm", "\344\271\260", 0));
        pbtn_sell->setText(QApplication::translate("TrainDlgForm", "\345\215\226", 0));
        pbtn_market_price_c->setText(QApplication::translate("TrainDlgForm", "\345\270\202\344\273\267\345\271\263\344\273\223", 0));
        pbtn_clear_close->setText(QApplication::translate("TrainDlgForm", "\345\205\250\351\203\250\345\271\263\344\273\223", 0));
        tab_detail->setTabText(tab_detail->indexOf(tab_position), QApplication::translate("TrainDlgForm", "\346\214\201\344\273\223", 0));
        tab_detail->setTabText(tab_detail->indexOf(tab_fill), QApplication::translate("TrainDlgForm", "\346\210\220\344\272\244\350\256\260\345\275\225", 0));
        tab_detail->setTabText(tab_detail->indexOf(tab_condition_order), QApplication::translate("TrainDlgForm", "\346\235\241\344\273\266\345\215\225", 0));
        checkb_follow_market->setText(QApplication::translate("TrainDlgForm", "\350\267\237\345\270\202", 0));
        tabwid_main->setTabText(tabwid_main->indexOf(tab_trade), QApplication::translate("TrainDlgForm", "\344\272\244\346\230\223", 0));
        label_8->setText(QApplication::translate("TrainDlgForm", "\346\235\203\347\233\212:", 0));
        label_6->setText(QApplication::translate("TrainDlgForm", "\345\217\257\347\224\250\350\265\204\351\207\221:", 0));
        lab_assets->setText(QApplication::translate("TrainDlgForm", "12000", 0));
        tabwid_main->setTabText(tabwid_main->indexOf(tab_account), QApplication::translate("TrainDlgForm", "\350\264\246\346\210\267", 0));
        lab_quote_title->setText(QApplication::translate("TrainDlgForm", "\344\273\267\346\240\274:", 0));
        lab_quote->setText(QApplication::translate("TrainDlgForm", "0.0", 0));
        group_box_capital->setTitle(QString());
        label_4->setText(QApplication::translate("TrainDlgForm", "\345\217\257\347\224\250\350\265\204\351\207\221:", 0));
        label_capital_available->setText(QApplication::translate("TrainDlgForm", "0", 0));
        label_capital->setText(QApplication::translate("TrainDlgForm", "0", 0));
        label_11->setText(QApplication::translate("TrainDlgForm", "\345\212\250\346\200\201\346\235\203\347\233\212:", 0));
        label_12->setText(QApplication::translate("TrainDlgForm", "\346\265\256\345\212\250\347\233\210\344\272\217:", 0));
        label_float_profit->setText(QApplication::translate("TrainDlgForm", "0", 0));
        pbtn_config->setText(QApplication::translate("TrainDlgForm", "\350\256\276\347\275\256", 0));
    } // retranslateUi

};

namespace Ui {
    class TrainDlgForm: public Ui_TrainDlgForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRAINDLG_H
