#include "aquire_data_win.h"
#include <QMessageBox>
//#include <QEvent>
#include <QtGui/QtEvents>
#include <qtimer.h>

#include "aquire_data_app.h"

#include "hq_wrapper_api.h"


AquireDataWin::AquireDataWin(AquireDataApp *app, QWidget *parent)
    : app_(app)
    , QWidget(parent)
    , timer_(nullptr)
{
    ui.setupUi(this);
    bool ret = connect(ui.btnGetHisData, SIGNAL(clicked()), this, SLOT(DoGetHisData()));

    timer_ = new QTimer(this);
    ret = connect(timer_, SIGNAL(timeout()), this, SLOT(DoTimeOut()));

    ret = ret;
}

AquireDataWin::~AquireDataWin()
{

}

bool AquireDataWin::Init()
{
    int ret = HqWrapperApi_Init();
    return ret > -1;
}

void AquireDataWin::DoGetHisData()
{ 
    bool result = HqWrapperApi_GetAllHisKBars("SCL9", true, MARKET_SH_FUTURES, KTYPE_PERIOD_5M);
    if( result )
        timer_->start(500);
}

void AquireDataWin::closeEvent(QCloseEvent * event)
{
    auto ret_button = QMessageBox::question(nullptr, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("ȷ���˳�ϵͳ?"),
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
        QMessageBox::information(nullptr, "information", "finish!");
    }
}