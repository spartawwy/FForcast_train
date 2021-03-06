#ifndef EXCHANGE_CALENDAR_SDF3SDFS_H_
#define EXCHANGE_CALENDAR_SDF3SDFS_H_

#include <vector>
#include <memory>
#include <unordered_map>
#include <tuple>

#include "tls_common.h"
 
typedef std::unordered_map<int, bool> T_DateMapIsopen;
typedef std::tuple<int, int> T_TupleIndexLen;

class ExchangeCalendar
{
public:

	 // return: yyyymmdd
    static int TodayAddDays(int days=0);
    // days can be nigative -
    static int DateAddDays(int date, int days);

    ExchangeCalendar();
    ExchangeCalendar(const ExchangeCalendar &lh);
    ExchangeCalendar & operator = (const ExchangeCalendar &lh);

    bool IsTradeDate(int date); // yyyymmdd
    bool IsTradeTime(int hhmm); // HHMM
    bool IsMidNightTradeTime(int hhmm); // HHMM
    int CeilingTradeDate(int date);
    int FloorTradeDate(int date);
    int PreTradeDate(int date, unsigned int n);
    int NextTradeDate(int date, unsigned int n);
	 
    int DateTradingSpan(int start_date, int end_date);
    T_TupleIndexLen GetStartIndexAndLen_backforward(int type_period, int start_date, int end_date);

    int Translate2DateMaySmall(int val);
    bool IsLeapYear(int Year);
    bool IsValidDate(int Year,int Month,int Day);

private:

    std::shared_ptr<T_DateMapIsopen> trade_dates_;

    //std::vector<T_CalendarDate>  calendar_date_;
    int min_trade_date_;
    int max_trade_date_;
    friend class DataBase;
};

#endif