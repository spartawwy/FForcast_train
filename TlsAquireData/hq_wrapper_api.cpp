#include "hq_wrapper_api.h"

#include <boost/lexical_cast.hpp>
#include <cassert>
#include <iostream>
#include <regex>

#include <thread>
#include <QDebug>

#include "ExHqApi.h"

#include "exch_calendar.h"
#include "write_log.h"

#pragma comment(lib, "TradeX2-M.lib")

#define FIX_TTY_BUG

ExchangeCalendar exchange_calendar;



//void test()
//{
//    std::list<int> a;
//    
//}

HqWrapperConcrete *GetInstance()
{
    static HqWrapperConcrete hq_app;
    return &hq_app;
}

int HqWrapperApi_Init()
{
   return (int)GetInstance()->Init();
}

void HqWrapperApi_Destroy()
{

}

int HqWrapperApi_GetHisKBars(const char* code, bool is_index, int nmarket, int kbar_type
                , int start_date, int start_time
                , int end_date, int end_time
                , T_KbarData *items, unsigned int item_max_size)
{

    return 0;
}


void little_sleep(int ms) 
{
  auto start = std::chrono::high_resolution_clock::now();
  auto end = start + std::chrono::milliseconds(ms);
  do 
  {
    std::this_thread::yield();
  } while (std::chrono::high_resolution_clock::now() < end);
}
 
// asynchronous 
bool HqWrapperApi_GetAllHisKBars(const char* para_code, bool para_is_index, int para_nmarket, int para_kbar_type)
{
    return GetInstance()->GetAllHisBars(para_code, para_is_index, para_nmarket, para_kbar_type);
}

bool HqWrapperApi_IsFinishGettingData()
{
    return GetInstance()->IsFinishGettingData();
}

HqWrapperConcrete::HqWrapperConcrete()
    : is_geting_data_(false)
    , conn_handle_(-1)
{

}

HqWrapperConcrete::~HqWrapperConcrete()
{

}

bool HqWrapperConcrete::Init()
{
    return ConnectServer();
}

bool HqWrapperConcrete::ConnectServer()
{
    std::lock_guard<std::mutex> locker(conn_handle_mutext_);
    conn_handle_ = _ConnectServer();
    return conn_handle_ > -1;
}

void HqWrapperConcrete::DisConnect()
{
    std::lock_guard<std::mutex> locker(conn_handle_mutext_);

    if( conn_handle_ > -1 )
        TdxExHq_Disconnect(conn_handle_);
    conn_handle_ = -1;
}

bool HqWrapperConcrete::ReconnectServer()
{ 
    std::lock_guard<std::mutex> locker(conn_handle_mutext_);

    if( conn_handle_ > -1 )
        TdxExHq_Disconnect(conn_handle_);

    conn_handle_ = _ConnectServer();
    return conn_handle_ > -1;
}

bool HqWrapperConcrete::IsConnhandleValid()
{
    std::lock_guard<std::mutex> locker(conn_handle_mutext_);
    return conn_handle_ > -1;
}

bool HqWrapperConcrete::GetAllHisBars(const char* para_code, bool para_is_index, int para_nmarket, int para_kbar_type)
{
    if( is_geting_data_ )
        return false;
    std::thread  thread_get_data([para_code, para_is_index, para_nmarket, para_kbar_type, this]()
    {
        std::lock_guard<std::mutex>  locker(data_get_mutext_);

        this->is_geting_data_= true;
        std::list<T_KbarData>  items;
        char code[64] = {'\0'};
        strcpy(code, para_code);
        bool is_index = para_is_index;
        int  nmarket = para_nmarket;
        int  kbar_type = para_kbar_type;

        short start = 0;
        const short max_count = 400;
        short count = max_count; 
        int ret = GetInstance()->__GetHisKBars(code, is_index, nmarket, kbar_type, start, count, items);
        while( ret >= max_count )
        {
           little_sleep(1);
           start += ret;
           count = max_count; 
           std::list<T_KbarData>  items_hlp;
           ret = GetInstance()->__GetHisKBars(code, is_index, nmarket, kbar_type, start, count, items_hlp);
           std::for_each(std::begin(items_hlp), std::end(items_hlp), [](T_KbarData& entery)
            {
                WriteLog("%d %d %.2f", entery.date, entery.hhmmss, entery.close);
            });
           items.splice(items.begin(), items_hlp);
        }
        

        this->is_geting_data_= false;
    });
    

    thread_get_data.detach();
    return true;
}

bool HqWrapperConcrete::GetHisKBars(const char* code, bool is_index, int nmarket, int kbar_type, int para_start, short count
                                   , std::list<T_KbarData> &items)
{
#if 1
    //const short max_count = MAX_K_COUNT;
    short start = para_start; //std::get<0>(tuple_index_len);  // back forward : <---
    //const short count = std::get<1>(tuple_index_len);

    short local_count = MAX_K_COUNT;
    short total_get = 0;

    start += count % MAX_K_COUNT;
    if( count / MAX_K_COUNT > 0 )
        start += (count / MAX_K_COUNT - 1) * MAX_K_COUNT;
    for( int i = count / MAX_K_COUNT ; i > 0; --i )
    {
        local_count = MAX_K_COUNT;
        // get data from (start + count)index to right(newest date):     ---start + count--->start----0|
        int ret_count = __GetHisKBars(code, is_index, nmarket, kbar_type, start, local_count, items);
        if( ret_count > 0 )
        {
            if( i > 1 )
                start -= local_count;
        }else
            --start;
        total_get += local_count;
    } 
    if( count % MAX_K_COUNT > 0 )
    {
        start -= count % MAX_K_COUNT;
        local_count = count % MAX_K_COUNT;
        __GetHisKBars(code, is_index, nmarket, kbar_type, start, local_count, items);
        total_get += local_count;
    }
#else

#endif
    /*local_logger_.LogLocal(TSystem::utility::FormatStr("GetHisKBars %d | %d %d | %d %d | %d %d", kbar_type, start_date, end_date, std::get<0>(tuple_index_len), count
        , (items.empty() ? 0 : items.back().date)
        , (items.empty() ? 0 : items.back().hhmmss)));*/

    return total_get > 0;
}

// items date is from small to big; // get data from (start + count)index to right(newest date):     ---start + count--->start----0|
int HqWrapperConcrete::__GetHisKBars(const char* code, bool is_index, int nmarket, int kbar_type, short start, short &count, std::list<T_KbarData> &items)
{
    //pFuncGetInstrumentBars = &TdxExHq_GetInstrumentBars;
    int result_count = 0;
    //std::vector<T_K_Data> resut;
    const int cst_result_len = 1024 * 1024;
    const int cst_err_len = 1024; 

    char* m_szResult = new char[cst_result_len];
    char* m_szErrInfo = new char[cst_err_len];

    memset(m_szResult, 0, cst_result_len);
    memset(m_szErrInfo, 0, cst_err_len);

    //K线数据数据种类, 0->5分钟K线    1->15分钟K线    2->30分钟K线  3->1小时K线    4->日K线  5->周K线  6->月K线  7->1分钟K线  8->1分钟K线  9->日K线  10->季K线  11->年K线
    int ktype = 4;
    switch(kbar_type)
    {
    case TlsTypePeriod::PERIOD_YEAR: ktype = 11; 
        break;
    case TlsTypePeriod::PERIOD_MON:  ktype = 6; 
        break;
    case TlsTypePeriod::PERIOD_WEEK: ktype = 5; 
        break;
    case TlsTypePeriod::PERIOD_DAY:  ktype = 4; 
        break;
    case TlsTypePeriod::PERIOD_HOUR: ktype = 3; 
        break;
    case TlsTypePeriod::PERIOD_30M:  ktype = 2; 
        break;
    case TlsTypePeriod::PERIOD_15M:  ktype = 1; 
        break;
    case TlsTypePeriod::PERIOD_5M:   ktype = 0; 
        break;
    case TlsTypePeriod::PERIOD_1M:   ktype = 7; 
        break;
    default:
        assert(false);
        ktype = 4; 
        break;
    }
    do 
    { 
        //bool1 = TdxExHq_GetInstrumentBars(nConn, 4, 30, "SC1907", 0, &Count, Result, ErrInfo);
        if( !IsConnhandleValid() )
        {
            if( !IsConnhandleValid() ) // judge again
            {
                if( !ReconnectServer() )
                    break;
            }
        }

        bool ret = false;

        do
        {
            std::lock_guard<std::mutex>  locker(conn_handle_mutext_);
            if( conn_handle_ < 0 ) 
                break;
            ret = TdxExHq_GetInstrumentBars(conn_handle_, ktype, nmarket, code, start, &count, m_szResult, m_szErrInfo);
        }while(0);

        if( !ret )
        { 
            if( ReconnectServer() )
            {
                do
                {
                    std::lock_guard<std::mutex>  locker(conn_handle_mutext_);
                    if( conn_handle_ < 0 ) 
                        break;
                    ret = TdxExHq_GetInstrumentBars(conn_handle_, ktype, nmarket, code, start, &count, m_szResult, m_szErrInfo);
                }while(0);
                if( !ret )
                    break;
            } else
                break;
        }
        if( count == 0 || strlen(m_szResult) < 1 )
        {
            std::cout << " result empty !" << std::endl;
            break;
        }
        result_count = count;
        qDebug() << m_szResult << "\n";

        const bool has_time = ( ktype < 4 || ktype == 7 || ktype == 8 ) ? true : false;
        std::string expresstion_str;
        std::string expresstion_str1;
        if( has_time )
        {
            //2019-05-30 15:00  476.100037      476.500031      475.900024      476.100037      32384   2802    0.000000
            //时间                     开盘价         最高价           最低价          收盘价        持仓    成交    结算
            expresstion_str = "^(\\d{4})-(\\d{2})-(\\d{2})\\s+(\\d{2}):(\\d{2})\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+\\.\\d+).*";
#ifdef FIX_TTY_BUG
            expresstion_str1 = "^(\\d{4})--(\\d{2})--(\\d{2})\\s+(\\d{2}):(\\d{2})\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+\\.\\d+).*";
#endif
        }else
        {   //2019-05-30        476.100037      476.500031      475.900024      476.100037      32384   2802    0.000000
            expresstion_str = "^(\\d{8})\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+\\.\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+\\.\\d+).*";
        }

#ifdef FIX_TTY_BUG
        std::regex  regex_obj1(expresstion_str1);
#endif
        std::regex  regex_obj(expresstion_str);
        char *p = m_szResult;
        while( *p != '\0' && *p != '\n') ++p;

        ++p;

        char *p_line_beg = p;
        while( *p != '\0')
        {
            p_line_beg = p;
            while( *p != '\0' && *p != '\n')++p;
            *p = '\0';
            ++p;
            std::string src_str = p_line_beg; 
            std::smatch  match_result;
            if( std::regex_match(src_str.cbegin(), src_str.cend(), match_result, regex_obj) )
            {
                int index = 1;
                T_KbarData  k_data;
                try
                {
                    //std::cout << match_result[index] << " "; // date 
                    if( has_time )
                    { 
                        int year = boost::lexical_cast<int>(match_result[index]);
                        ++index;
                        int mon = boost::lexical_cast<int>(match_result[index]);
                        ++index;
                        int day = boost::lexical_cast<int>(match_result[index]);
                        ++index;
                        k_data.date = year * 10000 + mon * 100 + day;
                        int hour = boost::lexical_cast<int>(match_result[index]);
                        ++index;  
                        int minute = boost::lexical_cast<int>(match_result[index]);
                        k_data.hhmmss = hour * 100 + minute;
#if 1
                        if( k_data.hhmmss > 2100 )
                        {
                            //k_data.date = exchange_calendar_->PreTradeDate(k_data.date, 1);
                        }
#endif 
                    }else
                    {
                        k_data.date = boost::lexical_cast<int>(match_result[index]);
                        k_data.hhmmss = 0;
                    }

                    ++index;
                    k_data.open = boost::lexical_cast<double>(match_result[index]);
                    ++index;
                    k_data.high = boost::lexical_cast<double>(match_result[index]);
                    ++index;
                    k_data.low = boost::lexical_cast<double>(match_result[index]);
                    ++index;
                    k_data.close = boost::lexical_cast<double>(match_result[index]);
                    ++index;
                    int hold = boost::lexical_cast<int>(match_result[index]);
                    ++index;
                    k_data.vol = boost::lexical_cast<double>(match_result[index]);
                    ++index;
                    //k_data.capital = boost::lexical_cast<double>(match_result[index]); // 结算价
                    items.push_back(std::move(k_data));

                }catch(boost::exception& e )
                {
                    e = e; 
                }
            }else
            {
#ifdef FIX_TTY_BUG

                std::smatch  match_result;
                if( std::regex_match(src_str.cbegin(), src_str.cend(), match_result, regex_obj1) )
                {
                    int index = 1;
                    T_KbarData  k_data;
                    try
                    { 
                        if( has_time )
                        { 
                            int year = boost::lexical_cast<int>(match_result[index]);
                            if( year < 1990 )
                                year += 31;
                            ++index;
                            int mon = boost::lexical_cast<int>(match_result[index]);
                            //qDebug() << mon << "\n";
                            mon = 20 - mon;
                            ++index;
                            int day = boost::lexical_cast<int>(match_result[index]);
                            day = 48 - day;
                            ++index;
                            k_data.date = year * 10000 + mon * 100 + day;
                            int hour = boost::lexical_cast<int>(match_result[index]);
                            ++index;  
                            int minute = boost::lexical_cast<int>(match_result[index]);
                            k_data.hhmmss = hour * 100 + minute;
                            if( k_data.hhmmss > 2100 )
                            {
                                //k_data.date = exchange_calendar_->PreTradeDate(k_data.date, 1);
                            }
                        }else
                        {
                            k_data.date = boost::lexical_cast<int>(match_result[index]);
                            k_data.hhmmss = 0;
                        }

                        ++index;
                        k_data.open = boost::lexical_cast<double>(match_result[index]);
                        ++index;
                        k_data.high = boost::lexical_cast<double>(match_result[index]);
                        ++index;
                        k_data.low = boost::lexical_cast<double>(match_result[index]);
                        ++index;
                        k_data.close = boost::lexical_cast<double>(match_result[index]);
                        ++index;
                        int hold = boost::lexical_cast<int>(match_result[index]);
                        ++index;
                        k_data.vol = boost::lexical_cast<double>(match_result[index]);
                        ++index;
                        //k_data.capital = boost::lexical_cast<double>(match_result[index]); // 结算价
                        items.push_back(std::move(k_data));

                    }catch(boost::exception& e )
                    {
                        e = e; 
                    }
                }
#else
                qDebug() << "match fail!\n";
#endif
            }
        }
        //std::cout << m_szResult << std::endl; 

    } while (0);

    delete []m_szResult;
    delete []m_szErrInfo;

    return result_count;
}


int HqWrapperConcrete::_ConnectServer()
{ 
    short port = 7727;
    int handle = -1;
    const int cst_result_len = 1024 * 1024;
    const int cst_err_len = 1024; 

    char* m_szResult = new char[cst_result_len];
    char* m_szErrInfo = new char[cst_err_len];

    memset(m_szResult, 0, cst_result_len);
    memset(m_szErrInfo, 0, cst_err_len);

    //连接服务器
    char * servers[] = {
        "106.14.95.149"
        , "59.175.238.38"
        , "112.74.214.43"};
    for( int i = 0; i < sizeof(servers)/sizeof(servers[0]); ++i )
    {
        handle = TdxExHq_Connect(servers[i], port, m_szResult, m_szErrInfo);
        if( handle < 0 )
        {
            handle = TdxExHq_Connect(servers[i], port, m_szResult, m_szErrInfo);
            if( handle < 0 )
            {
                std::cout << m_szErrInfo << std::endl;
                continue;
            }else
            {
                std::cout << m_szResult << std::endl;
                return handle;
            }
        }else
        {
            std::cout << m_szResult << std::endl;
            return handle;
        }
    } 
    return handle;
}
