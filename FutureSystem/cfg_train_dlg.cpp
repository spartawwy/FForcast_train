#include "cfg_train_dlg.h"

#include "train_dlg.h"


CfgTrainDlg::CfgTrainDlg(TrainDlg *parent)
    : parent_(parent)
{
    ui.setupUi(this);
    bool ret = connect(ui.pbtn_save, SIGNAL(clicked()), this, SLOT(OnSave()));
    assert(ret);
    ret = connect(ui.pbtn_cancel, SIGNAL(clicked()), this, SLOT(OnClose()));
    assert(ret);
}

void CfgTrainDlg::showEvent(QShowEvent *)
{
    ui.chbox_auto_stop_loss->setChecked(parent_->auto_stop_loss_);
    ui.spb_stop_loss->setValue(parent_->auto_stop_loss_ticks_);

    ui.chbox_auto_stop_profit->setChecked(parent_->auto_stop_profit_);
    ui.spb_stop_profit->setValue(parent_->auto_stop_profit_ticks_);
}

void CfgTrainDlg::OnSave()
{
    parent_->auto_stop_loss_ = ui.chbox_auto_stop_loss->isChecked();
    parent_->auto_stop_loss_ticks_ = ui.spb_stop_loss->value();

    parent_->auto_stop_profit_ = ui.chbox_auto_stop_profit->isChecked();
    parent_->auto_stop_profit_ticks_ = ui.spb_stop_profit->value();
    this->hide();
}

void CfgTrainDlg::OnClose()
{
    this->hide();
}