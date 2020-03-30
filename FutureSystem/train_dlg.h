#ifndef TRAIN_DLG_SDFS23343543_H_
#define TRAIN_DLG_SDFS23343543_H_

//#include <unordered_map>
#include <cassert>
#include <vector>
#include <list>

#include <QVector>
#include <QtWidgets/QWidget>
#include <QStandardItemModel>
#include "ui_traindlg.h"

#include "stkfo_common.h"
#include "train_trade.h"

#include "train_trade_dlg.h"

 
class KLineWall;
class MainWindow;
class TrainDlg : public QWidget
{
    Q_OBJECT

public:

    TrainDlg(KLineWall *parent, MainWindow *main_win);
     
    AccountInfo & account_info() { return account_info_; }

    const T_StockHisDataItem & CurHisStockDataItem();

public slots:

    void OnScrollTrainTimeMoved(int);
    void OnStartTrain();
    void OnStopTrain();

    void OnMoveToNextK();
    //void OnMoveToPreK();

    void OnNextStep();

    void OnOpenOpenWin();
    void OnOpenCloseWin();

    void OnTrade();
    void OnBuy();
    void OnSell();
    
protected:

    virtual void closeEvent(QCloseEvent *) override;
    //virtual void hideEvent(QHideEvent * event) override;
private:

    void PrintTradeRecords();
    
    void OpenPosition(double para_price, bool is_long);
    void ClosePosition(double para_price, bool is_long);

    bool AddOpenOrder(double price, unsigned int quantity, bool is_long);
    bool AddCloseOrder(double price, unsigned int quantity, bool is_long);

    unsigned int GetItemPositionAllQty(QStandardItemModel& model, int row_index);

    double RecaculatePosTableViewFloatProfit(double cur_price);

    void UpdateOrders2KlineWalls();
    // UI -----
    void SetStatusBar(const QString & val)
    {
        ui.lab_status->setText(val);
    }
    void RefreshCapitalUi();
    void RemoveInPositionTableView(int position_id, PositionType position_type);
    void RecaculatePosTableViewItem(QVector<int> &ids, int row_index);

private:

    Ui::TrainDlgForm ui;
     
    TrainTradeDlg  trade_dlg_;

    KLineWall *parent_;
    MainWindow *main_win_;
     
    bool is_started_;
    
    AccountInfo  account_info_;
    double force_close_low_;
    double force_close_high_;

    double fee_rate_;
     
    std::vector<TradeRecordAtom>  trade_records_;
    std::list<OrderInfo> hangon_order_infos_;
    std::list<OrderInfo> stop_order_infos_; 
    std::list<OrderInfo> condition_order_infos_; 

    T_DateRange  hisk_date_range_;
    int scroll_bar_date_;

    // auto stop profit/loss related -----
    bool auto_stop_profit_;
    bool auto_stop_loss_;
    unsigned int auto_stop_profit_ticks_;
    unsigned int auto_stop_loss_ticks_;

};

#endif // TRAIN_DLG_SDFS23343543_H_