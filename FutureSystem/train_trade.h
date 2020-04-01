#ifndef TRAIN_TRADE_DSFSDF_H_
#define TRAIN_TRADE_DSFSDF_H_

#include <cassert>
#include <vector>
#include <tuple>
#include <atomic>
#include <mutex>
#include <unordered_map>

#include <QString>

#include "stkfo_common.h"

#define POSITION_STATUS_FROZEN  0
#define POSITION_STATUS_AVAILABLE  1
#define POSITION_STATUS_ALL  2

static const int cst_position_target_status_available = 1;
static const double cst_per_tick = 0.1; // pow(0.1, DEFAULT_DECIMAL)
static const double cst_per_tick_capital = 100.00;
static const double cst_margin_capital = 40000.00;
static const double cst_default_ori_capital = 250000.00;
//static const double cst_default_fee_rate_percent = 0.025;

static const double cst_per_hand_open_fee = 25.0;
static const double cst_per_hand_close_fee = 0.0;

struct AccountAtom
{ 
    double  avaliable;
    double  frozen;
    double  float_profit;
    AccountAtom() : avaliable(0.0), frozen(0.0), float_profit(0.0){}
};

class TradeRecordAtom
{ 
public:
    int trade_id;
    int date;
    int hhmm;
    OrderAction action;
    PositionType pos_type;
    int quantity;
    double price;
    double profit;
    double fee;
    double price_stop_profit;
    double price_stop_loss;
    explicit TradeRecordAtom() : trade_id(-1), date(0), hhmm(0), action(OrderAction::OPEN), pos_type(PositionType::POS_LONG), quantity(0), price(0.0), profit(0.0), fee(0.0)
        , price_stop_profit(MAGIC_STOP_PRICE), price_stop_loss(MAGIC_STOP_PRICE) { }
    TradeRecordAtom(const TradeRecordAtom &lh) : trade_id(lh.trade_id), date(lh.date), hhmm(lh.hhmm), action(lh.action), pos_type(lh.pos_type), quantity(lh.quantity), price(lh.price), profit(lh.profit), fee(lh.fee)
        , price_stop_profit(lh.price_stop_profit), price_stop_loss(lh.price_stop_loss) { }
    TradeRecordAtom & operator = (const TradeRecordAtom &lh)
    {
        if( &lh == this ) 
            return *this;
        trade_id = lh.trade_id;
        date = lh.date; hhmm = lh.hhmm;
        action = lh.action; pos_type = lh.pos_type;
        quantity = lh.quantity; price = lh.price; profit = lh.profit; fee = lh.fee;
        price_stop_profit = lh.price_stop_profit; price_stop_loss = lh.price_stop_loss;
        return *this;
    }

    QString ToQStr();
};

class PositionAtom
{
public:
    PositionAtom() : trade_id(-1), price(0.0), is_long(false), stop_loss_price(MAGIC_STOP_PRICE), stop_profit_price(MAGIC_STOP_PRICE), qty_available(0), qty_frozens(){}
    PositionAtom(const PositionAtom& lh) : trade_id(lh.trade_id), price(lh.price), is_long(lh.is_long)
        , stop_loss_price(lh.stop_loss_price),stop_profit_price(lh.stop_profit_price), qty_available(lh.qty_available), qty_frozens(lh.qty_frozens){}
    PositionAtom& operator = (const PositionAtom& lh)
    { 
        if( this == &lh ) 
            return *this; 
        else 
        {
            trade_id = lh.trade_id;
            price = lh.price; 
            is_long = lh.is_long;
            stop_loss_price = lh.stop_loss_price;
            stop_profit_price = lh.stop_profit_price;
            qty_available = lh.qty_available;
            qty_frozens = lh.qty_frozens;
            //is_frozen = lh.is_frozen;
        }
        return *this; 
    }

    double FloatProfit(double price);

    unsigned int qty_all(){ return qty_available + qty_frozen(); }
    unsigned int qty_frozen();
    void Freeze(int id, unsigned qty)
    { 
        assert(qty_available >= qty);
        qty_available -= qty;
        qty_frozens.insert(std::make_pair(id, qty));
    }

    int trade_id;
    double price;
    bool is_long;
    double stop_loss_price;   // if < 0.0 means not set
    double stop_profit_price; // if < 0.0 means not set
    unsigned int qty_available;
    //<id, frozen quantity>
    std::unordered_map<int, unsigned int> qty_frozens;
};

class PositionInfo
{  
public:
     
    //PositionInfo(double open_fee=100.0, double close_fee=300.0) : open_fee_(open_fee), close_fee_(close_fee) { max_trade_id_ = 0; }
    PositionInfo() : open_fee_(cst_per_hand_open_fee), close_fee_(cst_per_hand_close_fee) { max_trade_id_ = 0; }
     
    void open_fee(double fee) { open_fee_ = fee; }
    void close_fee(double fee) { close_fee_ = fee; }
    

    void Clear(){ long_positions_.clear(); short_positions_.clear(); position_holder_.clear(); max_trade_id_ = 0;}

    int GenerateTradeId(){ return ++max_trade_id_; }
    unsigned int TotalPosition() { return LongPosQty() + ShortPosQty(); }
     
    unsigned int LongPosQty(int target_status=POSITION_STATUS_ALL);
    // (id, special position size)
    std::unordered_map<int, unsigned int> LongPosSizeInfo(int target_status=POSITION_STATUS_ALL){ return PositionSizeInfo(PositionType::POS_LONG, target_status);}
    double LongAveragePrice();
     
    unsigned int ShortPosQty(int target_status=POSITION_STATUS_ALL);
    // (id, special position size)
    std::unordered_map<int, unsigned int> ShortPosSizeInfo(int target_status=POSITION_STATUS_ALL){ return PositionSizeInfo(PositionType::POS_SHORT, target_status);}
    double ShortAveragePirce();

    unsigned int PositionQty(PositionType type, int target_status=POSITION_STATUS_ALL);
    // (id, special status position size)
    std::unordered_map<int, unsigned int> PositionSizeInfo(PositionType type,  int target_status=POSITION_STATUS_ALL);

    double FloatProfit(double price);
    // ret <low, high>
    std::tuple<double, double> GetForceClosePrices(double capital);

    std::vector<TradeRecordAtom> DoIfStopProfitLongPos(int date, int hhmm, double h_price, double &capital_ret, std::vector<int> &ret_ids, double *p_cur_price, double *p_profit);
    std::vector<TradeRecordAtom> DoIfStopProfitShortPos(int date, int hhmm, double l_price, double &capital_ret, std::vector<int> &ret_ids, double *p_cur_price, double *p_profit);
    std::vector<TradeRecordAtom> DoIfStopLossLongPos(int date, int hhmm, double l_price, double &capital_ret, std::vector<int> &ret_ids, double *p_cur_price, double *p_profit);
    std::vector<TradeRecordAtom> DoIfStopLossShortPos(int date, int hhmm, double h_price, double &capital_ret, std::vector<int> &ret_ids, double *p_cur_price, double *p_profit);
#if 0
    std::vector<TradeRecordAtom> DoIfStopProfit(int date, int hhmm, double h_price, double l_price, double *p_profit);
    std::vector<TradeRecordAtom> DoIfStopLoss(int date, int hhmm, double h_price, double l_price, double *p_profit);
#endif

    /*std::vector<PositionAtom *> FrozeLongPosWhichUnfrozen(unsigned int qty);
    std::vector<PositionAtom *> FrozeShortPosWhichUnfrozen(unsigned int qty);*/
    // return trades
    std::vector<TradeRecordAtom> CloseAvaliableLong(double price, unsigned int qty, double &capital_ret, double *p_profit, std::vector<int> *p_ret_close_ids=nullptr);
    // return trades
    std::vector<TradeRecordAtom> CloseAvaliableShort(double price, unsigned int qty, double &capital_ret, double *p_profit, std::vector<int> *p_ret_close_ids=nullptr);

    void PushBack(bool is_long, std::shared_ptr<PositionAtom> &item);

    PositionAtom * PopBack(bool is_long);

    PositionAtom * FindPositionAtom(int id);
    TradeRecordAtom  ClosePositionAtom(int id, double price, double *capital_ret);

    std::mutex mutex_;

private:
     
    PositionInfo(const PositionInfo&);
    PositionInfo& operator = (const PositionInfo&);

    double open_fee_;
    double close_fee_;
    std::unordered_map<int, std::shared_ptr<PositionAtom> > position_holder_;

    typedef std::vector<PositionAtom*> T_PositionAtoms;
    T_PositionAtoms  long_positions_;
    T_PositionAtoms  short_positions_;

    std::atomic_int max_trade_id_;
    
};

struct AccountInfo
{
    AccountAtom  capital; 
    PositionInfo  position;
};

std::string ToStr(OrderAction action);
std::string ToStr(PositionType pos_tp);

double CalculateFee(int quantity, double price, bool is_close);
int CalculateMaxQtyAllowOpen(double capital, double price);

#endif // TRAIN_TRADE_DSFSDF_H_