#ifndef CFG_TRAIN_DLG_H_
#define CFG_TRAIN_DLG_H_
 
#include <cassert>

//#include <QVector>
#include <QItemDelegate>
#include <QtWidgets/QWidget>
#include <QStandardItemModel>

#include "ui_cfgtrain.h"

#include "stkfo_common.h"
//#include "train_trade.h"
  
class PositionAtom;
class TrainDlg;
class CfgTrainDlg : public QWidget
{
    Q_OBJECT

public:

    CfgTrainDlg(TrainDlg *parent);

private slots:

    void OnSave();
    void OnClose();

private:

    virtual void showEvent(QShowEvent * event) override;
    //void SetStatusBar(const QString &content);

    Ui::CfgTrainForm ui;
    TrainDlg *parent_;
     
};

#endif // CFG_TRAIN_DLG_H_