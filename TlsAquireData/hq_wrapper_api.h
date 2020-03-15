#ifndef HQ_WRAPPER_API_
#define HQ_WRAPPER_API_

#include <mutex>
#include <vector>

#include "tls_common.h"
  

struct T_KbarData
{
    int  date; 
    int  hhmmss;
    double open;
    double close;
    double high;
    double low;
    double vol;
    T_KbarData() : date(0), hhmmss(0), open(0.0), close(0.0), high(0.0), low(0.0), vol(0.0){}
};


#define  IMEXPORT


//#ifdef __cplusplus
//extern "C" {
//#endif

// return connect handle( > -1 )
int HqWrapperApi_Init();
void HqWrapperApi_Destroy();

int HqWrapperApi_GetHisKBars(const char* code, bool is_index, int nmarket, int kbar_type
                 , int start_date, int start_time
                 , int end_date, int end_time
                 , T_KbarData *items, unsigned int item_max_size);

int HqWrapperApi_GetAllHisKBars(const char* code, bool is_index, int nmarket, int kbar_type);

//#ifdef __cplusplus
//}
//#endif

class HqWrapperConcrete
{
public:

    HqWrapperConcrete();
    ~HqWrapperConcrete();

    bool Init();
    bool ConnectServer();
    void DisConnect();
    bool ReconnectServer();

    bool IsConnhandleValid();

    bool GetHisKBars(const char* code, bool is_index, int nmarket, int kbar_type, int start, short count
                   , std::vector<T_KbarData> &items);

private:

    int _ConnectServer();

    // items date is from small to big; // get data from start index to left(oldest date):     <---len---0 
    bool __GetHisKBars(const char* code, bool is_index, int nmarket, int kbar_type, short start, short &count, std::vector<T_KbarData> &items);
    
    int conn_handle_;

    std::mutex conn_handle_mutext_;

};

#endif