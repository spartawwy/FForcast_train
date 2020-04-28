#include "aquire_data_win.h"
#include <QMessageBox>
//#include <QEvent>
#include <QtGui/QtEvents>
#include <qtimer.h>

#include "aquire_data_app.h"

#include "hq_wrapper_api.h"

int SaveKbarDataCallback(void *para, int kbar_type, T_KbarData *data_ret[], unsigned int size)
{
    AquireDataApp *app = (AquireDataApp*)para;
    app->SaveKbarDatas(kbar_type, data_ret, size);
    return 0;
}

AquireDataWin::AquireDataWin(AquireDataApp *app, QWidget *parent)
    : app_(app)
    , QWidget(parent)
    , timer_(nullptr)
{
    ui.setupUi(this);

    QVariant item_val;
    item_val.setValue(KTYPE_PERIOD_1M);
    ui.cmb_k_type->addItem("1M", item_val);
    item_val.setValue(KTYPE_PERIOD_5M);
    ui.cmb_k_type->addItem("5M", item_val);
    item_val.setValue(KTYPE_PERIOD_15M);
    ui.cmb_k_type->addItem("15M", item_val);
    item_val.setValue(KTYPE_PERIOD_30M);
    ui.cmb_k_type->addItem("30M", item_val);
    item_val.setValue(KTYPE_PERIOD_HOUR);
    ui.cmb_k_type->addItem("HOUR", item_val);

    item_val.setValue(KTYPE_PERIOD_DAY);
    ui.cmb_k_type->addItem("DAY", item_val);
    item_val.setValue(KTYPE_PERIOD_WEEK);
    ui.cmb_k_type->addItem("WEEK", item_val);
    item_val.setValue(KTYPE_PERIOD_MON);
    ui.cmb_k_type->addItem("MON", item_val);

    bool ret = connect(ui.btnGetHisData, SIGNAL(clicked()), this, SLOT(DoGetHisData()));
    ret = connect(ui.btnGetRecentHisData, SIGNAL(clicked()), this, SLOT(DoGetRecentHisData()));

    timer_ = new QTimer(this);
    ret = connect(timer_, SIGNAL(timeout()), this, SLOT(DoTimeOut()));

    ret = ret;
}

AquireDataWin::~AquireDataWin()
{

}

bool AquireDataWin::Init()
{
    int ret = HqWrapperApi_Init(app_->exchange_calendar());
    return ret > -1;
}

void AquireDataWin::DoGetHisData()
{ 
    auto val = ui.cmb_k_type->currentData();
    auto k_type = val.toInt();
    bool result = HqWrapperApi_GetAllHisKBars("SCL9", true, MARKET_SH_FUTURES, k_type, SaveKbarDataCallback, app_);
    if( result )
    {
        ui.lab_information->setText("Geting Data!");
        timer_->start(500);
    }
}

void AquireDataWin::DoGetRecentHisData()
{ 
    auto val = ui.cmb_k_type->currentData();
    auto k_type = val.toInt();
    int count = MAX_K_COUNT;
    // normal include night trading is 9 hours( day: 4 hours + night 5 hours)
    switch(k_type)
    {
        case KTYPE_PERIOD_1M:
            count = 60 * 9 * 20; 
            break;
        case KTYPE_PERIOD_5M:
            count = 60 * 9 * 20 / 5; 
            break;
        case KTYPE_PERIOD_15M:
            count = 60 * 9 * 20 / 15; 
            break;
        case KTYPE_PERIOD_30M:
            count = 60 * 9 * 20 / 30; 
            break;
        case KTYPE_PERIOD_HOUR:
            count = 9 * 20;
            break;
        case KTYPE_PERIOD_DAY:
            count = 3 * 20;
            break;
        case KTYPE_PERIOD_WEEK:
            count = 4 * 6;
            break;
        case KTYPE_PERIOD_MON:
            count = 12;
            break;
        /*case KTYPE_PERIOD_YEAR:
            count = 2;
            break;*/
    }
      
    bool result = HqWrapperApi_GetRecentHisKBars("SCL9", true, MARKET_SH_FUTURES, k_type, count, SaveKbarDataCallback, app_);
    if( result )
    {
        ui.lab_information->setText("Geting Recent Data!");
        timer_->start(500);
    }
}

void AquireDataWin::closeEvent(QCloseEvent * event)
{
    auto ret_button = QMessageBox::question(nullptr, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("确定退出系统?"),
        QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if( ret_button == QMessageBox::Cancel )
        event->ignore();
    else
        app_->Stop();
}

void AquireDataWin::DoTimeOut()
{
    if( HqWrapperApi_IsFinishGettingData() )
    {   
        timer_->stop();
        ui.lab_information->setText("Geting Data has Finished!");
        QMessageBox::information(nullptr, "information", "finish!");
    }
}