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

#define ORDER_TYPE_HANGON  1
#define ORDER_TYPE_STOPPROFIT 2
#define ORDER_TYPE_STOPLOSS 3
#define ORDER_TYPE_CONDITION  4
#define ORDER_TYPE_ALL  5
 
class CfgStopProfitLossDlg;
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

    void DoTblPosDbClick(const QModelIndex &);
    void OnTblHangonOrdersRowDoubleClicked(const QModelIndex &);
    void OnScrollTrainTimeMoved(int);
    void OnStartTrain();
    void OnStopTrain();

    void OnCloseAllUnfrozenPos();

    void OnNextStep();
     
    void OnBuy();
    void OnSell();
    
    double cur_quote(){ return cur_quote_; }

    void SaveStopProfitLoss(std::vector<PositionAtom> &pos_atoms);

protected:

    virtual void closeEvent(QCloseEvent *) override;
    //virtual void hideEvent(QHideEvent * event) override;
private:

    void PrintTradeRecords();
    
    //void OpenPosition(double para_price, bool is_long);
    void OpenPosition(double para_price, unsigned int qty, bool is_long);
    void CloseInputSizePosition(double para_price, bool is_long);

    bool AddOpenOrder(double price, unsigned int quantity, bool is_long);
    bool AddCloseOrder(double price, unsigned int quantity, bool is_long);

    std::vector<TradeRecordAtom> DoIfStopProfitLoss(const T_StockHisDataItem &k_item, std::vector<int> &ret_pos_ids, double &ret_profit);
    unsigned int GetItemPositionAllQty(QStandardItemModel& model, int row_index);

    void UpdateOrders2KlineWalls(int type);

    //int QtyInHangonOrderInfo();
    int TblHangonOrdersRowCount();
    
    // UI -----
    void SetStatusBar(const QString & val)
    {
        ui.lab_status->setText(val);
    }
    void RefreshCapitalUi();
    void RemoveInPositionTableView(int position_id, PositionType position_type);
    void RecaculatePosTableViewItem(QVector<int> &ids, int row_index);
    void RecaculatePosTableViewItem(int row_index);
    double RecaculatePosTableViewFloatProfit(double cur_price);

    void Append2TblHangonOrders(OrderInfo &order_info);
    void RemoveFromTblHangonOrderByFakeId(int fake_id);

    void Append2TblTrades(TradeRecordAtom &trade);

private:

    Ui::TrainDlgForm ui;

    KLineWall *parent_;
    MainWindow *main_win_;
    CfgStopProfitLossDlg *cfg_stop_profitloss_dlg_;

    bool is_started_;
    
    AccountInfo  account_info_;
    double ori_capital_;
    double force_close_low_;
    double force_close_high_;

    double fee_rate_;
     
    std::vector<TradeRecordAtom>  trade_records_;
    std::list<OrderInfo> hangon_order_infos_;
    std::list<OrderInfo> stop_profit_order_infos_; 
    std::list<OrderInfo> stop_loss_order_infos_; 
    std::list<OrderInfo> condition_order_infos_; 

    T_DateRange  hisk_date_range_;
    int scroll_bar_date_;

    // auto stop profit/loss related -----
    bool auto_stop_profit_;
    bool auto_stop_loss_;
    unsigned int auto_stop_profit_ticks_;
    unsigned int auto_stop_loss_ticks_;

    unsigned int cur_train_step_;
    double cur_quote_;
};

#endif // TRAIN_DLG_SDFS23343543_H_