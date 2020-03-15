#ifndef TLSAQUIREDATA_WIN_H
#define TLSAQUIREDATA_WIN_H

#include <QtWidgets/QWidget>
#include "ui_tlsaquiredata.h"

class AquireDataApp;
class AquireDataWin : public QWidget
{
    Q_OBJECT

public:
    AquireDataWin(AquireDataApp *app, QWidget *parent = 0);
    ~AquireDataWin();
    bool Init();

public slots:

    void DoGetHisData();

protected:
    virtual void closeEvent(QCloseEvent * event) override;

private:
    Ui::TlsAquireDataClass ui;

    AquireDataApp *app_;
};

#endif // TLSAQUIREDATA_H
