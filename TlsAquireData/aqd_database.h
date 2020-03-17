#ifndef AQUIRE_DATA_BASE_SDFS_H_
#define AQUIRE_DATA_BASE_SDFS_H_
 
#include <boost/thread.hpp>  
#include <boost/thread/recursive_mutex.hpp>  
#include <boost/thread/mutex.hpp>

//#include <string>
#include <atomic>
#include <unordered_map>
#include <list>

#include <SQLite/SQLite.h>
#include <TLib/core/tsystem_task_service.h>

#include "tls_common.h"

//struct T_StockCodeName
//{
//    std::string code;
//    std::string name;
//    int type;  // 0 --normal stock ; 1--index 
//    int nmarket;
//    T_StockCodeName(const std::string &cd, const std::string &nm, int tp, int maket) : code(cd), name(nm), type(tp), nmarket(maket){}
//    T_StockCodeName(const T_StockCodeName &lh) : code(lh.code), name(lh.name), type(lh.type), nmarket(lh.nmarket){}
//};

typedef std::list<T_KbarData>  T_KbarDataContainer;
typedef std::list<std::shared_ptr<T_KbarDataContainer> >  T_KbarDataBufContainer;
namespace SQLite
{
    class SQLiteConnection;
}

namespace TSystem
{
    class TaskStrand;
}
//class FuturesForecastApp;
//class StockMan;
class AquireDataApp;
class DataBase
{
public:

    DataBase(AquireDataApp *app);
    ~DataBase();

    bool Initialize();

    //void LoadAllStockBaseInfo(std::shared_ptr<StockMan> &stock_man);
    void LoadTradeDate(void *exchange_calendar);

    //void GetStockCode(const std::string &code, std::vector<T_StockCodeName>& ret);

    void SaveKbarData(std::shared_ptr<T_KbarDataContainer> &kbar_datas , TlsTypePeriod type);

    void TriggerProcessSave();
    void ProcessSave();

private:
     
    DataBase(DataBase&);
    DataBase& operator = (DataBase&);

    void Open(std::shared_ptr<SQLite::SQLiteConnection>& db_conn);
    void HandleSaveKbarData(std::shared_ptr<T_KbarDataContainer> &kbar_datas , TlsTypePeriod type);

    //TSystem::LocalLogger *local_logger_;
    AquireDataApp *app_;
    std::shared_ptr<TSystem::TaskStrand>       strand_;

    std::shared_ptr<SQLite::SQLiteConnection>  db_conn_;

    SQLite::SQLiteStatement                    kdata_mon_stm_;
    SQLite::SQLiteStatement                    kdata_week_stm_;
    SQLite::SQLiteStatement                    kdata_day_stm_;
    SQLite::SQLiteStatement                    kdata_hour_stm_;
    SQLite::SQLiteStatement                    kdata_30m_stm_;
    SQLite::SQLiteStatement                    kdata_15m_stm_;
    SQLite::SQLiteStatement                    kdata_5m_stm_;
    SQLite::SQLiteStatement                    kdata_1m_stm_;

    T_KbarDataBufContainer        kdata_mon_waitting_buffer_;
    T_KbarDataBufContainer        kdata_mon_process_buffer_;

    T_KbarDataBufContainer        kdata_week_waitting_buffer_;
    T_KbarDataBufContainer        kdata_week_process_buffer_;

    T_KbarDataBufContainer        kdata_day_waitting_buffer_;
    T_KbarDataBufContainer        kdata_day_process_buffer_;

    T_KbarDataBufContainer        kdata_hour_waitting_buffer_;
    T_KbarDataBufContainer        kdata_hour_process_buffer_;

    T_KbarDataBufContainer        kdata_30m_waitting_buffer_;
    T_KbarDataBufContainer        kdata_30m_process_buffer_;

    T_KbarDataBufContainer        kdata_15m_waitting_buffer_;
    T_KbarDataBufContainer        kdata_15m_process_buffer_;

    T_KbarDataBufContainer        kdata_5m_waitting_buffer_;
    T_KbarDataBufContainer        kdata_5m_process_buffer_;

    T_KbarDataBufContainer        kdata_1m_waitting_buffer_;
    T_KbarDataBufContainer        kdata_1m_process_buffer_;

    typedef boost::shared_mutex            WRMutex;  
    typedef boost::unique_lock<WRMutex>    WriteLock;  
    typedef boost::shared_lock<WRMutex>    ReadLock;  

    bool                  process_flag_;
};
 


#endif