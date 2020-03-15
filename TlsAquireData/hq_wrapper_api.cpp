#include "hq_wrapper_api.h"

#include <iostream>

#include "ExHqApi.h"

#include "exch_calendar.h"

#pragma comment(lib, "TradeX2-M.lib")

#define FIX_TTY_BUG

static int ConnectServer();

ExchangeCalendar exchange_calendar;

int HqWrapperApi_Init()
{
    int ret = ConnectServer();
    return ret > -1;
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


int ConnectServer()
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