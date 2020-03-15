#ifndef TLSAQUIREDATA_H
#define TLSAQUIREDATA_H

#include <QtWidgets/QWidget>
#include "ui_tlsaquiredata.h"

class TlsAquireData : public QWidget
{
    Q_OBJECT

public:
    TlsAquireData(QWidget *parent = 0);
    ~TlsAquireData();

public slots:
        void DoGetHisData();

private:
    Ui::TlsAquireDataClass ui;
};

#endif // TLSAQUIREDATA_H
