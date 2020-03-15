#include "tlsaquiredata.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QApplication>
#include <qtextcodec.h>

#include <Tlib/core/tsystem_core_paths.h>
#include <TLib/tool/tsystem_exe_frame.h>
 

#define APP_CODE_TEXT "GBK"

int main(int argc, char *argv[])
{
    QCoreApplication::addLibraryPath("D:\\Qt\\qt5.2.1_win32\\bin\\plugins");
    QCoreApplication::addLibraryPath(".");
    QCoreApplication::addLibraryPath("./plugins");
    QTextCodec::setCodecForLocale(QTextCodec::codecForName(APP_CODE_TEXT));

    QApplication a(argc, argv);

    TlsAquireData w;
    w.show();
    return a.exec();
}
