#include "train_trade.h"

#include <cassert>
#include <cstdio>
#include <cmath>
#include <algorithm>

#include <QString>

#include <TLib/core/tsystem_time.h>

#include "stkfo_common.h"


QString TradeRecordAtom::ToQStr()
{
    char buf[1024] = {'\0'};
    sprintf_s(buf, "编号:%d 时间:%d-%04d %s %s 数量:%d 价格:%.2f (止赢价:%.2f 止损价:%.2f) 佣金税费:%.2f 盈亏:%.2f "
        , this->trade_id
        , this->date
        , this->hhmm
        , ToStr(this->action).c_str()
        , ToStr(this->pos_type).c_str()
        , this->quantity
        , this->price
        , this->price_stop_profit
        , this->price_stop_loss
        , this->fee
        , this->profit
        );
    return QString::fromLocal8Bit(buf);
}

//----------------------
double PositionAtom::FloatProfit(double cur_price)
{
    double dis_price = is_long ? cur_price - price : price - cur_price;
    double val = dis_price / cst_per_tick * cst_per_tick_capital * qty_all();
    return ProcDecimal(val, 0);
}

double PositionAtom::PartProfit(double cur_price, unsigned int qty)
{
    double dis_price = is_long ? cur_price - price : price - cur_price;
    double val = dis_price / cst_per_tick * cst_per_tick_capital * qty;
    return ProcDecimal(val, 0);
}

unsigned int PositionAtom::qty_frozen()
{
    unsigned int  frozened_qty = 0;
    for( auto iter = qty_frozens.begin(); iter != qty_frozens.end(); ++iter )
    {
        frozened_qty += iter->second;
    }
    return frozened_qty;
}

//---------------------- 
unsigned int PositionInfo::LongPosQty(int target_status)
{ 
    return PositionQty(PositionType::POS_LONG, target_status);;
}

double PositionInfo::LongAveragePrice()
{
    if( long_positions_.empty() )
        return 0.0;
    double ave_price = 0;
    double amount = 0.0;
    int qty = 0;
    for( unsigned int i = 0; i < long_positions_.size(); ++i ) 
    {
        amount += long_positions_.at(i)->price * long_positions_.at(i)->qty_all();
        qty += long_positions_.at(i)->qty_all();
    }
    ave_price = amount / (double)qty;
    return ProcDecimal(ave_price, DEFAULT_DECIMAL);
}
 
//std::vector<PositionAtom *> PositionInfo::FrozeLongPosWhichUnfrozen(unsigned int qty)
//{
//    std::vector<PositionAtom *>  ret;
//    unsigned int temp_qty = 0;
//    //std::for_each( std::begin(long_positions_), std::end(long_positions_), [qty, &temp_qty, &ret](T_PositionAtoms::reference entry)
//    for( int i = 0; i < long_positions_.size(); ++i )
//    {
//        if( !long_positions_.at(i)->is_frozen )
//        {
//            temp_qty += long_positions_.at(i)->qty;
//        }
//    });
//    return qty;
//}

unsigned int PositionInfo::ShortPosQty(int target_status)
{
    return PositionQty(PositionType::POS_SHORT, target_status);
}

unsigned int PositionInfo::PositionQty(PositionType type, int target_status)
{
    T_PositionAtoms *p_positions = (type == PositionType::POS_LONG ? &long_positions_ : &short_positions_);
    unsigned int qty = 0;
    std::for_each( std::begin(*p_positions), std::end(*p_positions), [&qty, target_status](T_PositionAtoms::reference entry)
    {
        if( target_status == POSITION_STATUS_FROZEN )
        {
            qty += entry->qty_frozen();
        }else if( target_status == POSITION_STATUS_AVAILABLE )
        {
            qty += entry->qty_available;
        }else
            qty += entry->qty_all();
    });
    return qty;
}
 
std::unordered_map<int, unsigned int> PositionInfo::PositionSizeInfo(PositionType type, int target_status)
{
    T_PositionAtoms *p_positions = (type == PositionType::POS_LONG ? &long_positions_ : &short_positions_);
    std::unordered_map<int, unsigned int> ret;
    std::for_each( std::begin(*p_positions), std::end(*p_positions), [&ret, target_status](T_PositionAtoms::reference entry)
    {
        if( target_status == POSITION_STATUS_FROZEN )
        {
            ret.insert(std::make_pair(entry->trade_id, entry->qty_frozen()));
        }else if( target_status == POSITION_STATUS_AVAILABLE )
        {
            ret.insert(std::make_pair(entry->trade_id, entry->qty_available));
        }else
            ret.insert(std::make_pair(entry->trade_id, entry->qty_all()));

    });
    return ret;
}

double PositionInfo::ShortAveragePirce()
{
    if( short_positions_.empty() )
        return 0.0;
    double ave_price = 0;
    double amount = 0.0;
    int qty = 0;
    for( unsigned int i = 0; i < short_positions_.size(); ++i ) 
    {
        amount += short_positions_.at(i)->price * short_positions_.at(i)->qty_all();
        qty += short_positions_.at(i)->qty_all();
    }
    ave_price = amount / (double)qty;
    return ProcDecimal(ave_price, DEFAULT_DECIMAL);
}

double PositionInfo::FloatProfit(double price)
{
    assert(price > 0.0);
    double profit = 0.0;
    std::for_each(std::begin(long_positions_), std::end(long_positions_), [&](T_PositionAtoms::reference entry)
    {
        profit += (price - entry->price) / cst_per_tick * cst_per_tick_capital * entry->qty_all();
    });
    std::for_each(std::begin(short_positions_), std::end(short_positions_), [&](T_PositionAtoms::reference entry)
    {
        profit += (entry->price - price) / cst_per_tick * cst_per_tick_capital * entry->qty_all();
    });
    return ProcDecimal(profit, 0);
}

// return <low, high> ;  capital is remain capital
std::tuple<double, double> PositionInfo::GetForceClosePrices(double capital)
{
    assert(capital > EPSINON);

    double low_price = MAX_PRICE;
    double high_price = MIN_PRICE;

    double long_ava_price = LongAveragePrice();
    if( long_ava_price > 50.0 ) // means exists long position
    {
        double start_price = long_ava_price;
        while( start_price > long_ava_price - 50.0 )
        {
            start_price -= cst_per_tick;
            double profit = FloatProfit(start_price);
            if( capital + profit < EPSINON )
            {
                low_price = start_price;
                break;
            }
        }
    }
    
    double short_ava_price = ShortAveragePirce();
    if( short_ava_price > 50.0 )// means exists short position
    {
        double start_price = short_ava_price;
        while( start_price < short_ava_price + 50.0 )
        {
            start_price += cst_per_tick;
            double profit = FloatProfit(start_price);
            if( capital + profit < EPSINON )
            {
                high_price = start_price;
                break;
            }
        }
    }
    return std::make_tuple(low_price, high_price);
}

// return profit of close action; if p_cur_price not null use it as trade price;
std::vector<TradeRecordAtom> PositionInfo::DoIfStopProfitLongPos(const T_StockHisDataItem &k_item, double &capital_ret, std::vector<int> &ret_ids, double *p_cur_price, double *p_profit)
{
    //assert(k_item.date > 0);
    assert(k_item.high_price > EPSINON);

    std::vector<TradeRecordAtom> ret;
    double profit = 0.0;
    capital_ret = 0.0;
    for( auto iter = long_positions_.begin(); iter != long_positions_.end(); )
    {
        if( (*iter)->qty_available == 0 )
        {
            ++iter;
            continue;
        }
        if( (*iter)->stop_profit_price > EPSINON && k_item.high_price > (*iter)->stop_profit_price - EPSINON ) // >= stop profit price
        {
            TradeRecordAtom  trade_item;
            trade_item.trade_id = GenerateTradeId();
            trade_item.date = k_item.date;
            trade_item.hhmm = k_item.hhmmss;
            trade_item.action = OrderAction::CLOSE;
            trade_item.pos_type = PositionType::POS_LONG;
            trade_item.quantity = (*iter)->qty_available;
            if( p_cur_price )
                trade_item.price = *p_cur_price - cst_per_tick;
            else
                trade_item.price = (k_item.open_price > (*iter)->stop_profit_price ? k_item.open_price : (*iter)->stop_profit_price);

            trade_item.profit = (trade_item.price - (*iter)->price) / cst_per_tick * cst_per_tick_capital;
            trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
            ret.push_back(trade_item);
            capital_ret += cst_margin_capital * trade_item.quantity;
            profit += trade_item.profit - trade_item.fee;
            ret_ids.push_back((*iter)->trade_id);
            (*iter)->ClearAvaliable();
            //iter = long_positions_.erase(iter);
        }else
            ++iter;
    }
    if( p_profit )
        *p_profit = ProcDecimal(profit, 0);
    return ret;
}

// return profit of close action; if p_cur_price not null use it as trade price
std::vector<TradeRecordAtom> PositionInfo::DoIfStopProfitShortPos(const T_StockHisDataItem &k_item, double &capital_ret, std::vector<int> &ret_ids, double *p_cur_price, double *p_profit)
{
    assert(k_item.low_price > EPSINON);

    std::vector<TradeRecordAtom> ret;
    double profit = 0.0;
    capital_ret = 0.0; 
    for( auto iter = short_positions_.begin(); iter != short_positions_.end(); )
    {
        if( (*iter)->qty_available == 0 )
        {
            ++iter;
            continue;
        }
        if( (*iter)->stop_profit_price > EPSINON && k_item.low_price < (*iter)->stop_profit_price + EPSINON ) // <= profit price
        {
            TradeRecordAtom  trade_item;
            trade_item.trade_id = GenerateTradeId();
            trade_item.date = k_item.date;
            trade_item.hhmm = k_item.hhmmss;
            trade_item.action = OrderAction::CLOSE;
            trade_item.pos_type = PositionType::POS_SHORT;
            trade_item.quantity = (*iter)->qty_available;
            if( p_cur_price )
                trade_item.price = *p_cur_price + cst_per_tick;
            else
                trade_item.price = (k_item.open_price < (*iter)->stop_profit_price ? k_item.open_price : (*iter)->stop_profit_price);

            trade_item.profit = ((*iter)->price - trade_item.price) / cst_per_tick * cst_per_tick_capital;
            trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
            ret.push_back(trade_item);
            capital_ret += cst_margin_capital * trade_item.quantity;
            profit += trade_item.profit - trade_item.fee;
            ret_ids.push_back((*iter)->trade_id);
            (*iter)->ClearAvaliable();
            //iter = short_positions_.erase(iter);
        }else
            ++iter;
    }
    if( p_profit )
        *p_profit = ProcDecimal(profit, 0);
    return ret;
}

// return profit of close action ; if p_cur_price not null use it as trade price
std::vector<TradeRecordAtom> PositionInfo::DoIfStopLossLongPos(const T_StockHisDataItem &k_item, double &capital_ret, std::vector<int> &ret_ids, double *p_cur_price, double *p_profit)
{
    assert(k_item.low_price > EPSINON); 

    std::vector<TradeRecordAtom> ret;
    double profit = 0.0;
    capital_ret = 0.0;
    for( auto iter = long_positions_.begin(); iter != long_positions_.end(); )
    {
        if( (*iter)->qty_available == 0 )
        {
            ++iter;
            continue;
        }
        if( k_item.low_price < (*iter)->stop_loss_price + EPSINON ) // <= stop loss price
        {
            TradeRecordAtom  trade_item;
            trade_item.trade_id = GenerateTradeId();
            trade_item.date = k_item.date;
            trade_item.hhmm = k_item.hhmmss;
            trade_item.action = OrderAction::CLOSE;
            trade_item.pos_type = PositionType::POS_LONG;
            trade_item.quantity = (*iter)->qty_available;
            if( p_cur_price )
                trade_item.price = *p_cur_price - cst_per_tick;
            else
                trade_item.price = (k_item.open_price < (*iter)->stop_loss_price ? k_item.open_price : (*iter)->stop_loss_price - cst_per_tick);
            trade_item.profit = (trade_item.price - (*iter)->price) / cst_per_tick * cst_per_tick_capital;
            trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
            ret.push_back(trade_item);
            capital_ret += cst_margin_capital * trade_item.quantity;
            profit += trade_item.profit - trade_item.fee;
            ret_ids.push_back((*iter)->trade_id);
            (*iter)->ClearAvaliable();
            //iter = long_positions_.erase(iter);
        }else
            ++iter;
    }
    if( p_profit )
        *p_profit = ProcDecimal(profit, 0);
    return ret;
}

// return profit(include fee) of close action ; if p_cur_price not null use it as trade price
std::vector<TradeRecordAtom> PositionInfo::DoIfStopLossShortPos(const T_StockHisDataItem &k_item, double &capital_ret, std::vector<int> &ret_ids, double *p_cur_price, double *p_profit)
{
    assert(k_item.high_price > EPSINON);  

    std::vector<TradeRecordAtom> ret;
    double profit = 0.0;
    capital_ret = 0.0;
    for( auto iter = short_positions_.begin(); iter != short_positions_.end(); )
    {
        if( (*iter)->qty_available == 0 )
        {
            ++iter;
            continue;
        }
        if( (*iter)->stop_loss_price > EPSINON && k_item.high_price + EPSINON > (*iter)->stop_loss_price )
        {
            TradeRecordAtom  trade_item;
            trade_item.trade_id = GenerateTradeId();
            trade_item.date = k_item.date;
            trade_item.hhmm = k_item.hhmmss;
            trade_item.action = OrderAction::CLOSE;
            trade_item.pos_type = PositionType::POS_SHORT;
            trade_item.quantity = (*iter)->qty_available;
            if( p_cur_price )
                trade_item.price = *p_cur_price + cst_per_tick;
            else
                trade_item.price = (k_item.open_price > (*iter)->stop_loss_price ? k_item.open_price : (*iter)->stop_loss_price + cst_per_tick);
            trade_item.profit = ((*iter)->price - trade_item.price) / cst_per_tick * cst_per_tick_capital;
            trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
            ret.push_back(trade_item); 
            capital_ret += cst_margin_capital * trade_item.quantity;
            profit += trade_item.profit - trade_item.fee;
            ret_ids.push_back((*iter)->trade_id);
            (*iter)->ClearAvaliable();
            //iter = short_positions_.erase(iter);
        }else
            ++iter;
    }
    if( p_profit )
        *p_profit = ProcDecimal(profit, 0);
    return ret;
}

#if 0 
// ps : make sure h_price > l_price
std::vector<TradeRecordAtom> PositionInfo::DoIfStopProfit(int date, int hhmm, double h_price, double l_price, double *p_profit)
{
    assert(l_price > EPSINON);
    assert(h_price + EPSINON > l_price);

    std::vector<TradeRecordAtom> ret;
    double profit = 0.0;
    for( int i = long_positions_.size() - 1; i >= 0 ; --i ) 
    {
        if( h_price > long_positions_.at(i).stop_profit_price - EPSINON ) // >= stop profit price
        {
            TradeRecordAtom  trade_item;
            trade_item.date = date;
            trade_item.hhmm = hhmm;
            trade_item.action = OrderAction::CLOSE;
            trade_item.pos_type = PositionType::POS_LONG;
            trade_item.quantity = long_positions_.at(i).qty;
            trade_item.price = long_positions_.at(i).stop_profit_price - cst_per_tick;
            trade_item.profit = (trade_item.price - long_positions_.at(i).price) / cst_per_tick * cst_per_tick_capital;
            trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
            ret.push_back(trade_item);
            profit += trade_item.profit - trade_item.fee;
        }
    }

    for( int i = short_positions_.size() - 1; i >= 0 ; --i ) 
    {
        if( l_price + EPSINON < short_positions_.at(i).stop_profit_price )
        {
            TradeRecordAtom  trade_item;
            trade_item.date = date;
            trade_item.hhmm = hhmm;
            trade_item.action = OrderAction::CLOSE;
            trade_item.pos_type = PositionType::POS_SHORT;
            trade_item.quantity = short_positions_.at(i).qty;
            trade_item.price = short_positions_.at(i).stop_profit_price + cst_per_tick;
            trade_item.profit = (short_positions_.at(i).price - trade_item.price) / cst_per_tick * cst_per_tick_capital;
            trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
            ret.push_back(trade_item);
            profit += trade_item.profit - trade_item.fee;
        }
    }
    if( p_profit )
        *p_profit = profit;
    return ret;
}

std::vector<TradeRecordAtom> PositionInfo::DoIfStopLoss(int date, int hhmm, double h_price, double l_price, double *p_profit)
{
    assert(l_price > EPSINON);
    assert(h_price + EPSINON > l_price);

    std::vector<TradeRecordAtom> ret;
    double profit = 0.0;
    for( int i = long_positions_.size() - 1; i >= 0 ; --i ) 
    {
        if( l_price < long_positions_.at(i).stop_loss_price - EPSINON ) // <= stop loss price
        {
            TradeRecordAtom  trade_item;
            trade_item.date = date;
            trade_item.hhmm = hhmm;
            trade_item.action = OrderAction::CLOSE;
            trade_item.pos_type = PositionType::POS_LONG;
            trade_item.quantity = long_positions_.at(i).qty;
            trade_item.price = long_positions_.at(i).stop_loss_price - cst_per_tick;
            trade_item.profit = (trade_item.price - long_positions_.at(i).price) / cst_per_tick * cst_per_tick_capital;
            trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
            ret.push_back(trade_item);
            profit += trade_item.profit - trade_item.fee;
        }
    }

    for( int i = short_positions_.size() - 1; i >= 0 ; --i ) 
    {
        if( h_price + EPSINON > short_positions_.at(i).stop_loss_price )
        {
            TradeRecordAtom  trade_item;
            trade_item.date = date;
            trade_item.hhmm = hhmm;
            trade_item.action = OrderAction::CLOSE;
            trade_item.pos_type = PositionType::POS_SHORT;
            trade_item.quantity = short_positions_.at(i).qty;
            trade_item.price = short_positions_.at(i).stop_loss_price + cst_per_tick;
            trade_item.profit = (short_positions_.at(i).price - trade_item.price) / cst_per_tick * cst_per_tick_capital;
            trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
            ret.push_back(trade_item);
            profit += trade_item.profit - trade_item.fee;
        }
    }
    if( p_profit )
        *p_profit = profit;
    return ret;
}
#endif 

std::vector<TradeRecordAtom> PositionInfo::CloseAvaliable(bool target_long, double price, unsigned int qty, double &capital_ret, double *p_profit, std::vector<int> *p_ret_close_ids)
{
    std::vector<TradeRecordAtom> ret;
    assert( qty > 0 );
    T_PositionAtoms * p_positions = target_long ? &long_positions_ : &short_positions_;
    unsigned int remain_tgt_qty = qty;
    unsigned int pos_quantity = target_long ? LongPosQty() : ShortPosQty();
    assert( pos_quantity >= remain_tgt_qty );

    double profit = 0.0;
    capital_ret = 0.0;
    for( auto iter = p_positions->begin(); iter != p_positions->end(); )
    {
        if( (*iter)->qty_available == 0 )
        {
            ++iter;
            continue;
        }
        double this_profit = 0.0;
        int this_qty = 0;
        if( (*iter)->qty_available >= remain_tgt_qty )
        {
            this_qty = remain_tgt_qty;
            this_profit = (price - (*iter)->price) / cst_per_tick * cst_per_tick_capital * remain_tgt_qty;
            capital_ret += cst_margin_capital * remain_tgt_qty;
            if( (*iter)->qty_available == remain_tgt_qty )
            {
                if( p_ret_close_ids ) p_ret_close_ids->push_back((*iter)->trade_id);
                if( (*iter)->qty_frozen() == 0 ) iter = p_positions->erase(iter);
                else ++iter;
            }else // >
            {
                (*iter)->qty_available -= remain_tgt_qty;
                ++iter;
            }
            remain_tgt_qty = 0; 
        }else
        {
            this_qty = (*iter)->qty_available;
            this_profit = (price - (*iter)->price) / cst_per_tick * cst_per_tick_capital * this_qty;
            remain_tgt_qty -= this_qty;
            capital_ret += cst_margin_capital * this_qty;
            if( p_ret_close_ids ) p_ret_close_ids->push_back((*iter)->trade_id);
            if( (*iter)->qty_frozen() == 0 ) iter = p_positions->erase(iter);
            else ++iter;
        }
        profit += this_profit;

        TradeRecordAtom item;
        item.trade_id = GenerateTradeId();
        item.date = 0;
        item.hhmm = 0;
        item.action = OrderAction::CLOSE;
        item.pos_type = target_long ? PositionType::POS_SHORT : PositionType::POS_LONG;
        item.quantity = this_qty;
        item.price = price;
        item.profit = this_profit;
        item.fee = CalculateFee(item.quantity, price, true);
        ret.push_back(item);
        profit -= item.fee;

        if( remain_tgt_qty == 0 )
            break;
    } // for 

    if( p_profit )
        *p_profit = profit;
    return ret;
}
 
void PositionInfo::PushBack(bool is_long, std::shared_ptr<PositionAtom> &item)
{
    assert(item->trade_id >= 0);
    auto iter = position_holder_.insert(std::make_pair(item->trade_id, item)).first;
    if( is_long )
        long_positions_.push_back(iter->second.get());
    else
        short_positions_.push_back(iter->second.get());
}

PositionAtom * PositionInfo::PopBack(bool is_long)
{
    PositionAtom *ret = nullptr;
    if( is_long ) 
    {
        assert( !long_positions_.empty() );
        ret = long_positions_.at(long_positions_.size() - 1);
        long_positions_.pop_back();
    }else
    {
        assert( !short_positions_.empty() );
        ret = short_positions_.at(short_positions_.size() - 1);
        short_positions_.pop_back();
    }
    return ret;
}

PositionAtom * PositionInfo::FindPositionAtom(int id)
{
    auto iter = position_holder_.find(id);
    if( iter != position_holder_.end() )
        return iter->second.get();
    else
        return nullptr;
}

void PositionInfo::RemoveAtom(int id)
{
    auto iter = position_holder_.find(id);
    if( iter == position_holder_.end() )
        return;
    T_PositionAtoms *p_position_array = iter->second->is_long ? &long_positions_ : &short_positions_;
    auto atom_iter = p_position_array->begin();
    for( unsigned int i = 0; i < p_position_array->size(); ++i )
    {
        if( p_position_array->at(i)->trade_id == id )
        {
            p_position_array->erase(atom_iter++);
            break;
        }
    }
}

TradeRecordAtom  PositionInfo::ClosePositionAtom(int id, double price, double *capital_ret)
{
    //capital_ret = 0.0;
    PositionType  pos_type;
    bool is_find = false;
    auto iter = std::find_if(short_positions_.begin(), short_positions_.end(), [id](T_PositionAtoms::reference pos){ return pos->trade_id == id; });
    if( iter != short_positions_.end() )
    {
        pos_type = PositionType::POS_SHORT;
        is_find = true;
    }else
    {
        iter = std::find_if(long_positions_.begin(), long_positions_.end(), [id](T_PositionAtoms::reference pos){ return pos->trade_id == id; });
        if( iter != long_positions_.end() )
        {
            pos_type = PositionType::POS_LONG;
            is_find = true;
        }
    }

    if( is_find )
    {
        TradeRecordAtom trade_item;
        trade_item.trade_id = GenerateTradeId();
        trade_item.date = TSystem::Today();
        trade_item.hhmm = cur_hhmm();
        trade_item.action = OrderAction::CLOSE;
        trade_item.pos_type = pos_type;
        trade_item.price = price;
        trade_item.quantity = (*iter)->qty_all();  // close all position of this record 
        trade_item.profit =(*iter)->FloatProfit(trade_item.price);

        trade_item.fee = CalculateFee(trade_item.quantity, trade_item.price, trade_item.action);
         
        if( capital_ret )
            *capital_ret = cst_margin_capital * trade_item.quantity + trade_item.profit - trade_item.fee;

        if( pos_type == PositionType::POS_SHORT )
            short_positions_.erase(iter);
        else
            long_positions_.erase(iter);

        return trade_item;
    }else
        return TradeRecordAtom();

}


std::string ToStr(OrderAction action)
{
    switch(action)
    {
    case OrderAction::OPEN:  return "开"; break;
    case OrderAction::CLOSE: return "平"; break;
        //case OrderAction::UNFREEZE: return "解冻";break;
    default:assert(false);
    }
    return "";
}

std::string ToStr(PositionType pos_tp)
{
    switch(pos_tp)
    {
    case PositionType::POS_LONG:  return "多头"; break;
    case PositionType::POS_SHORT: return "空头"; break; 
    default:assert(false);
    }
    return "";
}


double CalculateFee(int quantity, double price, bool is_close)
{
    assert(quantity > 0.0);
    //assert(price > 0.0);
    return is_close ? quantity * cst_per_hand_close_fee : quantity * cst_per_hand_open_fee;
}

int CalculateMaxQtyAllowOpen(double capital, double price)
{
    assert(capital > 0.0);
    assert(price > 0.0);
    int qty = capital / cst_margin_capital;
    if( qty < 1 )
        return 0;
    double fee = CalculateFee(qty, price, false);
    if( qty == 1 && qty * cst_margin_capital + fee + cst_per_tick_capital > capital )
        return 0;

    for( int i = 0; i < 100; ++i )
    {
        if( qty > 1 && qty * cst_margin_capital + fee + cst_per_tick_capital > capital ) // capital is not enough
        {
            qty -= 1;
            fee = CalculateFee(qty, price, false);
        }else
            break;
    }
    return qty < 1 ? 0 : qty;
}
