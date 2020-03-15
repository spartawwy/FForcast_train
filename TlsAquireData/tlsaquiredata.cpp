#include "tlsaquiredata.h"

#include "hq_wrapper_api.h"


TlsAquireData::TlsAquireData(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    bool ret = connect(ui.btnGetHisData, SIGNAL(clicked()), this, SLOT(DoGetHisData()));

    ret = ret;

}

TlsAquireData::~TlsAquireData()
{

}

void TlsAquireData::DoGetHisData()
{
    int ret = HqWrapperApi_Init();
}