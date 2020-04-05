#include "train_dlg.h"

#include <QtWidgets/QComboBox>
#include <QMessageBox>
#include <QStatusBar>
#include <QDebug>

#include <TLib/core/tsystem_utility_functions.h>

#include "database.h"
#include "exchange_calendar.h"

#include "futures_forecast_app.h"
#include "mainwindow.h"
#include "tool_bar.h"
#include "kline_wall.h"
#include "cfg_stop_profitloss_dlg.h"

#include "train_trade.h"

//static const int cst_period_day_index = 4;
static const int cst_small_width = 60;

static const int cst_tbview_position_id = 0;
static const int cst_tbview_position_average_price = 1;
static const int cst_tbview_position_float_profit = 2;
static const int cst_tbview_position_bs = 3;
static const int cst_tbview_position_size = 4;
static const int cst_tbview_position_avaliable = 5;
static const int cst_tbview_position_filled_time = 6;
static const int cst_tbview_position_col_count = 7;

static const int cst_tbview_hangonorder_id = 0;
static const int cst_tbview_hangonorder_bs = 1;
static const int cst_tbview_hangonorder_oc = 2;
static const int cst_tbview_hangonorder_price = 3;
static const int cst_tbview_hangonorder_qty = 4;
static const int cst_tbview_hangonorder_col_count = 5;

static const int cst_tbview_trades_id = 0;
static const int cst_tbview_trades_bs = 1;
static const int cst_tbview_trades_oc = 2;
static const int cst_tbview_trades_price = 3;
static const int cst_tbview_trades_qty = 4;
static const int cst_tbview_trades_profit = 5;
static const int cst_tbview_trades_fee = 6;
static const int cst_tbview_trades_time = 7;
static const int cst_tbview_trades_col_count = 8;


static int find_model_first_fit_index(QStandardItemModel& model, bool is_long_pos);
static std::tuple<double, unsigned int> get_total_amount_qty(PositionInfo &position, QVector<int> &ids);

static bool erase_rel_item_by_pos_ids(std::list<OrderInfo> &order_info, std::list<OrderInfo>::iterator &order_item, std::vector<int> &target_del_ids );
static bool erase_rel_item_by_fake_ids(std::list<OrderInfo> &order_info, std::list<OrderInfo>::iterator &order_item, std::vector<int> &target_del_ids );
static void remove_container_items_by_pos_ids(std::list<OrderInfo> &container, std::vector<int> *long_deleted_ids, std::vector<int> *short_deleted_ids);

//////////////////////////////////////////
TrainDlg::TrainDlg(KLineWall *parent,  MainWindow *main_win)
    : QWidget(nullptr)
    , parent_(parent)
    , main_win_(main_win)
    , cfg_stop_profitloss_dlg_(nullptr)
    , is_started_(false)
    , account_info_()
    , ori_capital_(cst_default_ori_capital)
    , force_close_low_(MAX_PRICE)
    , force_close_high_(MIN_PRICE) 
    , scroll_bar_date_(0)
    , auto_stop_profit_(true)
    , auto_stop_loss_(true)
    , auto_stop_profit_ticks_(10)
    , auto_stop_loss_ticks_(10)
    , cur_train_step_(0)
    , cur_quote_(0.0)
{
    ui.setupUi(this);

    cfg_stop_profitloss_dlg_ = new CfgStopProfitLossDlg(this);
    cfg_stop_profitloss_dlg_->setWindowFlags(cfg_stop_profitloss_dlg_->windowFlags() | Qt::WindowStaysOnTopHint/*Qt::Dialog*/ );
    cfg_stop_profitloss_dlg_->hide();
     
    bool ret = false;
    ret = connect(ui.pbtnStart, SIGNAL(clicked()), this, SLOT(OnStartTrain()));
    ret = connect(ui.pbtnStop, SIGNAL(clicked()), this, SLOT(OnStopTrain()));

    ret = connect(ui.pbtnNextStep, SIGNAL(clicked()), this, SLOT(OnNextStep()));
     
    ret = connect(ui.pbtn_buy, SIGNAL(clicked()), this, SLOT(OnBuy()));
    ret = connect(ui.pbtn_sell, SIGNAL(clicked()), this, SLOT(OnSell()));
    ret = ret;
    // set his k date range info----------------
    T_DateRange  date_rage_5m;
    bool result = main_win_->app_->data_base()->GetHisKBarDateRange(DEFAULT_CODE, false, TypePeriod::PERIOD_5M, date_rage_5m);
    if( !result )
        QMessageBox::information(nullptr, "erro", "there is no history k date of 5m");
    int eldest_date_5m = std::get<0>(date_rage_5m);
    int eldest_time_5m = std::get<1>(date_rage_5m);
    int latest_date_5m = std::get<2>(date_rage_5m);
    int latest_time_5m = std::get<3>(date_rage_5m);
    T_DateRange  date_rage_1m;
    result = main_win_->app_->data_base()->GetHisKBarDateRange(DEFAULT_CODE, false, TypePeriod::PERIOD_1M, date_rage_1m);
    if( !result )
        QMessageBox::information(nullptr, "erro", "there is no history k date of 1m");
    int eldest_date_1m = std::get<0>(date_rage_1m);
    int eldest_time_1m = std::get<1>(date_rage_1m);
    int latest_date_1m = std::get<2>(date_rage_1m);
    int latest_time_1m = std::get<3>(date_rage_1m);

    int eldest_date = std::max(eldest_date_5m, eldest_date_1m);
    int eldest_time = 0; 
    if( eldest_date_5m == eldest_date_1m )
        eldest_time = std::max(eldest_time_5m, eldest_time_1m);
    else 
        eldest_time = (eldest_date == eldest_date_5m ? eldest_time_5m : eldest_time_1m);

    int latest_date = std::min(latest_date_5m, latest_date_1m);
    int latest_time = 0;
    if( latest_date_5m == latest_date_1m )
        latest_time = std::min(latest_time_5m, latest_time_1m);
    else
        latest_time = (latest_date == latest_date_5m ? latest_time_5m : latest_time_1m);

    hisk_date_range_ = std::make_tuple(eldest_date, eldest_time, latest_date, latest_time);
    
    //----------------------------------------
    ui.hScrollBar_TrainTimeRange->setMinimum(eldest_date);
    ui.hScrollBar_TrainTimeRange->setMaximum(latest_date);
    ui.hScrollBar_TrainTimeRange->setValue(latest_date);
    int distan_days = main_win_->app_->exchange_calendar()->DateTradingSpan(eldest_date, latest_date);
    ui.hScrollBar_TrainTimeRange->setSingleStep((latest_date-eldest_date)/distan_days);
#if 1  // temp for debug
    ui.lab_start_date->setText(QString::number(20200305));//ndedt
    ui.hScrollBar_TrainTimeRange->setValue(20200305);
#endif
    scroll_bar_date_ = eldest_date;
    ret = connect(ui.hScrollBar_TrainTimeRange, SIGNAL(sliderMoved(int)), this, SLOT(OnScrollTrainTimeMoved(int)));
    assert(ret);
    //-----------------------------------------
    ret = connect(ui.pbtn_market_price_c, SIGNAL(clicked()), this, SLOT(OnCloseAllUnfrozenPos()));
    //------------------table position
    ui.table_view_position->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    QStandardItemModel * model = new QStandardItemModel(0, cst_tbview_position_col_count, this);
    model->setHorizontalHeaderItem(cst_tbview_position_id, new QStandardItem(QString::fromLocal8Bit("ID")));
    model->horizontalHeaderItem(cst_tbview_position_id)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_position_bs, new QStandardItem(QString::fromLocal8Bit("买卖")));
    model->horizontalHeaderItem(cst_tbview_position_bs)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_position_size, new QStandardItem(QString::fromLocal8Bit("持仓")));
    model->horizontalHeaderItem(cst_tbview_position_size)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_position_avaliable, new QStandardItem(QString::fromLocal8Bit("可用")));
    model->horizontalHeaderItem(cst_tbview_position_avaliable)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_position_average_price, new QStandardItem(QString::fromLocal8Bit("均价")));
    model->horizontalHeaderItem(cst_tbview_position_average_price)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_position_float_profit, new QStandardItem(QString::fromLocal8Bit("浮赢")));
    model->horizontalHeaderItem(cst_tbview_position_float_profit)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_position_filled_time, new QStandardItem(QString::fromLocal8Bit("时间")));
    model->horizontalHeaderItem(cst_tbview_position_filled_time)->setTextAlignment(Qt::AlignCenter);
    ui.table_view_position->setModel(model);
    ui.table_view_position->setColumnWidth(cst_tbview_position_id, cst_small_width);
    ui.table_view_position->setColumnWidth(cst_tbview_position_average_price, cst_small_width);
    ui.table_view_position->setColumnWidth(cst_tbview_position_float_profit, cst_small_width);
    ui.table_view_position->setColumnWidth(cst_tbview_position_bs, cst_small_width/2);
    ui.table_view_position->setColumnWidth(cst_tbview_position_size, cst_small_width);
    ui.table_view_position->setColumnWidth(cst_tbview_position_avaliable, cst_small_width);
    ret = connect(ui.table_view_position, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(DoTblPosDbClick(const QModelIndex&)));
    assert(ret);
    //--------------------table hangon order------ 
     
    ui.table_view_order_hangon->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    model = new QStandardItemModel(0, cst_tbview_hangonorder_col_count, this);
    model->setHorizontalHeaderItem(cst_tbview_hangonorder_id, new QStandardItem(QString::fromLocal8Bit("ID")));
    model->horizontalHeaderItem(cst_tbview_hangonorder_id)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_hangonorder_bs, new QStandardItem(QString::fromLocal8Bit("买卖")));
    model->horizontalHeaderItem(cst_tbview_hangonorder_bs)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_hangonorder_oc, new QStandardItem(QString::fromLocal8Bit("开平")));
    model->horizontalHeaderItem(cst_tbview_hangonorder_oc)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_hangonorder_price, new QStandardItem(QString::fromLocal8Bit("价格")));
    model->horizontalHeaderItem(cst_tbview_hangonorder_price)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_hangonorder_qty, new QStandardItem(QString::fromLocal8Bit("数量")));
    model->horizontalHeaderItem(cst_tbview_hangonorder_qty)->setTextAlignment(Qt::AlignCenter);
    ui.table_view_order_hangon->setModel(model);
    ui.table_view_order_hangon->setColumnWidth(cst_tbview_hangonorder_id, cst_small_width/2);
    ui.table_view_order_hangon->setColumnWidth(cst_tbview_hangonorder_bs, cst_small_width/2);
    ui.table_view_order_hangon->setColumnWidth(cst_tbview_hangonorder_oc, cst_small_width/2);
    ui.table_view_order_hangon->setColumnWidth(cst_tbview_hangonorder_qty, cst_small_width);
    ui.table_view_order_hangon->setColumnWidth(cst_tbview_hangonorder_price, cst_small_width);
    ret = connect(ui.table_view_order_hangon, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(OnTblHangonOrdersRowDoubleClicked(const QModelIndex&)));
    assert(ret);
    //------------------table trades----------
    ui.table_view_trades->setEditTriggers(QAbstractItemView::NoEditTriggers);
    model = new QStandardItemModel(0, cst_tbview_trades_col_count, this);
    model->setHorizontalHeaderItem(cst_tbview_trades_id, new QStandardItem(QString::fromLocal8Bit("ID")));
    model->horizontalHeaderItem(cst_tbview_trades_id)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_trades_bs, new QStandardItem(QString::fromLocal8Bit("买卖")));
    model->horizontalHeaderItem(cst_tbview_trades_bs)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_trades_oc, new QStandardItem(QString::fromLocal8Bit("开平")));
    model->horizontalHeaderItem(cst_tbview_trades_oc)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_trades_price, new QStandardItem(QString::fromLocal8Bit("价格")));
    model->horizontalHeaderItem(cst_tbview_trades_price)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_trades_qty, new QStandardItem(QString::fromLocal8Bit("数量")));
    model->horizontalHeaderItem(cst_tbview_trades_qty)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_trades_profit, new QStandardItem(QString::fromLocal8Bit("盈亏")));
    model->horizontalHeaderItem(cst_tbview_trades_profit)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_trades_fee, new QStandardItem(QString::fromLocal8Bit("手续费")));
    model->horizontalHeaderItem(cst_tbview_trades_fee)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_trades_time, new QStandardItem(QString::fromLocal8Bit("时间")));
    model->horizontalHeaderItem(cst_tbview_trades_time)->setTextAlignment(Qt::AlignCenter);
    ui.table_view_trades->setModel(model);
    ui.table_view_trades->setColumnWidth(cst_tbview_trades_id, cst_small_width/2);
    ui.table_view_trades->setColumnWidth(cst_tbview_trades_bs, cst_small_width/2);
    ui.table_view_trades->setColumnWidth(cst_tbview_trades_oc, cst_small_width/2);
    ui.table_view_trades->setColumnWidth(cst_tbview_trades_price, cst_small_width);
    ui.table_view_trades->setColumnWidth(cst_tbview_trades_qty, cst_small_width);
    ui.table_view_trades->setColumnWidth(cst_tbview_trades_profit, cst_small_width);
    ui.table_view_trades->setColumnWidth(cst_tbview_trades_fee, cst_small_width);
    ui.table_view_trades->setColumnWidth(cst_tbview_trades_time, cst_small_width);
    // cst_tbview_trades_id 

    OnStopTrain();

    account_info_.capital.avaliable = ori_capital_;
    account_info_.capital.frozen = 0.0;
    account_info_.capital.float_profit = 0.0;
    RefreshCapitalUi();
    //ui.dbspbFeeRate->setValue(cst_default_fee_rate_percent);
}

const T_StockHisDataItem & TrainDlg::CurHisStockDataItem()
{
    return parent_->CurTrainStockDataItem();
}
               
void TrainDlg::DoTblPosDbClick(const QModelIndex &index)
{
    assert(cfg_stop_profitloss_dlg_);

    QStandardItemModel * model = static_cast<QStandardItemModel *>(ui.table_view_position->model());

    std::vector<std::shared_ptr<PositionAtom>> rel_pos_atoms;  
    QVector<int> pos_ids = model->item(index.row(), cst_tbview_position_id)->data().value<QVector<int>>();
    for( int i = 0; i < pos_ids.size(); ++i )
    {
        std::shared_ptr<PositionAtom> pos_atom = account_info_.position.FindPositionAtomSharedPointer(pos_ids[i]);
        if( !pos_atom || pos_atom->qty_available == 0 )
            continue;
        rel_pos_atoms.push_back(pos_atom);
    }
    cfg_stop_profitloss_dlg_->SetContent(rel_pos_atoms);
    cfg_stop_profitloss_dlg_->showNormal();
}

void TrainDlg::OnTblHangonOrdersRowDoubleClicked(const QModelIndex &index)
{
    QStandardItemModel * tbl_view_pos_model = static_cast<QStandardItemModel *>(ui.table_view_position->model());
    auto model = (QStandardItemModel*)ui.table_view_order_hangon->model();
    auto temp_order_id = model->item(index.row(), cst_tbview_hangonorder_id)->text().toInt();
    //bool is_target_long = model->item(index.row(), cst_tbview_hangonorder_oc)->text() == QString::fromLocal8Bit("卖") ? true : false;
    for( auto iter = hangon_order_infos_.begin(); iter != hangon_order_infos_.end(); )
    {
        if( iter->fake_id == temp_order_id )
        {
            if( iter->action == OrderAction::OPEN )
            {
                account_info_.capital.frozen -= cst_margin_capital * iter->qty;
                account_info_.capital.avaliable += cst_margin_capital * iter->qty;
                RefreshCapitalUi(); 
            }else  // close
            {
                // unfroze related positions 
                for(auto item = iter->help_contain.begin(); item != iter->help_contain.end(); ++item )
                { 
                    auto pos_item = account_info_.position.FindPositionAtom(item->first);
                    if( pos_item )
                    {
                        auto iter_frozen_party = pos_item->qty_frozens.find(temp_order_id);
                        if( iter_frozen_party != pos_item->qty_frozens.end() )
                        {
                            pos_item->qty_available += iter_frozen_party->second;
                            pos_item->qty_frozens.erase(iter_frozen_party);
                        }
                    }
                }
                auto row_index = find_model_first_fit_index(*tbl_view_pos_model, iter->position_type == PositionType::POS_LONG);
                RecaculatePosTableViewItem(row_index);
            }
            hangon_order_infos_.erase(iter++);
            break;
        }
        ++iter;
    }
    model->removeRow(index.row());
     
    UpdateOrders2KlineWalls(ORDER_TYPE_HANGON);
}

void TrainDlg::OnScrollTrainTimeMoved(int val)
{
    int distance = val - ui.hScrollBar_TrainTimeRange->minimum();
    int dis_trade_days = distance / ui.hScrollBar_TrainTimeRange->singleStep(); 
    scroll_bar_date_ = main_win_->app_->exchange_calendar()->NextTradeDate(ui.hScrollBar_TrainTimeRange->minimum(), dis_trade_days);
    ui.lab_start_date->setText(QString::number(scroll_bar_date_));
}

void TrainDlg::closeEvent(QCloseEvent * /*event*/)
{
    main_win_->is_train_mode(false);
    parent_->k_rend_index_for_train_ = 0;
    if( main_win_->SubKlineWall() )
        main_win_->SubKlineWall()->k_rend_index_for_train_ = 0;
    main_win_->tool_bar()->main_cycle_comb()->setEnabled(true);

    is_started_ = false;

    OnStopTrain();
}
 
void TrainDlg::OnStartTrain()
{ 
    trade_records_.clear();

    account_info_.capital.avaliable = cst_default_ori_capital;
    account_info_.capital.frozen = 0.0;
    account_info_.capital.float_profit = 0.0;
    RefreshCapitalUi();

    ui.le_cur_capital->setText(ToQString(account_info_.capital.avaliable + account_info_.capital.frozen));
    ui.lab_assets->setText(ToQString(account_info_.capital.avaliable + account_info_.capital.frozen));
    //ui.le_long_pos->setText(ToQString(0));
    //ui.le_short_pos->setText(ToQString(0));

    ui.plain_te_record->clear();

    //ui.dbspbBegCapital->setEnabled(false);
     
    ui.pbtnStart->setEnabled(false);
    ui.pbtnStop->setEnabled(true);

    ui.pbtnNextK->setEnabled(true);
      
    /*if( is_started_ )
    {   
         this->hide();
         auto ret = QMessageBox::information(nullptr, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("是否重新开始训练?"), QMessageBox::Yes, QMessageBox::No); 
         if( ret == QMessageBox::No )
         {
             this->showNormal();
             return;
         }
         this->showNormal();
    }*/
    assert(main_win_->OriStepKlineWall());
    assert(main_win_->SubKlineWall());

    int start_day = ui.lab_start_date->text().toInt();
    int start_time = 905;
    //int date = ui.de_begin->date().toString("yyyyMMdd").toInt();
    cur_train_step_ = 0;
    auto p_item = parent_->SetTrainStartDateTime(TypePeriod::PERIOD_5M, start_day, start_time);
    if( p_item )
        cur_train_step_= p_item->hhmmss % 5;
    
    main_win_->OriStepKlineWall()->SetTrainStartDateTime(DEFAULT_ORI_STEP_TYPE_PERIOD, start_day, start_time);
    main_win_->OriStepKlineWall()->ShowDurationKlines(start_day, start_time);

    main_win_->SubKlineWall()->SetTrainStartDateTime(TypePeriod(main_win_->tool_bar_->sub_cycle_comb()->currentData().toInt())
        , start_day, start_time);
    main_win_->SubKlineWall()->right_clicked_k_date(start_day);
    main_win_->SubKlineWall()->right_clicked_k_hhmm(start_time);

    main_win_->SubKlineWall()->slotOpenRelatedSubKwall(false);
    
    is_started_ = true;
}

void TrainDlg::OnStopTrain()
{
    force_close_low_ = MAX_PRICE;
    force_close_high_ = MIN_PRICE;

    ui.pbtnStart->setEnabled(true);
    ui.pbtnStop->setEnabled(false);

    ui.pbtnNextK->setEnabled(false);

    ui.le_cur_capital->setText(ToQString(account_info_.capital.avaliable));
    ui.lab_assets->setText(ToQString(account_info_.capital.avaliable));
}
 
void TrainDlg::OnCloseAllUnfrozenPos()
{
    // todo:
}

//ps: auto ajust account_info_.capital
std::vector<TradeRecordAtom> TrainDlg::DoIfStopProfitLoss(const T_StockHisDataItem &k_item, std::vector<int> &ret_pos_ids, double &ret_profit)
{
    std::vector<TradeRecordAtom>  records;

    if( account_info_.position.TotalPosition() == 0 )
        return records;
    int date = 0;
    int time = 0;
    double capital_ret_stop_profit_long  = 0.0;
    double capital_ret_stop_loss_short   = 0.0;
    double capital_ret_stop_profit_short = 0.0;
    double capital_ret_stop_loss_long    = 0.0;
    if( fabs(k_item.close_price - k_item.high_price) > fabs(k_item.close_price - k_item.low_price) ) 
    {
        // close price is nearby low price: first high price then low price
        double profit_long_pos = 0.0;
        std::vector<int> stop_profit_long_ids;
        std::vector<TradeRecordAtom> trades_stop_profit_long = account_info_.position.DoIfStopProfitLongPos(k_item, capital_ret_stop_profit_long, stop_profit_long_ids, nullptr, &profit_long_pos);
        records.insert(records.end(), trades_stop_profit_long.begin(), trades_stop_profit_long.end());
        ret_pos_ids.insert(ret_pos_ids.end(), stop_profit_long_ids.begin(), stop_profit_long_ids.end());

        double loss_short_pos = 0.0;
        std::vector<int> stop_loss_short_ids;
        std::vector<TradeRecordAtom> trades_stop_loss_short = account_info_.position.DoIfStopLossShortPos(k_item, capital_ret_stop_loss_short, stop_loss_short_ids, nullptr, &loss_short_pos);
        records.insert(records.end(), trades_stop_loss_short.begin(), trades_stop_loss_short.end());
        ret_pos_ids.insert(ret_pos_ids.end(), stop_loss_short_ids.begin(), stop_loss_short_ids.end());

        double profit_short_pos = 0.0;
        std::vector<int> stop_profit_short_ids;
        std::vector<TradeRecordAtom> trades_stop_profit_short = account_info_.position.DoIfStopProfitShortPos(k_item, capital_ret_stop_profit_short, stop_profit_short_ids, nullptr, &profit_short_pos);
        records.insert(records.end(), trades_stop_profit_short.begin(), trades_stop_profit_short.end());
        ret_pos_ids.insert(ret_pos_ids.end(), stop_profit_short_ids.begin(), stop_profit_short_ids.end());

        double loss_long_pos = 0.0;
        std::vector<int> stop_loss_long_ids;
        std::vector<TradeRecordAtom> trades_stop_loss_long = account_info_.position.DoIfStopLossLongPos(k_item, capital_ret_stop_loss_long, stop_loss_long_ids, nullptr, &loss_long_pos);
        records.insert(records.end(), trades_stop_loss_long.begin(), trades_stop_loss_long.end());
        ret_pos_ids.insert(ret_pos_ids.end(), stop_loss_long_ids.begin(), stop_loss_long_ids.end());

        ret_profit = profit_long_pos + loss_short_pos + profit_short_pos + loss_long_pos;
    }else
    { // close price is nearby high price: first low price then high price
        double profit_short_pos = 0.0;
        std::vector<int> stop_profit_short_ids;
        std::vector<TradeRecordAtom> trades_stop_profit_short = account_info_.position.DoIfStopProfitShortPos(k_item, capital_ret_stop_profit_short, stop_profit_short_ids, nullptr, &profit_short_pos);
        records.insert(records.end(), trades_stop_profit_short.begin(), trades_stop_profit_short.end());
        ret_pos_ids.insert(ret_pos_ids.end(), stop_profit_short_ids.begin(), stop_profit_short_ids.end());

        double loss_long_pos = 0.0;
        std::vector<int> stop_loss_long_ids;
        std::vector<TradeRecordAtom> trades_stop_loss_long = account_info_.position.DoIfStopLossLongPos(k_item, capital_ret_stop_loss_long, stop_loss_long_ids, nullptr, &loss_long_pos);
        records.insert(records.end(), trades_stop_loss_long.begin(), trades_stop_loss_long.end());
        ret_pos_ids.insert(ret_pos_ids.end(), stop_loss_long_ids.begin(), stop_loss_long_ids.end());

        double profit_long_pos = 0.0;
        std::vector<int> stop_profit_long_ids;
        std::vector<TradeRecordAtom> trades_stop_profit_long = account_info_.position.DoIfStopProfitLongPos(k_item, capital_ret_stop_profit_long, stop_profit_long_ids, nullptr, &profit_long_pos);
        records.insert(records.end(), trades_stop_profit_long.begin(), trades_stop_profit_long.end());
        ret_pos_ids.insert(ret_pos_ids.end(), stop_profit_long_ids.begin(), stop_profit_long_ids.end());

        double loss_short_pos = 0.0;
        std::vector<int> stop_loss_short_ids;
        std::vector<TradeRecordAtom> trades_stop_loss_short = account_info_.position.DoIfStopLossShortPos(k_item, capital_ret_stop_loss_short, stop_loss_short_ids, nullptr, &loss_short_pos);
        records.insert(records.end(), trades_stop_loss_short.begin(), trades_stop_loss_short.end());
        ret_pos_ids.insert(ret_pos_ids.end(), stop_loss_short_ids.begin(), stop_loss_short_ids.end());

        ret_profit = profit_long_pos + loss_short_pos + profit_short_pos + loss_long_pos;
    }
    for( unsigned int i = 0; i < records.size(); ++i )
    {
        account_info_.capital.frozen -= cst_margin_capital * records.at(i).quantity;
        account_info_.capital.avaliable += cst_margin_capital * records.at(i).quantity + ret_profit;
    }
    return records;
}

void TrainDlg::OnNextStep()
{  
    static auto remove_pos_tableview_closed_atoms = [this](const std::vector<int> &pos_del_ids, bool is_long)
    {
        if( pos_del_ids.empty() )
            return;
        QTableView &tbv = *ui.table_view_position;
        QStandardItemModel * model = static_cast<QStandardItemModel *>(tbv.model());
        auto row_index = find_model_first_fit_index(*model, is_long);
        assert(row_index > -1); 
        QVector<int> ids = model->item(row_index, cst_tbview_position_id)->data().value<QVector<int>>();
        QVector<int> ids_after;
        for( int i = 0; i < ids.size(); ++i )
        {
            auto ret = std::find_if(pos_del_ids.cbegin(), pos_del_ids.cend(),[&ids, i](int id){ return id == ids[i]; });
            if( ret == std::end(pos_del_ids) )
                ids_after.push_back(ids[i]);
        }
        if( ids_after.empty() )
            model->removeRow(row_index);
        else
            RecaculatePosTableViewItem(ids_after, row_index);
    };
    
    //double close_price = MAGIC_STOP_PRICE;
    ++cur_train_step_;
    T_StockHisDataItem item = main_win_->OriStepKlineWall()->Train_NextStep();
    parent_->Train_NextStep(item, cur_train_step_);
    main_win_->SubKlineWall()->Train_NextStep(item, cur_train_step_);

    const T_StockHisDataItem & stock_item = CurHisStockDataItem();
    if( stock_item.date == 0 )
    {
        SetStatusBar(QString::fromLocal8Bit("日期为0, 数据异常!"));
        return;
    }
    cur_quote_ = item.close_price;
    ui.lab_quote->setText(QString::number(item.close_price));
    if( ui.checkb_follow_market->isChecked() )
        ui.dbspb_price->setValue(item.close_price);
     
    //-------------------handle order infos--------------
    /* there are total 3 order types : 
    1. hang on orders 
    2. auto stop profit/loss orders : ps: only allowed to close related position
    3. condition orders: ps: only allowed to open new position
    */ 
    std::vector<TradeRecordAtom> trade_record_atoms;
    std::vector<int> fake_del_ids;
    for(auto iter = hangon_order_infos_.begin(); iter != hangon_order_infos_.end(); )
    {
       if( iter->action == OrderAction::CLOSE )
       {
           std::vector<int> pos_del_ids;
           bool is_fit_price = iter->position_type == PositionType::POS_LONG ? (!(item.high_price < iter->price)) : (!(item.low_price > iter->price));
           if( !is_fit_price )
           {
               ++iter;
               continue;
           }
           double filled_price = iter->price;
           if( iter->position_type == PositionType::POS_LONG )
               filled_price = (item.open_price > iter->price ? item.open_price : iter->price);
           else
               filled_price = (item.open_price < iter->price ? item.open_price : iter->price);
           fake_del_ids.push_back(iter->fake_id);
            // for each related position
            double total_profit = 0.0;
            unsigned int total_decrease_size = 0;
            for( auto posid_size_iter = iter->help_contain.begin(); posid_size_iter != iter->help_contain.end(); ++posid_size_iter )
            {
                auto p_pos_atom = account_info_.position.FindPositionAtom(posid_size_iter->first);
                auto decrease_size = p_pos_atom->DecreaseFrozen(iter->fake_id);
                assert(decrease_size == posid_size_iter->second);
                total_decrease_size += decrease_size;
                if( 0 == p_pos_atom->qty_all() )
                    pos_del_ids.push_back(posid_size_iter->first);

                total_profit += p_pos_atom->PartProfit(filled_price, decrease_size);
            }
            assert(iter->qty == total_decrease_size);

            TradeRecordAtom trade_item;
            trade_item.trade_id = account_info_.position.GenerateTradeId();
            trade_item.date = item.date;
            trade_item.hhmm = item.hhmmss;
            trade_item.action = OrderAction::CLOSE;
            trade_item.pos_type = iter->position_type;
            trade_item.price =  filled_price;
            trade_item.quantity = total_decrease_size;  // close all position of this record 
            trade_item.profit = total_profit; //
            trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
            trade_record_atoms.push_back(trade_item);
            Append2TblTrades(trade_item);

            double capital_ret = cst_margin_capital * total_decrease_size + trade_item.profit - trade_item.fee;
            account_info_.capital.frozen -= cst_margin_capital * total_decrease_size;
            account_info_.capital.avaliable += capital_ret;
            
            RemoveFromTblHangonOrderByFakeId(iter->fake_id);// reset hangon table view

            if( !pos_del_ids.empty() )
            {
                // reset position table view
                remove_pos_tableview_closed_atoms(pos_del_ids, iter->position_type == PositionType::POS_LONG);
                // reset stop profit/loss related
                remove_container_items_by_pos_ids(stop_profit_order_infos_, &pos_del_ids, &pos_del_ids);
                remove_container_items_by_pos_ids(stop_loss_order_infos_, &pos_del_ids, &pos_del_ids);
                UpdateOrders2KlineWalls(ORDER_TYPE_STOPPROFIT);
                UpdateOrders2KlineWalls(ORDER_TYPE_STOPLOSS);
            }

            hangon_order_infos_.erase(iter++);
            continue;

       }else //open 
       {
           bool is_fit_price = iter->position_type == PositionType::POS_LONG ? (!(item.low_price > iter->price)) : (!(item.high_price < iter->price));
           if( is_fit_price )
           { 
               double filled_price = iter->price;
               if( iter->position_type == PositionType::POS_LONG )
                   filled_price = (item.open_price < iter->price ? item.open_price : iter->price);
               else
                   filled_price = (item.open_price > iter->price ? item.open_price : iter->price);
               OpenPosition(filled_price, iter->qty, iter->position_type == PositionType::POS_LONG);
               RemoveFromTblHangonOrderByFakeId(iter->fake_id);
               hangon_order_infos_.erase(iter++);
               continue;
           }
       }
       ++iter;
    } // for each hangon order
    UpdateOrders2KlineWalls(ORDER_TYPE_HANGON);

    //-------for stop profit/loss ----------------
    std::vector<int> ret_position_ids;
    double profit = 0.0;
    auto trade_items = DoIfStopProfitLoss(item, ret_position_ids, profit); // ps: auto ajust account_.capital
    if( !trade_items.empty() )
    {
        std::vector<int> long_zero_size_pos_ids;
        std::vector<int> short_zero_size_pos_ids;
        for( unsigned int i = 0; i < ret_position_ids.size(); ++i )
        {
            auto p_pos_atom = account_info_.position.FindPositionAtom(ret_position_ids.at(i));
            if( p_pos_atom->qty_all() == 0 )
            {
                if( p_pos_atom->is_long ) long_zero_size_pos_ids.push_back(ret_position_ids.at(i));
                else short_zero_size_pos_ids.push_back(ret_position_ids.at(i));
                account_info_.position.RemoveAtom(ret_position_ids.at(i));
            }
        }
        remove_pos_tableview_closed_atoms(long_zero_size_pos_ids, true);
        remove_pos_tableview_closed_atoms(short_zero_size_pos_ids, false);

        std::for_each(trade_items.begin(), trade_items.end(), [this](TradeRecordAtom &trade_item){ Append2TblTrades(trade_item);});

        remove_container_items_by_pos_ids(stop_profit_order_infos_, &long_zero_size_pos_ids, &short_zero_size_pos_ids);
        remove_container_items_by_pos_ids(stop_loss_order_infos_, &long_zero_size_pos_ids, &short_zero_size_pos_ids);

        UpdateOrders2KlineWalls(ORDER_TYPE_STOPPROFIT);
        UpdateOrders2KlineWalls(ORDER_TYPE_STOPLOSS);
    } 

    //-------------------

    double total_profit = RecaculatePosTableViewFloatProfit(item.close_price);
    account_info_.capital.float_profit = ProcDecimal(total_profit, 0);

    RefreshCapitalUi();

    // status bar ----------------------------
    int hh = item.hhmmss/100;
    int mm = item.hhmmss%100;
    int ss = 0;
    int min_5_amain_second = (5 - (hh * 60 + mm) % 5) * 60 - ss; 
    int min_15_amain_second = (15 - (hh * 60 + mm) % 15) * 60 - ss; 
    QString content = QString("%1                                         %2                 5M: %3     \t\t     15M: %4")
        .arg(QString("%1:%2:00").arg(hh,2,10,QLatin1Char('0')).arg(mm,2,10,QLatin1Char('0')))
        .arg(QString::number(item.close_price, 'f', 1))
        .arg(min_5_amain_second)
        .arg(min_15_amain_second);
    main_win_->statusBar()->showMessage(content);
}

void TrainDlg::RemoveInPositionTableView(int position_id, PositionType position_type)
{
    QTableView &tbv = *ui.table_view_position;
    QStandardItemModel * model = static_cast<QStandardItemModel *>(tbv.model());
    auto row_index = find_model_first_fit_index(*model, position_type == PositionType::POS_LONG);
    QVector<int> ids = model->item(row_index, cst_tbview_position_id)->data().value<QVector<int>>();
    auto target_item = std::find_if(std::begin(ids), std::end(ids),[position_id](int id){ return id == position_id; });
    //assert( target_item != std::end(ids) );
    ids.erase(target_item);
    if( ids.empty() )
        model->removeRow(row_index);
    else
    {
        RecaculatePosTableViewItem(ids, row_index);
    }
}

void TrainDlg::RecaculatePosTableViewItem(QVector<int> &ids, int row_index)
{
    QTableView &tbv = *ui.table_view_position;
    QStandardItemModel * model = static_cast<QStandardItemModel *>(tbv.model());
    QVariant var_data;
    var_data.setValue(ids);
    model->item(row_index, cst_tbview_position_id)->setData(var_data);
    //recaculate average open price-------
    double total_amount = 0.0;
    unsigned int total_qty = 0;
    unsigned int avaliable_qty = 0;
    for( int i = 0; i < ids.size(); ++i )
    {
        auto pos_item = account_info_.position.FindPositionAtom(ids[i]);
        if( pos_item )
        {
            total_amount += pos_item->qty_all() * pos_item->price;
            total_qty += pos_item->qty_all();
            avaliable_qty += pos_item->qty_available;
        }
    }
    assert(total_qty > 0);
    double after_average_price = ProcDecimal(total_amount/total_qty, DEFAULT_DECIMAL + 1);

    model->item(row_index, cst_tbview_position_average_price)->setText(QString::number(after_average_price));
    model->item(row_index, cst_tbview_position_size)->setText(QString::number(total_qty));
    model->item(row_index, cst_tbview_position_avaliable)->setText(QString::number(avaliable_qty));
}

void TrainDlg::RecaculatePosTableViewItem(int row_index)
{
    QTableView &tbv = *ui.table_view_position;
    QStandardItemModel * model = static_cast<QStandardItemModel *>(tbv.model());
     
    QVector<int> ids = model->item(row_index, cst_tbview_position_id)->data().value<QVector<int>>();
    RecaculatePosTableViewItem(ids, row_index);
}

// ps: auto set  profit ui item
double TrainDlg::RecaculatePosTableViewFloatProfit(double cur_price)
{
    double total_profit = 0.0;
    QTableView &tbv = *ui.table_view_position;
    QStandardItemModel * model = static_cast<QStandardItemModel *>(tbv.model());
    for( int i = 0; i < model->rowCount(); ++i )
    {
        bool is_long_pos = model->item(i, cst_tbview_position_bs)->data().toBool();
        auto position_size = model->item(i, cst_tbview_position_size)->text().toInt();
        auto average_price = model->item(i, cst_tbview_position_average_price)->text().toDouble();

        double profit = 0.0;
        if( is_long_pos )
            profit = (cur_price - average_price) / cst_per_tick * cst_per_tick_capital * position_size;
        else
            profit = (average_price - cur_price) / cst_per_tick * cst_per_tick_capital * position_size;

        profit = ProcDecimal(profit, 0);
        total_profit += profit;
        //ProcDecimal(allow_highest_price, DEFAULT_DECIMAL);
        auto profit_item = model->item(i, cst_tbview_position_float_profit);
        profit_item->setText(QString::number(profit));
    }
    return total_profit;
}

void TrainDlg::UpdateOrders2KlineWalls(int type)
{
    if( type == ORDER_TYPE_ALL )
    { 
        parent_->hangon_order_infos_ = hangon_order_infos_;
        parent_->stop_profit_order_infos_ = stop_profit_order_infos_;
        parent_->stop_loss_order_infos_ = stop_loss_order_infos_;
        parent_->condition_order_infos_ = condition_order_infos_;
        if( main_win_->SubKlineWall() )
        {
            main_win_->SubKlineWall()->hangon_order_infos_ = hangon_order_infos_;
            main_win_->SubKlineWall()->stop_profit_order_infos_ = stop_profit_order_infos_;
            main_win_->SubKlineWall()->stop_loss_order_infos_ = stop_loss_order_infos_;
            main_win_->SubKlineWall()->condition_order_infos_ = condition_order_infos_;
        }
    }else if( type == ORDER_TYPE_HANGON )
    {
        parent_->hangon_order_infos_ = hangon_order_infos_; 
        if( main_win_->SubKlineWall() )
        {
            main_win_->SubKlineWall()->hangon_order_infos_ = hangon_order_infos_; 
        }
    }else if( type == ORDER_TYPE_STOPPROFIT )
    { 
        parent_->stop_profit_order_infos_ = stop_profit_order_infos_; 
        if( main_win_->SubKlineWall() )
        {
            main_win_->SubKlineWall()->stop_profit_order_infos_ = stop_profit_order_infos_; 
        }
    }else if( type == ORDER_TYPE_STOPLOSS )
    { 
        parent_->stop_loss_order_infos_ = stop_loss_order_infos_; 
        if( main_win_->SubKlineWall() )
        {
            main_win_->SubKlineWall()->stop_loss_order_infos_ = stop_loss_order_infos_; 
        }
    }else if( type == ORDER_TYPE_CONDITION )
    { 
        parent_->condition_order_infos_ = condition_order_infos_;
        if( main_win_->SubKlineWall() )
        {
            main_win_->SubKlineWall()->condition_order_infos_ = condition_order_infos_;
        }
    }

    parent_->update();
    if( main_win_->SubKlineWall() )
        main_win_->SubKlineWall()->update();
}

int TrainDlg::TblHangonOrdersRowCount()
{
    auto model = (QStandardItemModel*)ui.table_view_order_hangon->model();
    return model->rowCount();
}

void TrainDlg::Append2TblHangonOrders(OrderInfo &order_info)
{
    auto model = (QStandardItemModel*)ui.table_view_order_hangon->model();
    model->insertRow(model->rowCount());
    int row_index = model->rowCount() - 1;
    auto item = new QStandardItem(QString::number(order_info.fake_id));
    model->setItem(row_index, cst_tbview_hangonorder_id, item);

    QString bs_str;
    if( order_info.action == OrderAction::OPEN )
        bs_str = QString::fromLocal8Bit(order_info.position_type == PositionType::POS_LONG ? "买" : "卖");
    else
        bs_str = QString::fromLocal8Bit(order_info.position_type == PositionType::POS_LONG ? "卖" : "买");
    item = new QStandardItem(bs_str);
    model->setItem(row_index, cst_tbview_hangonorder_bs, item);

    item = new QStandardItem(QString::fromLocal8Bit(order_info.action == OrderAction::CLOSE ? "平" : "开"));
    model->setItem(row_index, cst_tbview_hangonorder_oc, item);

    item = new QStandardItem(QString::number(order_info.price));
    model->setItem(row_index, cst_tbview_hangonorder_price, item);

    item = new QStandardItem(QString::number(order_info.qty));
    model->setItem(row_index, cst_tbview_hangonorder_qty, item);
}

void TrainDlg::RemoveFromTblHangonOrderByFakeId(int fake_id)
{
    auto model = (QStandardItemModel*)ui.table_view_order_hangon->model();
    for( int i = 0; i < model->rowCount(); ++i )
    {
        if( model->item(i, cst_tbview_hangonorder_id)->text().toInt() == fake_id )
        {
            model->removeRow(i);
            break;
        }
    }
}

void TrainDlg::Append2TblTrades(TradeRecordAtom &trade)
{
    auto model = (QStandardItemModel*)ui.table_view_trades->model();
    model->insertRow(model->rowCount());
    unsigned int row_index = model->rowCount() - 1;
    auto item = new QStandardItem(QString::number(trade.trade_id));
    model->setItem(row_index, cst_tbview_trades_id, item);

    QString bs_tag;
    if( trade.action == OrderAction::OPEN )
        bs_tag = trade.pos_type == PositionType::POS_LONG ? QString::fromLocal8Bit("买") : QString::fromLocal8Bit("卖");
    else
        bs_tag = trade.pos_type == PositionType::POS_LONG ? QString::fromLocal8Bit("卖") : QString::fromLocal8Bit("买");
    item = new QStandardItem(bs_tag);
    model->setItem(row_index, cst_tbview_trades_bs, item);

    item = new QStandardItem(trade.action == OrderAction::OPEN ? QString::fromLocal8Bit("开") : QString::fromLocal8Bit("平"));
    model->setItem(row_index, cst_tbview_trades_oc, item);

    item = new QStandardItem(QString::number(trade.price));
    model->setItem(row_index, cst_tbview_trades_price, item);

    item = new QStandardItem(QString::number(trade.quantity));
    model->setItem(row_index, cst_tbview_trades_qty, item);

    item = new QStandardItem(QString::number(trade.fee));
    model->setItem(row_index, cst_tbview_trades_fee, item);

    item = new QStandardItem(QString::number(trade.profit));
    model->setItem(row_index, cst_tbview_trades_profit, item);

    item = new QStandardItem(QString::number(trade.hhmm));
    model->setItem(row_index, cst_tbview_trades_time, item);
}

void TrainDlg::RefreshCapitalUi()
{
    ui.label_capital->setText(QString::number(account_info_.capital.avaliable + account_info_.capital.frozen + account_info_.capital.float_profit));
    ui.label_capital_available->setText(QString::number(account_info_.capital.avaliable));

    ui.label_close_profit->setText(QString::number(account_info_.capital.avaliable + account_info_.capital.frozen - ori_capital_));
    ui.label_float_profit->setText(QString::number(account_info_.capital.float_profit));
}  

void TrainDlg::OnBuy()
{
    SetStatusBar("");
    const double market_price = ui.lab_quote->text().toDouble();
    const double order_price = ui.dbspb_price->text().toDouble();
    if( market_price < EPSINON )
    {
        SetStatusBar(QString::fromLocal8Bit("市价异常!"));
        return;
    } 
    if( ui.radio_postion_o->isChecked() ) // buy to open long position 
    {
        if( !(order_price < market_price) )
            OpenPosition(market_price, ui.spb_order_num->value(), true);
        else
            AddOpenOrder(ui.dbspb_price->value(), ui.spb_order_num->value(), true);
    }else // buy to close short position
    {
        if( !(order_price < market_price) )
            CloseInputSizePosition(market_price, false);
        else
            AddCloseOrder(ui.dbspb_price->value(), ui.spb_order_num->value(), false);
    }
    RefreshCapitalUi();
}

void TrainDlg::OnSell()
{
    SetStatusBar("");
    const double market_price = ui.lab_quote->text().toDouble();
    const double order_price = ui.dbspb_price->text().toDouble();
    if( market_price < EPSINON )
    {
        SetStatusBar(QString::fromLocal8Bit("市价异常!"));
        return;
    }
    if( ui.radio_postion_o->isChecked() ) // sell to open short position 
    {
        if( !(order_price > market_price) )
            OpenPosition(market_price, ui.spb_order_num->value(), false);
        else
            AddOpenOrder(ui.dbspb_price->value(), ui.spb_order_num->value(), false);
    }else // sell to close long position
    {
        if( !(order_price > market_price) )
            CloseInputSizePosition(market_price, true);
        else
            AddCloseOrder(ui.dbspb_price->value(), ui.spb_order_num->value(), true);
    }
    RefreshCapitalUi();
}

void TrainDlg::SaveStopProfitLoss(std::vector<PositionAtom> &pos_atoms)
{
    assert(!pos_atoms.empty());
    for( unsigned int i = 0; i < pos_atoms.size(); ++i )
    {
       auto pos_item = account_info_.position.FindPositionAtomSharedPointer(pos_atoms[i].trade_id);
       if( !pos_item || pos_item->qty_available == 0 )
           continue;
       pos_item->stop_profit_price = pos_atoms[i].stop_profit_price;
       pos_item->stop_loss_price = pos_atoms[i].stop_loss_price;
       
       auto target_order_item = std::find_if(stop_profit_order_infos_.begin(), stop_profit_order_infos_.end(), [&](OrderInfo &order_info){ return order_info.rel_position_id == pos_atoms[i].trade_id; });
       if( target_order_item != stop_profit_order_infos_.end() )
           target_order_item->price = pos_atoms[i].stop_profit_price;

       target_order_item = std::find_if(stop_loss_order_infos_.begin(), stop_loss_order_infos_.end(), [&](OrderInfo &order_info){ return order_info.rel_position_id == pos_atoms[i].trade_id; });
       if( target_order_item != stop_loss_order_infos_.end() )
           target_order_item->price = pos_atoms[i].stop_loss_price;
    }
    UpdateOrders2KlineWalls(ORDER_TYPE_STOPPROFIT);
    UpdateOrders2KlineWalls(ORDER_TYPE_STOPLOSS);
}

void TrainDlg::OpenPosition(double para_price, unsigned int qty, bool is_long)
{ 
    assert(qty > 0);
    double price = para_price; 
    double capital_buy = cst_margin_capital * qty;
    double fee = CalculateFee(qty, price, false);
    if( capital_buy + fee > account_info_.capital.avaliable )
    {
        SetStatusBar(QString::fromLocal8Bit("可用资金不足!"));
        return;
    }
    account_info_.capital.avaliable -= capital_buy + fee; 
    account_info_.capital.frozen += capital_buy;

    auto pos_atom = std::make_shared<PositionAtom>();
    pos_atom->trade_id = account_info_.position.GenerateTradeId();
    pos_atom->is_long = is_long;
    pos_atom->price = price;
    pos_atom->qty_available = qty;
    
    account_info_.position.PushBack(is_long, pos_atom);

    TradeRecordAtom  trade_item;
    trade_item.trade_id = pos_atom->trade_id;
    trade_item.date = 0;
    trade_item.hhmm = 0;
    trade_item.action = OrderAction::OPEN;
    trade_item.pos_type = is_long ? PositionType::POS_LONG : PositionType::POS_SHORT;
    trade_item.quantity = qty;
    trade_item.price = pos_atom->price;
    trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
    trade_records_.push_back(trade_item);
    Append2TblTrades(trade_item);

    // fill or ajust position table view -------------
    QTableView &tbv = *ui.table_view_position;
    QStandardItemModel * model = static_cast<QStandardItemModel *>(tbv.model());
    auto align_way = Qt::AlignCenter;
    QVariant var_ids;
    QVector<int> ids;
    auto row_index = find_model_first_fit_index(*model, pos_atom->is_long);
    if( row_index > -1 ) 
    {   // merge long position
        ids = model->item(row_index, cst_tbview_position_id)->data().value<QVector<int>>();
        ids.push_back(pos_atom->trade_id);
        var_ids.setValue(ids);
        model->item(row_index, cst_tbview_position_id)->setData(var_ids);
         
        unsigned int total_qty = is_long ? account_info_.position.LongPosQty(POSITION_STATUS_ALL) : account_info_.position.ShortPosQty(POSITION_STATUS_ALL);
        assert(total_qty > 0); 
        unsigned int ava_qty = is_long ? account_info_.position.LongPosQty(POSITION_STATUS_AVAILABLE) : account_info_.position.ShortPosQty(POSITION_STATUS_AVAILABLE);
        double avarege_price = is_long ? account_info_.position.LongAveragePrice() : account_info_.position.ShortAveragePirce();
        model->item(row_index, cst_tbview_position_average_price)->setText(QString::number(avarege_price));
        model->item(row_index, cst_tbview_position_size)->setText(QString::number(total_qty));
        model->item(row_index, cst_tbview_position_avaliable)->setText(QString::number(ava_qty));

    }else
    {
        model->insertRow(model->rowCount());
        row_index = model->rowCount() - 1;

        ids.push_back(pos_atom->trade_id);
        var_ids.setValue(ids);
        auto item = new QStandardItem( QString::number(pos_atom->trade_id) );
        item->setData(var_ids);
        model->setItem(row_index, cst_tbview_position_id, item);
        model->item(row_index, cst_tbview_position_id)->setTextAlignment(align_way);

        item = new QStandardItem( QString::number(pos_atom->price) );
        model->setItem(row_index, cst_tbview_position_average_price, item);
        model->item(row_index, cst_tbview_position_average_price)->setTextAlignment(align_way);

        item = new QStandardItem( QString::number(0) );
        model->setItem(row_index, cst_tbview_position_float_profit, item);
        model->item(row_index, cst_tbview_position_float_profit)->setTextAlignment(align_way);

        item = new QStandardItem( QString::fromLocal8Bit(pos_atom->is_long ? "买" : "卖") );
        item->setData(QVariant(pos_atom->is_long));
        model->setItem(row_index, cst_tbview_position_bs, item);
        model->item(row_index, cst_tbview_position_bs)->setTextAlignment(align_way);

        item = new QStandardItem( QString::number(pos_atom->qty_all()) );
        model->setItem(row_index, cst_tbview_position_size, item);
        model->item(row_index, cst_tbview_position_size)->setTextAlignment(align_way);

        item = new QStandardItem( QString::number(pos_atom->qty_available) );
        model->setItem(row_index, cst_tbview_position_avaliable, item);
        model->item(row_index, cst_tbview_position_avaliable)->setTextAlignment(align_way);
    }
    // consider auto stop (profit/loss) order -------------------
    // ps: auto set PositionAtom's stop profit/loss price
    auto append_stop_order = [this](PositionAtom &pos_atom, double price, bool is_long, bool is_stop_loss)
    {
        OrderInfo order(is_stop_loss ? OrderType::STOPLOSS : OrderType::STOPPROFIT);
        order.rel_position_id = pos_atom.trade_id;
        order.action = OrderAction::CLOSE;
        // target position type
        order.position_type = is_long ? PositionType::POS_LONG : PositionType::POS_SHORT;
        order.qty = pos_atom.qty_available;
        if( is_stop_loss )
        {
            order.price = is_long ? (price - cst_per_tick * (double)auto_stop_loss_ticks_) : (price + cst_per_tick * (double)auto_stop_loss_ticks_);
            pos_atom.stop_loss_price = order.price;
            stop_loss_order_infos_.push_back(order);
        }else // stop profit
        {
            order.price = is_long ? (price + cst_per_tick * (double)auto_stop_profit_ticks_) : (price - cst_per_tick * (double)auto_stop_profit_ticks_);
            pos_atom.stop_profit_price = order.price;
            stop_profit_order_infos_.push_back(order);
        }
        
    };
    
    if( auto_stop_profit_ )
    {
        append_stop_order(*pos_atom, price, is_long, false);
        UpdateOrders2KlineWalls(ORDER_TYPE_STOPPROFIT);
    }
    if( auto_stop_loss_ )
    {
        append_stop_order(*pos_atom, price, is_long, true); 
        UpdateOrders2KlineWalls(ORDER_TYPE_STOPLOSS);
    }
     
}


void TrainDlg::CloseInputSizePosition(double para_price, bool is_long)
{
    int close_qty = ui.spb_order_num->value();
    assert(close_qty > 0);
    QTableView &tbv = *ui.table_view_position;
    QStandardItemModel * model = static_cast<QStandardItemModel *>(tbv.model());
    auto row_index = find_model_first_fit_index(*model, is_long);
    if( row_index < 0 ) 
    { 
        return SetStatusBar(is_long ? QString::fromLocal8Bit("无多头仓位可平!") : QString::fromLocal8Bit("无空头仓位可平!"));
    }
     
    auto total_qty = is_long ? account_info_.position.LongPosQty(POSITION_STATUS_AVAILABLE) : account_info_.position.ShortPosQty(POSITION_STATUS_AVAILABLE);
    if( (unsigned int)close_qty > total_qty )
         return SetStatusBar(QString::fromLocal8Bit("平仓数量超过现有仓位!"));
    QVector<int> ids = model->item(row_index, cst_tbview_position_id)->data().value<QVector<int>>();
    
    int date = 0;
    int hhmm = 0;//ndedt
    double capital_ret = 0.0;
    double profit_close = 0.0;
    std::vector<int> position_ids;
    std::vector<TradeRecordAtom> trade_record_atoms;
    if( is_long )
    {
        trade_record_atoms = account_info_.position.CloseAvaliableLong(para_price, close_qty, capital_ret, &profit_close, &position_ids);
    }else
    {
        trade_record_atoms = account_info_.position.CloseAvaliableShort(para_price, close_qty, capital_ret, &profit_close, &position_ids);
    }
    if( trade_record_atoms.empty() )
    {
        return SetStatusBar(QString::fromLocal8Bit("此平仓失效!"));
    }
    remove_container_items_by_pos_ids(stop_profit_order_infos_, (is_long ? &position_ids : nullptr), (is_long ? nullptr : &position_ids));
    remove_container_items_by_pos_ids(stop_loss_order_infos_, (is_long ? &position_ids : nullptr), (is_long ? nullptr : &position_ids));

    UpdateOrders2KlineWalls(ORDER_TYPE_STOPPROFIT);
    UpdateOrders2KlineWalls(ORDER_TYPE_STOPLOSS);

    //capital------------
    account_info_.capital.frozen -= capital_ret;
    account_info_.capital.avaliable += capital_ret + profit_close;

    // reset position table view----------
    QVector<int> ids_after;
    for( int i = 0; i < ids.size(); ++i )
    {
        auto ret = std::find_if(std::begin(position_ids), std::end(position_ids),[&ids, i](int id){ return id == ids[i]; });
        if( ret == std::end(position_ids) )
            ids_after.push_back(ids[i]);
    }
    if( ids_after.empty() )
        model->removeRow(row_index);
    else
    { 
        RecaculatePosTableViewItem(ids_after, row_index); 
    }
     
    // recaculae float prift ------
    double cur_quote = ui.lab_quote->text().toDouble();
    double total_profit = RecaculatePosTableViewFloatProfit(cur_quote);
    account_info_.capital.float_profit = ProcDecimal(total_profit, 0);

    // ------------------
    std::for_each(std::begin(trade_record_atoms), std::end(trade_record_atoms),[this](TradeRecordAtom &trade_atom)
    {
        Append2TblTrades(trade_atom);
    });
}

bool TrainDlg::AddOpenOrder(double price, unsigned int quantity, bool is_long)
{ 
    double capital_buy = cst_margin_capital * quantity;
    double fee = CalculateFee(quantity, price, false);
    if( capital_buy + fee > account_info_.capital.avaliable )
    {
        SetStatusBar(QString::fromLocal8Bit("可用资金不足!"));
        return false;
    }
    account_info_.capital.avaliable -= capital_buy;
    account_info_.capital.frozen += capital_buy ;

    OrderInfo order(OrderType::HANGON);
    order.fake_id = TblHangonOrdersRowCount();  
    order.action = OrderAction::OPEN;
    order.position_type = is_long ? PositionType::POS_LONG : PositionType::POS_SHORT;
    order.qty = quantity;
    order.price = price;
    hangon_order_infos_.push_back(order);
    Append2TblHangonOrders(order);
    UpdateOrders2KlineWalls(ORDER_TYPE_HANGON);
    return true;
}

bool TrainDlg::AddCloseOrder(double price, unsigned int quantity, bool is_long)
{
    unsigned int rel_pos_size = is_long ? account_info_.position.LongPosQty(POSITION_STATUS_AVAILABLE) : account_info_.position.ShortPosQty(POSITION_STATUS_AVAILABLE);
    if( quantity > rel_pos_size )
    {
        SetStatusBar(is_long ? QString::fromLocal8Bit("无多头仓位可平!") : QString::fromLocal8Bit("无空头仓位可平!"));
        return false;
    }

    //---------------------------------------
    OrderInfo order(OrderType::HANGON);
    order.fake_id = TblHangonOrdersRowCount();
    order.action = OrderAction::CLOSE;
    order.position_type = is_long ? PositionType::POS_LONG : PositionType::POS_SHORT;
    order.qty = quantity;
    order.price = price;
    // froze avaliable position
    std::unordered_map<int, unsigned int> rel_ids_sizes = is_long ? account_info_.position.LongPosSizeInfo(POSITION_STATUS_AVAILABLE) : account_info_.position.ShortPosSizeInfo(POSITION_STATUS_AVAILABLE);
    unsigned int remain_qty = quantity;

    for( auto iter = rel_ids_sizes.begin(); iter != rel_ids_sizes.end() && remain_qty > 0; ++iter )
    {
        PositionAtom * p_target_atom = account_info_.position.FindPositionAtom(iter->first);
        assert( p_target_atom );
        if( p_target_atom->qty_available >= remain_qty )
        {
            p_target_atom->Freeze(order.fake_id, remain_qty);
            order.help_contain.insert(std::make_pair(p_target_atom->trade_id, remain_qty));
            remain_qty = 0;
            break;
        }else
        {
            p_target_atom->Freeze(order.fake_id, p_target_atom->qty_available);
            order.help_contain.insert(std::make_pair(p_target_atom->trade_id, p_target_atom->qty_available));
            remain_qty -= (int)p_target_atom->qty_available;
        }
    }

    hangon_order_infos_.push_back(order);

    Append2TblHangonOrders(order);

    // ----------update table_view_position 
    QTableView &tbv = *ui.table_view_position;
    QStandardItemModel * model = static_cast<QStandardItemModel *>(tbv.model());
    auto row_index = find_model_first_fit_index(*model, is_long);
    if( row_index > -1 ) 
    {  
        unsigned int ava_qty = is_long ? account_info_.position.LongPosQty(POSITION_STATUS_AVAILABLE) : account_info_.position.ShortPosQty(POSITION_STATUS_AVAILABLE);
        model->item(row_index, cst_tbview_position_avaliable)->setText(QString::number(ava_qty));
    }
     
    UpdateOrders2KlineWalls(ORDER_TYPE_HANGON);
    return true;
}

unsigned int TrainDlg::GetItemPositionAllQty(QStandardItemModel& model, int row_index)
{ 
    unsigned int total_qty = 0;
    QVector<int> ids = model.item(row_index, cst_tbview_position_id)->data().value<QVector<int>>();
    for( int i = 0; i < ids.size(); ++i )
    {
        PositionAtom *position_atom = account_info_.position.FindPositionAtom(ids[i]);
        assert(position_atom);
        total_qty += position_atom->qty_all();
    }
    return total_qty;
}

void TrainDlg::PrintTradeRecords()
{
    ui.plain_te_record->clear();

    QString records_str;
    for(unsigned int i = 0; i < trade_records_.size(); ++i )
    {
        records_str.append(trade_records_.at(i).ToQStr());
        records_str.append("\n");
    }
    ui.plain_te_record->setPlainText(records_str);
    //ui.plain_te_record->setText()
}

bool erase_rel_item_by_pos_ids(std::list<OrderInfo> &order_info, std::list<OrderInfo>::iterator &order_item, std::vector<int> &target_del_ids )
{
    auto ret = std::find_if(target_del_ids.begin(), target_del_ids.end(), [&order_item](int id){ return id == order_item->rel_position_id; });
    if( ret != target_del_ids.end() )
    {
        order_info.erase(order_item++);
        return true;
    }else
        return false;
}

bool erase_rel_item_by_fake_ids(std::list<OrderInfo> &order_info, std::list<OrderInfo>::iterator &order_item, std::vector<int> &target_del_ids)
{
    auto ret = std::find_if(target_del_ids.begin(), target_del_ids.end(), [&order_item](int id){ return id == order_item->fake_id; });
    if( ret != target_del_ids.end() )
    {
        order_info.erase(order_item++);
        return true;
    }else
        return false;
};

void remove_container_items_by_pos_ids(std::list<OrderInfo> &container, std::vector<int> *long_deleted_ids, std::vector<int> *short_deleted_ids)
{
    for( auto order_item = container.begin(); order_item != container.end(); )
    {
        if( long_deleted_ids && erase_rel_item_by_pos_ids(container, order_item, *long_deleted_ids) )
            continue;
        if( short_deleted_ids && erase_rel_item_by_pos_ids(container, order_item, *short_deleted_ids) )
            continue;
        ++order_item;
    }
}

int find_model_first_fit_index(QStandardItemModel& model, bool is_long_pos)
{
    for(int j = 0; j < model.rowCount(); ++j )
    {
        if( model.item(j, cst_tbview_position_bs)->data().toBool() == is_long_pos )
            return j;
    }
    return -1;
}

std::tuple<double, unsigned int> get_total_amount_qty(PositionInfo &position, QVector<int> &ids)
{
    double total_amount = 0.0;
    unsigned int total_qty = 0;
    for( int i = 0; i < ids.size(); ++i )
    {
        auto pos_item = position.FindPositionAtom(ids[i]);
        if( pos_item )
        {
            total_amount += pos_item->qty_all() * pos_item->price;
            total_qty += pos_item->qty_all();
        }
    }
    return std::make_tuple(total_amount, total_qty);
}

