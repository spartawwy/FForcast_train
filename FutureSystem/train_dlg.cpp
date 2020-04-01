#include "train_dlg.h"

#include <QtWidgets/QComboBox>
#include <QMessageBox>
#include <QDebug>

#include <TLib/core/tsystem_utility_functions.h>

#include "database.h"
#include "exchange_calendar.h"

#include "futures_forecast_app.h"
#include "mainwindow.h"
#include "tool_bar.h"
#include "kline_wall.h"

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
static const int cst_tbview_hangonorder_price = 2;
static const int cst_tbview_hangonorder_qty = 3;
static const int cst_tbview_hangonorder_col_count = 4;

static int find_model_first_fit_index(QStandardItemModel& model, bool is_long_pos);
static std::tuple<double, unsigned int> get_total_amount_qty(PositionInfo &position, QVector<int> &ids);

//////////////////////////////////////////
TrainDlg::TrainDlg(KLineWall *parent,  MainWindow *main_win)
    : QWidget(nullptr)
    , parent_(parent)
    , main_win_(main_win)
    , trade_dlg_(this, true)
    , is_started_(false)
    , account_info_()
    , force_close_low_(MAX_PRICE)
    , force_close_high_(MIN_PRICE) 
    , scroll_bar_date_(0)
    , auto_stop_profit_(false)
    , auto_stop_loss_(false)
    , auto_stop_profit_ticks_(10)
    , auto_stop_loss_ticks_(10)
{
    ui.setupUi(this);

    trade_dlg_.setWindowFlags(trade_dlg_.windowFlags() | Qt::WindowStaysOnTopHint/*Qt::Dialog*/ );
    bool ret = false;
    //ui.dateEditTrainBeg->mousePressEvent
    ret = connect(ui.pbtnStart, SIGNAL(clicked()), this, SLOT(OnStartTrain()));
    ret = connect(ui.pbtnStop, SIGNAL(clicked()), this, SLOT(OnStopTrain()));
    ret = connect(ui.pbtnNextK, SIGNAL(clicked()), this, SLOT(OnMoveToNextK()));
    ret = connect(ui.pbtnNextStep, SIGNAL(clicked()), this, SLOT(OnNextStep()));
     
    ret = connect(trade_dlg_.ui.pbt_trade, SIGNAL(clicked()), this, SLOT(OnTrade()));
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
    ui.lab_start_date->setText(QString::number(eldest_date));

    scroll_bar_date_ = eldest_date;
    ret = connect(ui.hScrollBar_TrainTimeRange, SIGNAL(sliderMoved(int)), this, SLOT(OnScrollTrainTimeMoved(int)));
    //-----------------------------------------

    //------------------table position
    ui.table_view_position->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //ret = connect(ui.table_view_position, SIGNAL(doubleClicked(const QModelIndex)), this, SLOT(DoTabTasksDbClick(const QModelIndex)));

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

    //--------------------table hangon order------ 

    
    ui.table_view_order_hangon->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    model = new QStandardItemModel(0, cst_tbview_hangonorder_col_count, this);
    model->setHorizontalHeaderItem(cst_tbview_hangonorder_id, new QStandardItem(QString::fromLocal8Bit("ID")));
    model->horizontalHeaderItem(cst_tbview_hangonorder_id)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_hangonorder_bs, new QStandardItem(QString::fromLocal8Bit("买卖")));
    model->horizontalHeaderItem(cst_tbview_hangonorder_bs)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_hangonorder_price, new QStandardItem(QString::fromLocal8Bit("价格")));
    model->horizontalHeaderItem(cst_tbview_hangonorder_price)->setTextAlignment(Qt::AlignCenter);
    model->setHorizontalHeaderItem(cst_tbview_hangonorder_qty, new QStandardItem(QString::fromLocal8Bit("数量")));
    model->horizontalHeaderItem(cst_tbview_hangonorder_qty)->setTextAlignment(Qt::AlignCenter);
    ui.table_view_order_hangon->setModel(model);
    ui.table_view_order_hangon->setColumnWidth(cst_tbview_hangonorder_id, cst_small_width/2);
    ui.table_view_order_hangon->setColumnWidth(cst_tbview_hangonorder_bs, cst_small_width/2);
    ui.table_view_order_hangon->setColumnWidth(cst_tbview_hangonorder_qty, cst_small_width);
    ui.table_view_order_hangon->setColumnWidth(cst_tbview_hangonorder_price, cst_small_width);
    ret = connect(ui.table_view_order_hangon, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onTblHangonOrdersRowDoubleClicked(const QModelIndex &)));
     
    
    OnStopTrain();

    account_info_.capital.avaliable = cst_default_ori_capital;
    account_info_.capital.frozen = 0.0;
    account_info_.capital.float_profit = 0.0;
    RefreshCapitalUi();
    //ui.dbspbFeeRate->setValue(cst_default_fee_rate_percent);
}

const T_StockHisDataItem & TrainDlg::CurHisStockDataItem()
{
    return parent_->CurTrainStockDataItem();
}

void TrainDlg::OnTblHangonOrdersRowDoubleClicked(const QModelIndex &index)
{
    auto model = (QStandardItemModel*)ui.table_view_order_hangon->model();
    auto temp_order_id = model->item(index.row(), cst_tbview_hangonorder_id)->text().toInt();
    for( auto iter = hangon_order_infos_.begin(); iter != hangon_order_infos_.end(); )
    {
        if( iter->rel_position_id == temp_order_id )
        {
            if( iter->action == OrderAction::OPEN )
            {
                account_info_.capital.frozen -= cst_margin_capital * iter->qty;
                account_info_.capital.avaliable += cst_margin_capital * iter->qty;;
                RefreshCapitalUi();
            }else  // close
            {
                // unfroze related positions
                for(int i = 0; iter->help_contain.size(); ++i )
                {
                    auto pos_item = account_info_.position.FindPositionAtom(iter->help_contain.at(i));
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
            }
            hangon_order_infos_.erase(iter++);
            break;
        }
        ++iter;
    }
    model->removeRow(index.row());
    parent_->hangon_order_infos_ = hangon_order_infos_;
    main_win_->SubKlineWall()->hangon_order_infos_ = hangon_order_infos_;
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

//void TrainDlg::hideEvent(QHideEvent * event)
//{
//    main_win_->is_train_mode(false);
//    parent_->k_rend_index_for_train_ = 0;
//    main_win_->tool_bar()->main_cycle_comb()->setEnabled(true);
//}

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

    /*ui.pbtnBuy->setEnabled(true);
    ui.pbtnSell->setEnabled(true);*/

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

    int start_day = ui.lab_start_date->text().toInt();
    int start_time = 905;
    //int date = ui.de_begin->date().toString("yyyyMMdd").toInt();
    //int time = ui.de_begin->time().toString("hhmm").toInt();
     
    //parent_->SetTrainStartDateTime(TypePeriod(main_win_->tool_bar_->main_cycle_comb()->currentData().toInt()), start_day, start_time);
    auto p_item = parent_->SetTrainStartDateTime(TypePeriod::PERIOD_5M, start_day, start_time);
    if( p_item )
        parent_->cur_train_step(p_item->hhmmss % 5);
    if( main_win_->SubKlineWall() )
    {
        main_win_->SubKlineWall()->cur_train_step(parent_->cur_train_step());
        main_win_->SubKlineWall()->SetTrainStartDateTime(TypePeriod(main_win_->tool_bar_->sub_cycle_comb()->currentData().toInt())
            , start_day, start_time);
        main_win_->SubKlineWall()->right_clicked_k_date(start_day);
        main_win_->SubKlineWall()->right_clicked_k_hhmm(start_time);

        main_win_->SubKlineWall()->slotOpenRelatedSubKwall(false);
    }
    
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

void TrainDlg::OnMoveToNextK()
{
    auto date_time = parent_->MoveRightEndToNextK();
    if( main_win_->SubKlineWall() )
        main_win_->SubKlineWall()->MoveRightEndToNextK(std::get<0>(date_time), std::get<1>(date_time));

    const T_StockHisDataItem & stock_item = CurHisStockDataItem();
    if( stock_item.date == 0 )
    {
        SetStatusBar(QString::fromLocal8Bit("日期为0, 数据异常!"));
        return;
    }
    //assert(trade_dlg_.date_ == stock_item.date);
    
    double profit = 0.0;
    double capital_ret_stop_profit_short = 0.0;
    double capital_ret_stop_loss_long = 0.0;
    double capital_ret_stop_profit_long = 0.0;
    double capital_ret_stop_loss_short = 0.0;
    
    bool has_trade = false;
    if( fabs(stock_item.close_price - stock_item.high_price) > fabs(stock_item.close_price - stock_item.low_price) ) 
    {
        // close price is nearby low price: first high price then low price
        double profit_long_pos = 0.0;
        std::vector<int> stop_profit_long_ids;
        std::vector<TradeRecordAtom> trades_stop_profit_long = account_info_.position.DoIfStopProfitLongPos(stock_item.date, stock_item.hhmmss, stock_item.high_price, capital_ret_stop_profit_long, stop_profit_long_ids, nullptr, &profit_long_pos);
        has_trade = (has_trade || !trades_stop_profit_long.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_profit_long.begin(), trades_stop_profit_long.end());
        double loss_short_pos = 0.0;
        std::vector<int> stop_loss_short_ids;
        std::vector<TradeRecordAtom> trades_stop_loss_short = account_info_.position.DoIfStopLossShortPos(stock_item.date, stock_item.hhmmss, stock_item.high_price, capital_ret_stop_loss_short, stop_loss_short_ids, nullptr, &loss_short_pos);
        has_trade = (has_trade || !trades_stop_loss_short.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_loss_short.begin(), trades_stop_loss_short.end());

        double profit_short_pos = 0.0;
        std::vector<int> stop_profit_short_ids;
        std::vector<TradeRecordAtom> trades_stop_profit_short = account_info_.position.DoIfStopProfitShortPos(stock_item.date, stock_item.hhmmss, stock_item.low_price, capital_ret_stop_profit_short, stop_profit_short_ids, nullptr, &profit_short_pos);
        has_trade = (has_trade || !trades_stop_profit_short.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_profit_short.begin(), trades_stop_profit_short.end());
        double loss_long_pos = 0.0;
        std::vector<int> stop_loss_long_ids;
        std::vector<TradeRecordAtom> trades_stop_loss_long = account_info_.position.DoIfStopLossLongPos(stock_item.date, stock_item.hhmmss, stock_item.low_price, capital_ret_stop_loss_long, stop_loss_long_ids, nullptr, &loss_long_pos);
        has_trade = (has_trade || !trades_stop_loss_long.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_loss_long.begin(), trades_stop_loss_long.end());

        profit = profit_long_pos + loss_short_pos + profit_short_pos + loss_long_pos;
    }else
    { // close price is nearby high price: first low price then high price
        double profit_short_pos = 0.0;
        std::vector<int> stop_profit_short_ids;
        std::vector<TradeRecordAtom> trades_stop_profit_short = account_info_.position.DoIfStopProfitShortPos(stock_item.date, stock_item.hhmmss, stock_item.low_price, capital_ret_stop_profit_short, stop_profit_short_ids, nullptr, &profit_short_pos);
        has_trade = (has_trade || !trades_stop_profit_short.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_profit_short.begin(), trades_stop_profit_short.end());
        double loss_long_pos = 0.0;
        std::vector<int> stop_loss_long_ids;
        std::vector<TradeRecordAtom> trades_stop_loss_long = account_info_.position.DoIfStopLossLongPos(stock_item.date, stock_item.hhmmss, stock_item.low_price, capital_ret_stop_loss_long, stop_loss_long_ids, nullptr, &loss_long_pos);
        has_trade = (has_trade || !trades_stop_loss_long.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_loss_long.begin(), trades_stop_loss_long.end());

        double profit_long_pos = 0.0;
        std::vector<int> stop_profit_long_ids;
        std::vector<TradeRecordAtom> trades_stop_profit_long = account_info_.position.DoIfStopProfitLongPos(stock_item.date, stock_item.hhmmss, stock_item.high_price, capital_ret_stop_profit_long, stop_profit_long_ids, nullptr, &profit_long_pos);
        has_trade = (has_trade || !trades_stop_profit_long.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_profit_long.begin(), trades_stop_profit_long.end());
        double loss_short_pos = 0.0;
        std::vector<int> stop_loss_short_ids;
        std::vector<TradeRecordAtom> trades_stop_loss_short = account_info_.position.DoIfStopLossShortPos(stock_item.date, stock_item.hhmmss, stock_item.high_price, capital_ret_stop_loss_short, stop_loss_short_ids, nullptr, &loss_short_pos);
        has_trade = (has_trade || !trades_stop_loss_short.empty());
        trade_records_.insert(trade_records_.end(), trades_stop_loss_short.begin(), trades_stop_loss_short.end());

        profit = profit_long_pos + loss_short_pos + profit_short_pos + loss_long_pos;
    }
     
    if( has_trade )
    {
        account_info_.capital.float_profit = account_info_.position.FloatProfit(stock_item.open_price);
        account_info_.capital.avaliable += profit + capital_ret_stop_profit_short + capital_ret_stop_loss_long + capital_ret_stop_profit_long + capital_ret_stop_loss_short;
        auto low_high = account_info_.position.GetForceClosePrices(account_info_.capital.avaliable + account_info_.capital.float_profit);
        force_close_low_ = std::get<0>(low_high);
        force_close_high_ = std::get<1>(low_high);
    }

    if( (force_close_low_ < MAX_PRICE && stock_item.low_price < force_close_low_ + EPSINON)
        || (force_close_high_ > MIN_PRICE && stock_item.high_price + EPSINON > force_close_high_) )
    {  // force close
        double profit_close_long = 0.0;
        auto long_pos = account_info_.position.LongPosQty();
        if( long_pos > 0 )
        {
        double capital_ret = 0.0;
        std::vector<TradeRecordAtom> trades_close_long = account_info_.position.CloseAvaliableLong(force_close_low_, unsigned int(long_pos), capital_ret, &profit_close_long);
        trade_records_.insert(trade_records_.end(), trades_close_long.begin(), trades_close_long.end());
        }

        double profit_close_short = 0.0;
        auto short_pos = account_info_.position.ShortPosQty();
        if( short_pos > 0 )
        {
        double capital_ret = 0.0;
        std::vector<TradeRecordAtom> trades_close_short = account_info_.position.CloseAvaliableShort(force_close_low_, unsigned int(short_pos), capital_ret, &profit_close_short);
        trade_records_.insert(trade_records_.end(), trades_close_short.begin(), trades_close_short.end());
        }

        double this_price = 0.0;
        if( force_close_low_ < MAX_PRICE && stock_item.low_price < force_close_low_ + EPSINON )
            this_price = force_close_low_;
        else
            this_price = force_close_high_;
        if( long_pos + short_pos > 0 )
            account_info_.capital.avaliable = (long_pos + short_pos) * cst_margin_capital
                                          - CalculateFee(long_pos + short_pos, this_price, true);
        assert(account_info_.capital.avaliable + account_info_.capital.float_profit > EPSINON);
    }else
    {
        account_info_.capital.float_profit = account_info_.position.FloatProfit(stock_item.close_price);
        assert(account_info_.capital.avaliable + account_info_.capital.float_profit > EPSINON);
    }
     
    auto long_pos = account_info_.position.LongPosQty();
    auto short_pos = account_info_.position.ShortPosQty();
     
    ui.le_cur_capital->setText(ToQString(account_info_.capital.avaliable + account_info_.capital.float_profit));
    ui.lab_assets->setText(ToQString(account_info_.capital.avaliable + account_info_.capital.float_profit
                                    + cst_margin_capital * (long_pos + short_pos)));

    PrintTradeRecords();
}

void TrainDlg::OnNextStep()
{  
    //double close_price = MAGIC_STOP_PRICE;
    T_StockHisDataItem item;
    if( main_win_->SubKlineWall() )
        item = main_win_->SubKlineWall()->Train_NextStep();
        parent_->Train_NextStep(item);

    const T_StockHisDataItem & stock_item = CurHisStockDataItem();
    if( stock_item.date == 0 )
    {
        SetStatusBar(QString::fromLocal8Bit("日期为0, 数据异常!"));
        return;
    }
    ui.lab_quote->setText(QString::number(item.close_price));
    if( ui.checkb_follow_market->isChecked() )
        ui.dbspb_price->setValue(item.close_price);
     
    //-------------------handle order infos--------------
    /* there are total 3 order types : 
    1. hang on orders 
    2. auto stop profit/loss orders : ps: only allowed to close related position
    3. condition orders: ps: only allowed to open new position
    */ 
    for(auto iter = hangon_order_infos_.begin(); iter != hangon_order_infos_.end(); )
    {
       if( iter->action == OrderAction::CLOSE )
       {
           bool is_fit_price = iter->position_type == PositionType::POS_LONG ? (!(item.high_price < iter->price)) : (!(item.low_price > iter->price));
           if( is_fit_price )
           { 
                assert(iter->rel_position_id > -1);
                double capital_ret = 0.0;
                TradeRecordAtom trade_info = account_info_.position.ClosePositionAtom(iter->rel_position_id, iter->price, &capital_ret);
                assert(trade_info.pos_type == iter->position_type);
                account_info_.capital.frozen -= cst_margin_capital * trade_info.quantity;
                account_info_.capital.avaliable += capital_ret;

                RemoveInPositionTableView(iter->rel_position_id, iter->position_type);

                hangon_order_infos_.erase(iter++);
                continue;
           }
       }else //open 
       {
           bool is_fit_price = iter->position_type == PositionType::POS_LONG ? (!(item.high_price < iter->price)) : (!(item.low_price > iter->price));
           if( is_fit_price )
           { 
               OpenPosition(iter->price, iter->qty, iter->position_type == PositionType::POS_LONG);
               hangon_order_infos_.erase(iter++);
               continue;
           }
       }
       ++iter;
    }

    for(auto iter = stop_order_infos_.begin(); iter != stop_order_infos_.end(); )
    {
       assert( iter->action == OrderAction::CLOSE );
        
    }
    //-------------------
    double total_profit = RecaculatePosTableViewFloatProfit(item.close_price);
    account_info_.capital.float_profit = ProcDecimal(total_profit, 0);

    RefreshCapitalUi();
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
    for( int i = 0; i < ids.size(); ++i )
    {
        auto pos_item = account_info_.position.FindPositionAtom(ids[i]);
        if( pos_item )
        {
            total_amount += pos_item->qty_all() * pos_item->price;
            total_qty += pos_item->qty_all();
        }
    }
    assert(total_qty > 0);
    double after_average_price = ProcDecimal(total_amount/total_qty, DEFAULT_DECIMAL + 1);

    model->item(row_index, cst_tbview_position_average_price)->setText(QString::number(after_average_price));
    model->item(row_index, cst_tbview_position_size)->setText(QString::number(total_qty));
    model->item(row_index, cst_tbview_position_avaliable)->setText(QString::number(total_qty));
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

void TrainDlg::UpdateOrders2KlineWalls()
{
    parent_->hangon_order_infos_ = hangon_order_infos_;
    parent_->stop_order_infos_ = stop_order_infos_;
    parent_->condition_order_infos_ = condition_order_infos_;
    parent_->update();
    if( main_win_->SubKlineWall() )
    {
        main_win_->SubKlineWall()->hangon_order_infos_ = hangon_order_infos_;
        main_win_->SubKlineWall()->stop_order_infos_ = stop_order_infos_;
        main_win_->SubKlineWall()->condition_order_infos_ = condition_order_infos_;
        main_win_->SubKlineWall()->update();
    }
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

    item = new QStandardItem(QString::fromLocal8Bit(order_info.position_type == PositionType::POS_LONG ? "买" : "卖"));
    model->setItem(row_index, cst_tbview_hangonorder_bs, item);

    item = new QStandardItem(QString::number(order_info.price));
    model->setItem(row_index, cst_tbview_hangonorder_price, item);

    item = new QStandardItem(QString::number(order_info.qty));
    model->setItem(row_index, cst_tbview_hangonorder_qty, item);
}

void TrainDlg::RefreshCapitalUi()
{
    ui.label_capital->setText(QString::number(account_info_.capital.avaliable + account_info_.capital.frozen + account_info_.capital.float_profit));
    ui.label_capital_available->setText(QString::number(account_info_.capital.avaliable));
    ui.label_float_profit->setText(QString::number(account_info_.capital.float_profit));
}
//
//void TrainDlg::OnOpenOpenWin()
//{
//    trade_dlg_.SetStatusBar("");
//    const T_StockHisDataItem & stock_item = CurHisStockDataItem();
//    if( stock_item.date == 0 )
//    {
//        trade_dlg_.SetStatusBar(QString::fromLocal8Bit("当前日期无对应期货数据!"));
//        return;
//    }
//    this->hide();
//    trade_dlg_.is_close(false);
//    trade_dlg_.SetDate(stock_item.date);
//    trade_dlg_.SetHhmm(stock_item.hhmmss);
//    trade_dlg_.ui.radioBtn_open->setChecked(true);
//    trade_dlg_.ui.radioBtn_close->setChecked(false);
//    //const double allow_highest_price = (stock_item.open_price < stock_item.close_price ? stock_item.close_price : stock_item.open_price);
//
//    trade_dlg_.ui.le_price->setText(ToQString(stock_item.close_price));
//    trade_dlg_.ui.le_stop_profit->setText(ToQString(MAGIC_STOP_PRICE));
//    trade_dlg_.ui.le_stop_loss->setText(ToQString(MAGIC_STOP_PRICE));
//
//    int qty = CalculateMaxQtyAllowOpen(account_info_.capital.avaliable + account_info_.capital.float_profit, stock_item.close_price);
//    trade_dlg_.ui.le_qty_ava->setText(ToQString(qty));
//    trade_dlg_.ui.le_qty->setText(ToQString(qty));
//    trade_dlg_.ui.le_capital_ava->setText(ToQString(account_info_.capital.avaliable + account_info_.capital.float_profit));
//
//    trade_dlg_.showNormal();
//}
//
//void TrainDlg::OnOpenCloseWin()
//{
//    trade_dlg_.SetStatusBar("");
//    const T_StockHisDataItem & stock_item = CurHisStockDataItem();
//    if( stock_item.date == 0 )
//    {
//        trade_dlg_.SetStatusBar(QString::fromLocal8Bit("当前日期无对应期货数据!"));
//        return;
//    }
//    this->hide();
//    trade_dlg_.is_close(true);
//    trade_dlg_.SetDate(stock_item.date);
//    trade_dlg_.SetHhmm(stock_item.hhmmss);
//    trade_dlg_.ui.radioBtn_close->setChecked(true);
//    trade_dlg_.ui.radioBtn_open->setChecked(false);
//
//    //const double allow_highest_price = (stock_item.open_price < stock_item.close_price ? stock_item.close_price : stock_item.open_price);
//
//    trade_dlg_.ui.le_price->setText(ToQString(stock_item.close_price));
//
//    if( trade_dlg_.ui.radioBtn_long->isChecked() )
//    {
//        trade_dlg_.ui.le_qty->setText(ToQString((int)account_info_.position.LongPos()));
//        trade_dlg_.ui.le_qty_ava->setText(ToQString((int)account_info_.position.LongPos()));
//    }else
//    {
//        trade_dlg_.ui.le_qty->setText(ToQString((int)account_info_.position.ShortPosQty()));
//        trade_dlg_.ui.le_qty_ava->setText(ToQString((int)account_info_.position.ShortPosQty()));
//    }
//    trade_dlg_.ui.le_capital_ava->setText(ToQString(account_info_.capital.avaliable));
//
//    trade_dlg_.showNormal();
//}

void TrainDlg::OnTrade()
{
    trade_dlg_.SetStatusBar("");
    if( trade_dlg_.date_ == 0 )
    {
        trade_dlg_.SetStatusBar(QString::fromLocal8Bit("当前日期无对应股票数据!"));
        return;
    }
    const T_StockHisDataItem & stock_item = CurHisStockDataItem();
    //assert(trade_dlg_.date_ == stock_item.date);

    double price = trade_dlg_.ui.le_price->text().toDouble();
    int quantity = (int)trade_dlg_.ui.le_qty->text().toDouble();
    trade_dlg_.ui.le_qty->setText(ToQString(quantity));

    double allow_lowest_price = (stock_item.open_price < stock_item.close_price ? stock_item.open_price : stock_item.close_price);
    allow_lowest_price = ProcDecimal(allow_lowest_price, DEFAULT_DECIMAL);
    double allow_highest_price = (stock_item.open_price < stock_item.close_price ? stock_item.close_price : stock_item.open_price);
    allow_highest_price = ProcDecimal(allow_highest_price, DEFAULT_DECIMAL);
    if( price < allow_lowest_price || price > allow_highest_price ) 
    {
        trade_dlg_.SetStatusBar(QString::fromLocal8Bit("报价非法!"));
        trade_dlg_.ui.le_price->setFocus();
        return;
    }
    const bool is_pos_type_long = trade_dlg_.is_long();

    if( trade_dlg_.is_close_ )
    {
        if( quantity <= 0 )
        {
            trade_dlg_.SetStatusBar(QString::fromLocal8Bit("平仓数量非法!"));
            trade_dlg_.ui.le_qty->setFocus();
            return;
        } 
        int avaliable_pos = 0;
        if( is_pos_type_long )
            avaliable_pos = (int)account_info_.position.LongPosQty();
        else
            avaliable_pos = (int)account_info_.position.ShortPosQty();

        if( quantity > avaliable_pos )
        {
            trade_dlg_.SetStatusBar(QString::fromLocal8Bit("可平数量不足!"));
            trade_dlg_.ui.le_qty->setFocus();
            return;
        }
        
        double final_fill_price = 0.0;
        double profit = 0.0;
        double capital_ret = 0.0;
        if( is_pos_type_long )
        {
            final_fill_price = price - cst_per_tick;
            account_info_.position.CloseAvaliableLong(final_fill_price, unsigned int(quantity), capital_ret, &profit);
        }else
        {
            final_fill_price = price + cst_per_tick;
            account_info_.position.CloseAvaliableShort(final_fill_price, unsigned int(quantity), capital_ret, &profit);
        }
        double fee = CalculateFee(quantity, final_fill_price, true);
         
        account_info_.capital.avaliable += profit - fee + capital_ret; 

        TradeRecordAtom  trade_item;
        trade_item.trade_id = account_info_.position.GenerateTradeId();
        trade_item.date = trade_dlg_.date_;
        trade_item.hhmm = trade_dlg_.hhmm_;
        trade_item.action = OrderAction::CLOSE;
        trade_item.pos_type = is_pos_type_long ? PositionType::POS_LONG : PositionType::POS_SHORT;
        trade_item.price = final_fill_price;
        trade_item.quantity = quantity;
        trade_item.fee = fee;
        trade_item.profit = profit; 
        trade_records_.push_back(trade_item);
    }else // open 
    { 
        if( quantity < 1 )
        {
            trade_dlg_.SetStatusBar(QString::fromLocal8Bit("购买数量非法!"));
            trade_dlg_.ui.le_qty->setFocus();
            return;
        }
        double capital_buy = cst_margin_capital * quantity;
        double final_fill_price = 0.0;
        if( is_pos_type_long )
        {
            final_fill_price = price + cst_per_tick;
        }else
        {
            final_fill_price = price - cst_per_tick;
        }
        double fee = CalculateFee(quantity, final_fill_price, false);
        if( capital_buy + fee > account_info_.capital.avaliable )
        {
            trade_dlg_.SetStatusBar(QString::fromLocal8Bit("可用资金不足!"));
            return;
        }
        double stop_profit_price = MAGIC_STOP_PRICE;
        if( !trade_dlg_.ui.le_stop_profit->text().trimmed().isEmpty() )
            stop_profit_price = trade_dlg_.ui.le_stop_profit->text().trimmed().toDouble();
        double stop_loss_price = MAGIC_STOP_PRICE;
        if( !trade_dlg_.ui.le_stop_loss->text().trimmed().isEmpty() )
            stop_loss_price = trade_dlg_.ui.le_stop_loss->text().trimmed().toDouble();

        account_info_.capital.avaliable -= capital_buy + fee;

        auto pos_atom = std::make_shared<PositionAtom>();
        pos_atom->trade_id = account_info_.position.GenerateTradeId();
        pos_atom->price = final_fill_price;
        pos_atom->qty_available = (unsigned int)quantity;
        pos_atom->stop_profit_price = stop_profit_price;
        pos_atom->stop_loss_price = stop_loss_price;
        account_info_.position.PushBack(is_pos_type_long, pos_atom);
 
        TradeRecordAtom  trade_item;
        trade_item.trade_id = pos_atom->trade_id;
        trade_item.date = trade_dlg_.date_;
        trade_item.hhmm = trade_dlg_.hhmm_;
        trade_item.action = OrderAction::OPEN;
        trade_item.pos_type = is_pos_type_long ? PositionType::POS_LONG : PositionType::POS_SHORT;
        trade_item.price = final_fill_price;
        trade_item.quantity = quantity;
        trade_item.profit = 0.0;
        trade_item.fee = fee;
        trade_item.price_stop_loss = stop_loss_price;
        trade_item.price_stop_profit = stop_profit_price;
        trade_records_.push_back(trade_item);

        trade_dlg_.ui.le_capital_ava->setText(ToQString(account_info_.capital.avaliable));
    }
    
    auto force_close_low_high = account_info_.position.GetForceClosePrices(account_info_.capital.avaliable + account_info_.capital.frozen);
    force_close_low_ = std::get<0>(force_close_low_high);
    force_close_high_ = std::get<1>(force_close_low_high);

    // ui --------
    ui.le_cur_capital->setText(ToQString(account_info_.capital.avaliable));
    

    double assets_today_end = account_info_.capital.avaliable + account_info_.capital.frozen
        + account_info_.position.FloatProfit(price) + account_info_.position.TotalPosition() * cst_margin_capital;
    ui.lab_assets->setText(ToQString(assets_today_end));

    trade_dlg_.hide();
    PrintTradeRecords();
    this->showNormal();
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
            OpenPosition(ui.spb_order_num->value(), market_price, true);
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

void TrainDlg::OpenPosition(double para_price, unsigned int qty, bool is_long)
{ 
    //auto qty = (unsigned int)ui.spb_order_num->value();
    assert(qty > 0);
    double price = para_price;//is_long ? (ui.dbspb_price->value() + cst_per_tick) : (ui.dbspb_price->value() - cst_per_tick);
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
    //pos_atom->stop_profit_price = stop_profit_price;
    //pos_atom->stop_loss_price = stop_loss_price;

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

        //std::unordered_map<int, unsigned int> pos_ids_sizes = account_info_.position.LongPosSizeInfo(POSITION_STATUS_ALL);

        //auto amount_qty = get_total_amount_qty(account_info_.position, ids);
        //double total_amount = std::get<0>(amount_qty);
        unsigned int total_qty = account_info_.position.LongPosQty(POSITION_STATUS_ALL);
        assert(total_qty > 0); 
        unsigned int ava_qty = is_long ? account_info_.position.LongPosQty(POSITION_STATUS_AVAILABLE) : account_info_.position.ShortPosQty(POSITION_STATUS_AVAILABLE);
        double avarege_price = pos_atom->is_long ? account_info_.position.LongAveragePrice() : account_info_.position.ShortAveragePirce();
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
    // consider auto stop (profit/loss) order -----
    auto append_stop_order = [this](const PositionAtom &pos_atom, double price, bool is_long, bool is_stop_loss)
    {
        OrderInfo  order;
        order.rel_position_id = pos_atom.trade_id;
        order.action = OrderAction::CLOSE;
        order.position_type = is_long ? PositionType::POS_LONG : PositionType::POS_SHORT;
        order.qty = pos_atom.qty_available;
        if( is_stop_loss )
            order.price = is_long ? (price - cst_per_tick * (double)auto_stop_loss_ticks_) : (price + cst_per_tick * (double)auto_stop_loss_ticks_);
        else // stop profit
            order.price = is_long ? (price + cst_per_tick * (double)auto_stop_profit_ticks_) : (price - cst_per_tick * (double)auto_stop_profit_ticks_);
        stop_order_infos_.push_back(order);
    };
    
    if( auto_stop_profit_ )
        append_stop_order(*pos_atom, price, is_long, false);
    if( auto_stop_loss_ )
        append_stop_order(*pos_atom, price, is_long, true);
     
}


void TrainDlg::CloseInputSizePosition(double para_price, bool is_long)
{
    int close_qty = ui.spb_order_num->value();
    QTableView &tbv = *ui.table_view_position;
    QStandardItemModel * model = static_cast<QStandardItemModel *>(tbv.model());
    auto row_index = find_model_first_fit_index(*model, is_long);
    if( row_index < 0 ) 
    { 
        return SetStatusBar(is_long ? QString::fromLocal8Bit("无多头仓位可平!") : QString::fromLocal8Bit("无空头仓位可平!"));
    }
    
    //auto total_qty = GetItemPositionAllQty(*model, row_index);
    auto total_qty = is_long ? account_info_.position.LongPosQty(POSITION_STATUS_AVAILABLE) : account_info_.position.ShortPosQty(POSITION_STATUS_AVAILABLE);
    if( close_qty > total_qty )
         return SetStatusBar(QString::fromLocal8Bit("平仓数量超过现有仓位!"));
    QVector<int> ids = model->item(row_index, cst_tbview_position_id)->data().value<QVector<int>>();
    
    int date = 0;
    int hhmm = 0;//ndedt
    double capital_ret = 0.0;
    double profit_close = 0.0;
    std::vector<int> position_ids;
    std::vector<TradeRecordAtom> trade_record_atoms;
    if( is_long )
        trade_record_atoms = account_info_.position.CloseAvaliableLong(para_price, close_qty, capital_ret, &profit_close, &position_ids);
    else
        trade_record_atoms = account_info_.position.CloseAvaliableShort(para_price, close_qty, capital_ret, &profit_close, &position_ids);
    account_info_.capital.frozen -= capital_ret;
    account_info_.capital.avaliable += capital_ret + profit_close;

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
#if 1 
        RecaculatePosTableViewItem(ids_after, row_index);
#else
        QVariant var_data;
        var_data.setValue(ids_after);
        model->item(row_index, cst_tbview_position_id)->setData(var_data);
        //recaculate average open price-------
        double total_amount = 0.0;
        unsigned int total_qty = 0;
        for( int i = 0; i < ids_after.size(); ++i )
        {
            auto pos_item = account_info_.position.FindPositionAtom(ids_after[i]);
            if( pos_item )
            {
                total_amount += pos_item->qty * pos_item->price;
                total_qty += pos_item->qty;
            }
        }
        assert(total_qty > 0);
        double after_average_price = ProcDecimal(total_amount/total_qty, DEFAULT_DECIMAL + 1);

        model->item(row_index, cst_tbview_position_average_price)->setText(QString::number(after_average_price));
        model->item(row_index, cst_tbview_position_size)->setText(QString::number(total_qty));
        model->item(row_index, cst_tbview_position_avaliable)->setText(QString::number(total_qty));
#endif
    }
    double cur_quote = ui.lab_quote->text().toDouble();
    double total_profit = RecaculatePosTableViewFloatProfit(cur_quote);
    account_info_.capital.float_profit = ProcDecimal(total_profit, 0);
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

    OrderInfo  order;
    order.fake_id = TblHangonOrdersRowCount();  
    order.action = OrderAction::OPEN;
    order.position_type = is_long ? PositionType::POS_LONG : PositionType::POS_SHORT;
    order.qty = quantity;
    order.price = price;
    hangon_order_infos_.push_back(order);
    Append2TblHangonOrders(order);
    UpdateOrders2KlineWalls();
    return true;
}

bool TrainDlg::AddCloseOrder(double price, unsigned int quantity, bool is_long)
{
    unsigned int rel_pos_size = is_long ? account_info_.position.LongPosQty(POSITION_STATUS_AVAILABLE) : account_info_.position.ShortPosQty(POSITION_STATUS_AVAILABLE);
    if( quantity > rel_pos_size )
    {
        SetStatusBar(QString::fromLocal8Bit("仓位不足!"));
        return false;
    }

    //---------------------------------------
    OrderInfo  order;
    order.fake_id = TblHangonOrdersRowCount();
    order.action = OrderAction::CLOSE;
    order.position_type = is_long ? PositionType::POS_LONG : PositionType::POS_SHORT;
    order.qty = quantity;
    order.price = price;
    // froze avaliable position
    std::unordered_map<int, unsigned int> rel_ids_sizes = is_long ? account_info_.position.LongPosSizeInfo(POSITION_STATUS_AVAILABLE) : account_info_.position.ShortPosSizeInfo(POSITION_STATUS_AVAILABLE);
    int remain_qty = quantity;

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

    parent_->hangon_order_infos_ = hangon_order_infos_;
    parent_->update();
    if( main_win_->SubKlineWall() )
    {
        main_win_->SubKlineWall()->hangon_order_infos_ = hangon_order_infos_;
        main_win_->SubKlineWall()->update();
    }
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
