//#include "tlsaquiredata.h"
#include "aquire_data_app.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QApplication>
#include <qtextcodec.h>

#include <Tlib/core/tsystem_core_paths.h>
#include <TLib/tool/tsystem_exe_frame.h>
 
#include "write_log.h"

#define APP_CODE_TEXT "GBK"
//
//int main(int argc, char *argv[])
//{
//    QCoreApplication::addLibraryPath("D:\\Qt\\qt5.2.1_win32\\bin\\plugins");
//    QCoreApplication::addLibraryPath(".");
//    QCoreApplication::addLibraryPath("./plugins");
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName(APP_CODE_TEXT));
//
//    QApplication a(argc, argv);
//
//    TlsAquireData w;
//    w.show();
//    return a.exec();
//}


class TheFrame : public TSystem::ExecutableFrame
{

public:
    int main(int argc, char* argv[])
    {
        WriteLog("%s %d", __FUNCTION__, __LINE__);
        TSystem::utility::ProjectTag("WZF");
#ifdef PUBLISH
        if( !ValidSerial() )
            return -1;
#else
        WriteLog("%s %d", __FUNCTION__, __LINE__);
        QCoreApplication::addLibraryPath("D:\\Qt\\qt5.2.1_win32\\bin\\plugins");
#endif
        QCoreApplication::addLibraryPath(".");
        QCoreApplication::addLibraryPath("./plugins");
        QTextCodec::setCodecForLocale(QTextCodec::codecForName(APP_CODE_TEXT));
        WriteLog("%s %d", __FUNCTION__, __LINE__);
        AquireDataApp aquire_data_app(argc, argv); 
        int ret = 0; 
        PrintAppInfo(aquire_data_app);
        WriteLog("%s %d", __FUNCTION__, __LINE__);
        if( aquire_data_app.Init() )
        { 
            PrintLaunchDone();
            ret = aquire_data_app.exec(); 
        }else
            aquire_data_app.Stop();

        aquire_data_app.WaitShutdown();

        return ret;
    }
};

int main(int argc, char* argv[])
{
    TheFrame frame;
    return frame.LaunchServer(argc, argv);
}