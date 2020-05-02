#include "kline_wall.h"
#include <cassert>
#include <qdebug.h>

#include <TLib/core/tsystem_utility_functions.h>

#include "mainwindow.h"
#include "stkfo_common.h"
#include "futures_forecast_app.h"

const static std::string cst_logf_pretag = "autoforcast";
// (c1, c2, c3) or (d1, d2, d3)
typedef std::tuple<double, double, double> T_TargetPrices;

using namespace TSystem;

static void append_forcast_c(const std::string &code, TypePeriod k_type, const T_StockHisDataItem &item_a, const T_StockHisDataItem &item_b, bool is_ab_down, ForcastMan &forcast_man)
{
    T_Data2pForcast data_2pdown_fcst(is_ab_down);
    data_2pdown_fcst.stock_code = code;
    data_2pdown_fcst.date_a = item_a.date; 
    data_2pdown_fcst.hhmm_a = item_a.hhmmss;
    data_2pdown_fcst.date_b = item_b.date; 
    data_2pdown_fcst.hhmm_b = item_b.hhmmss;

    auto c1_c2_c3 = std::make_tuple(0.0, 0.0, 0.0);
    if( is_ab_down )
        c1_c2_c3 = ForcastC_ABDown(item_a.high_price, item_b.low_price);
    else
        c1_c2_c3 = ForcastC_ABUp(item_a.low_price, item_b.high_price);
    data_2pdown_fcst.c1 = std::get<0>(c1_c2_c3);
    data_2pdown_fcst.c2 = std::get<1>(c1_c2_c3);
    data_2pdown_fcst.c3 = std::get<2>(c1_c2_c3);
    forcast_man.Append(k_type, code, is_ab_down, data_2pdown_fcst);
}

static T_TargetPrices forcast_2p(const T_StockHisDataItem &item_a, const T_StockHisDataItem &item_b, bool is_ab_down)
{
    auto c1_c2_c3 = std::make_tuple(0.0, 0.0, 0.0);
    if( is_ab_down )
        c1_c2_c3 = ForcastC_ABDown(item_a.high_price, item_b.low_price);
    else
        c1_c2_c3 = ForcastC_ABUp(item_a.low_price, item_b.high_price);
    return c1_c2_c3;
}

void KLineWall::HandleAutoForcast()
{ 
    static auto find_price_min_max_index = [](const T_HisDataItemContainer &k_datas, int beg_index, int end_index)
    {
        int min_price_index = beg_index;
        int max_price_index = beg_index;
        double min_price = MAX_PRICE;
        double max_price = MIN_PRICE;
        int i = beg_index;
        for( ; i <= end_index; ++i )
        {
            if( k_datas[i]->stk_item.low_price < min_price )
            {
                min_price = k_datas[i]->stk_item.low_price;
                min_price_index = i;
            }
            if( k_datas[i]->stk_item.high_price > max_price )
            {
                max_price = k_datas[i]->stk_item.high_price;
                max_price_index = i;
            }
        }
        return std::make_tuple(min_price_index, max_price_index);
    };

    if( !main_win_->is_show_autoforcast() )
        return;
    auto_forcast_man_.RemoveForcastItems(stock_code_, k_type_);

    double max_spread = 100.0;
    double min_spread = 1.0;
    switch(k_type_)
    {
    case TypePeriod::PERIOD_YEAR: max_spread = 500.0; min_spread = 50.0; break;
    case TypePeriod::PERIOD_MON:
    case TypePeriod::PERIOD_WEEK: max_spread = 300.0; min_spread = 50.0;  break;
    case TypePeriod::PERIOD_DAY: max_spread = 200; min_spread = 5.0; break;
    case TypePeriod::PERIOD_HOUR:max_spread = 100; min_spread = 3.0; break;
    case TypePeriod::PERIOD_30M: max_spread = 100; min_spread = 2.0; break;
    case TypePeriod::PERIOD_15M: max_spread = 100; min_spread = 1.0; break;
    case TypePeriod::PERIOD_5M: max_spread = 50.0; min_spread = 1.0; break;
    case TypePeriod::PERIOD_1M: max_spread = 30.0; min_spread = 1.0; break;
    default: break;
    }

    T_StructLineContainer &line_datas = app_->stock_data_man().GetStructLineContainer(ToPeriodType(k_type_), stock_code_); 
    if( line_datas.empty() )
        return;

    T_HisDataItemContainer &k_datas = app_->stock_data_man().GetHisDataContainer(k_type_, stock_code_);
    assert(k_datas.size() > k_rend_index_for_train_);
    const int k_end_index_for_train = k_datas.size() - 1 - k_rend_index_for_train_;
    T_StockHisDataItem &item_cur = k_datas[k_end_index_for_train]->stk_item;
    //const double cur_price = item_cur.close_price;

    T_StockHisDataItem &item_a = k_datas[line_datas[0]->beg_index]->stk_item;
    T_StockHisDataItem &item_b = k_datas[line_datas[0]->end_index]->stk_item;
    bool is_ab_down = line_datas[0]->type == LineType::DOWN;
    if( line_datas[0]->type == LineType::DOWN )
    {  
        if( line_datas.size() == 1 )
        {
            auto min_max_price_indexs = find_price_min_max_index(k_datas, line_datas[0]->end_index + 1, k_end_index_for_train);
            double max_price = k_datas[std::get<1>(min_max_price_indexs)]->stk_item.high_price;
            if( max_price > item_a.high_price )
            {
                T_StockHisDataItem &item_max_price = k_datas[std::get<1>(min_max_price_indexs)]->stk_item;
                append_forcast_c(stock_code_, k_type_, item_a, item_max_price, false, auto_forcast_man_);
            }else
                append_forcast_c(stock_code_, k_type_, item_a, item_b, is_ab_down, auto_forcast_man_);

        }else if( line_datas.size() == 2 )
        {
            app_->local_logger().LogLocal(cst_logf_pretag, utility::FormatStr("LineType::DOWN lines 2"));
        }else
        {
            // find target line-------------
            unsigned int target_line_index = 0;
            double highest_price = item_a.high_price;
            for( unsigned int i = 2; i < line_datas.size(); i += 2 )
            {
                //assert(line_datas[i]->type == LineType::DOWN);
                if( line_datas[i]->type != LineType::DOWN )
                    break;
                T_StockHisDataItem &item_front_beg = k_datas[line_datas[i]->beg_index]->stk_item; 
                T_StockHisDataItem &item_front_end = k_datas[line_datas[i]->end_index]->stk_item; 
                if( item_front_end.low_price < item_b.low_price )
                    break;
                if( item_front_beg.high_price < highest_price )
                    break;
                if( item_front_beg.high_price - item_b.high_price > max_spread )
                    break;
                if( item_front_beg.high_price > highest_price )
                    highest_price = item_front_beg.high_price;
                target_line_index = i;
            }
            append_forcast_c(stock_code_, k_type_, k_datas[line_datas[target_line_index]->beg_index]->stk_item, item_b, is_ab_down, auto_forcast_man_);
        }

    }else // LineType::UP
    {  
        if( line_datas.size() == 1 )
        {
            auto min_max_price_indexs = find_price_min_max_index(k_datas, line_datas[0]->end_index + 1, k_end_index_for_train);
            double min_price = k_datas[std::get<0>(min_max_price_indexs)]->stk_item.low_price;
            if( min_price < item_a.low_price )
            { 
                T_StockHisDataItem &item_min_price = k_datas[std::get<0>(min_max_price_indexs)]->stk_item;
                append_forcast_c(stock_code_, k_type_, item_b, item_min_price, true, auto_forcast_man_);
            }else
                append_forcast_c(stock_code_, k_type_, item_a, item_b, is_ab_down, auto_forcast_man_);

        }else if( line_datas.size() == 2 )
        {
            app_->local_logger().LogLocal(cst_logf_pretag, utility::FormatStr("LineType::UP lines 2"));

        }else if( line_datas.size() == 3 )
        {
            append_forcast_c(stock_code_, k_type_, item_a, item_b, is_ab_down, auto_forcast_man_);
        }else
        {
            // find target line-------------
            unsigned int target_line_index = 0;
            double lowest_price = item_a.low_price;
            for( unsigned int i = 2; i < line_datas.size(); i += 2 )
            {
                //assert(line_datas[i]->type == LineType::UP);
                if( line_datas[i]->type != LineType::UP )
                    break;
                T_StockHisDataItem &item_front_beg = k_datas[line_datas[i]->beg_index]->stk_item; 
                T_StockHisDataItem &item_front_end = k_datas[line_datas[i]->end_index]->stk_item; 
                if( item_front_end.high_price > item_b.high_price )
                    break;
                if( item_front_beg.low_price > lowest_price )
                    break;
                if( item_b.high_price - item_front_beg.low_price > max_spread )
                    break;
                if( item_front_beg.low_price < lowest_price )
                    lowest_price = item_front_beg.low_price;
                target_line_index = i;
            }//for
            append_forcast_c(stock_code_, k_type_, k_datas[line_datas[target_line_index]->beg_index]->stk_item, item_b, is_ab_down, auto_forcast_man_);
        }
    }// LineType::UP
}