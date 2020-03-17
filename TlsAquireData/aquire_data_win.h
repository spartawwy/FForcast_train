#ifndef TLSAQUIREDATA_WIN_H
#define TLSAQUIREDATA_WIN_H

#include <QtWidgets/QWidget>
#include "ui_tlsaquiredata.h"

class QTimer;
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
    void DoTimeOut();

protected:
    virtual void closeEvent(QCloseEvent * event) override;

private:
    Ui::TlsAquireDataClass ui;

    AquireDataApp *app_;

    QTimer *timer_;
};

#endif // TLSAQUIREDATA_H
