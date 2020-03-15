#ifndef AQUIRE_DATA_APP_H_
#define AQUIRE_DATA_APP_H_

#include <memory>

#include <QtWidgets/QApplication>
#include <QThread>

#include <TLib/tool/tsystem_server_client_appbase.h>
#include <TLib/core/tsystem_communication_common.h>
#include <TLib/core/tsystem_serialization.h>

//#include "stock_data_man.h"

//class QTimer;
class ExchangeCalendar;
class DataBase;
//class StockMan;
class AquireDataWin;
//class MainWindow;
//class KLineWall;
class AquireDataApp : public QApplication, public TSystem::ServerClientAppBase
{
    //Q_OBJECT

public:

    AquireDataApp(int argc, char* argv[]); 
    ~AquireDataApp();

    bool Init();
    void Stop();

protected:

    virtual void HandleNodeHandShake(TSystem::communication::Connection* , const TSystem::Message& ) override {};
    virtual void HandleNodeDisconnect(std::shared_ptr<TSystem::communication::Connection>& 
        , const TSystem::TError& ) override {};

private:
    std::shared_ptr<AquireDataWin>  win_;
};

#endif