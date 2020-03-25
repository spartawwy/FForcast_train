#ifndef HQ_WRAPPER_API_
#define HQ_WRAPPER_API_

#include <mutex>
#include <vector>
#include <memory>
#include <list>

#include "tls_common.h"


#define  IMEXPORT

typedef std::list<std::unique_ptr<T_KbarData> >   T_KDataContainer;
//#ifdef __cplusplus
//extern "C" {
//#endif

// return connect handle( > -1 )
int HqWrapperApi_Init(void *exchange_calendare);
void HqWrapperApi_Destroy();

int HqWrapperApi_GetHisKBars(const char* code, bool is_index, int nmarket, int kbar_type
                 , int start_date, int start_time
                 , int end_date, int end_time
                 , T_KbarData *items, unsigned int item_max_size);

bool HqWrapperApi_GetAllHisKBars(const char* code, bool is_index, int nmarket, int kbar_type
                                 , T_GetDataCallBack *func, void *func_para);

bool HqWrapperApi_IsFinishGettingData();
//#ifdef __cplusplus
//}
//#endif

class ExchangeCalendar;
class HqWrapperConcrete
{
public:

    HqWrapperConcrete();
    ~HqWrapperConcrete();

    bool Init(ExchangeCalendar &calendar);
    bool ConnectServer();
    void DisConnect();
    bool ReconnectServer();

    bool IsConnhandleValid();

    bool GetHisKBars(const char* code, bool is_index, int nmarket, int kbar_type, int start, short count
                   , T_KDataContainer &items);

    bool GetAllHisBars(const char* para_code, bool para_is_index, int para_nmarket, int para_kbar_type
                   , T_GetDataCallBack *func, void *func_para);

    // items date is from small to big; // get data from start index to left(oldest date):     <---len---0 
    int __GetHisKBars(const char* code, bool is_index, int nmarket, int kbar_type, short start, short &count, T_KDataContainer &items);
    
    bool IsFinishGettingData(){ return !is_geting_data_; }
    
private:

    int _ConnectServer();

    std::shared_ptr<ExchangeCalendar>  exchange_calendar_;

    int conn_handle_;

    std::mutex conn_handle_mutext_;
    std::mutex data_get_mutext_;
    //std::mutex data_get_inner_mutext_;

    bool is_geting_data_;

    T_GetDataCallBack *p_get_data_callback;
    void *p_func_para;
};

#endif