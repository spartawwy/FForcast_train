#include "aquire_data_app.h"
#include "aquire_data_win.h"

#include "aqd_database.h"
#include "exch_calendar.h"

#include "write_log.h"

AquireDataApp::AquireDataApp(int argc, char* argv[])
    : QApplication(argc, argv)
    , ServerClientAppBase("client", "aquire_data_app", "1.0")
{

}

AquireDataApp::~AquireDataApp()
{

}

bool AquireDataApp::Init()
{
    WriteLog("%s %d", __FUNCTION__, __LINE__);
    option_dir_type(AppBase::DirType::STAND_ALONE_APP);
    option_validate_app(false);

    std::string cur_dir(".//");
    work_dir(cur_dir);
    local_logger_.SetDir(cur_dir);

    WriteLog("%s %d", __FUNCTION__, __LINE__);
    data_base_ = std::make_shared<DataBase>(this);
    data_base_->Initialize();

    exchange_calendar_ = std::make_shared<ExchangeCalendar>();
    WriteLog("%s %d", __FUNCTION__, __LINE__);
    data_base_->LoadTradeDate(exchange_calendar_.get());
    //---------------
    WriteLog("%s %d", __FUNCTION__, __LINE__);
    win_ = std::make_shared<AquireDataWin>(this);
    WriteLog("%s %d", __FUNCTION__, __LINE__);
    win_->Init();
    WriteLog("%s %d", __FUNCTION__, __LINE__);
    win_->show();

    return true;
}

void AquireDataApp::Stop()
{
    Shutdown();
    this->quit();
}

void AquireDataApp::SaveKbarDatas(int kbar_type, T_KbarData *data_ret[], unsigned int size)
{
    auto data_container = std::make_shared<T_KbarDataContainer>();
    for( int i = 0; i < size; ++i )
    {
        data_container->push_front(*data_ret[i]);
    }
    data_base_->SaveKbarData(data_container , TlsTypePeriod(kbar_type));
}