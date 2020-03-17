#include "aquire_data_app.h"
#include "aquire_data_win.h"

#include "aqd_database.h"

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
    option_dir_type(AppBase::DirType::STAND_ALONE_APP);
    option_validate_app(false);

    std::string cur_dir(".//");
    work_dir(cur_dir);
    local_logger_.SetDir(cur_dir);

    data_base_ = std::make_shared<DataBase>(this);
    data_base_->Initialize();
    //---------------
    win_ = std::make_shared<AquireDataWin>(this);
    win_->Init();
    win_->show();

    return true;
}

void AquireDataApp::Stop()
{
    Shutdown();
    this->quit();
}