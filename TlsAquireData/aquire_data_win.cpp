#include "aquire_data_win.h"
#include <QMessageBox>
//#include <QEvent>
#include <QtGui/QtEvents>
#include "aquire_data_app.h"

#include "hq_wrapper_api.h"


AquireDataWin::AquireDataWin(AquireDataApp *app, QWidget *parent)
    : app_(app)
    , QWidget(parent)
{
    ui.setupUi(this);
    bool ret = connect(ui.btnGetHisData, SIGNAL(clicked()), this, SLOT(DoGetHisData()));

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
    int ret = HqWrapperApi_Init();

    HqWrapperApi_GetAllHisKBars("SCL9", true, MARKET_SH_FUTURES, KTYPE_PERIOD_HOUR);
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