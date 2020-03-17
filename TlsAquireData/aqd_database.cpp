#include "aqd_database.h"

#include <boost/lexical_cast.hpp>
#include <cctype>
#include <memory>

#include <SQLite/sqlite_connection.h>

#include <TLib/core/tsystem_core_common.h>
#include <TLib/core/tsystem_sqlite_functions.h>
#include <TLib/core/tsystem_utility_functions.h>
#include <Tlib/core/tsystem_time.h>
#include <TLib/core/tsystem_task_service.h>

#include "exch_calendar.h"

#include "aquire_data_app.h" 

using namespace SQLite;
using namespace  TSystem;

DataBase::DataBase(AquireDataApp *app)
    : app_(app)
    , strand_(std::make_shared<TSystem::TaskStrand>(app->task_pool()))
    , db_conn_(std::make_shared<SQLite::SQLiteConnection>())
    , kdata_mon_stm_(db_conn_)
    , kdata_week_stm_(db_conn_)
    , kdata_day_stm_(db_conn_)
    , kdata_hour_stm_(db_conn_)
    , kdata_30m_stm_(db_conn_)
    , kdata_15m_stm_(db_conn_)
    , kdata_5m_stm_(db_conn_)
    , kdata_1m_stm_(db_conn_)
    , process_flag_(false)
{
    /*kdata_mon_waitting_buffer_.reserve(1024);
    kdata_mon_process_buffer_.reserve(1024);

    kdata_5m_waitting_buffer_.reserve(1024);
    kdata_5m_process_buffer_.reserve(1024);*/
     
}

DataBase::~DataBase()
{
    if( db_conn_ )
        db_conn_->Close();
}

bool DataBase::Initialize()
{
    Open(db_conn_);

    // setup busy timeout : hard code for 60s
	db_conn_->SetBusyTimeout(60*1000);

    //----------------
    // create table
    //---------------
    std::string sql;
    sql = "CREATE TABLE IF NOT EXISTS SCL9_MON( "
        "longdate INTEGER, time INTEGER, "
        "open DOUBLE, close DOUBLE, high DOUBLE, low DOUBLE, "
        "PRIMARY KEY(longdate, time) )";
    db_conn_->ExecuteSQL(sql.c_str());

    sql = "CREATE TABLE IF NOT EXISTS SCL9_WEEK( "
        "longdate INTEGER, time INTEGER, "
        "open DOUBLE, close DOUBLE, high DOUBLE, low DOUBLE, "
        "PRIMARY KEY(longdate, time) )";
    db_conn_->ExecuteSQL(sql.c_str());

    sql = "CREATE TABLE IF NOT EXISTS SCL9_DAY( "
        "longdate INTEGER, time INTEGER, "
        "open DOUBLE, close DOUBLE, high DOUBLE, low DOUBLE, "
        "PRIMARY KEY(longdate, time) )";
    db_conn_->ExecuteSQL(sql.c_str());

    sql = "CREATE TABLE IF NOT EXISTS SCL9_HOUR( "
        "longdate INTEGER, time INTEGER, "
        "open DOUBLE, close DOUBLE, high DOUBLE, low DOUBLE, "
        "PRIMARY KEY(longdate, time) )";
    db_conn_->ExecuteSQL(sql.c_str());

    sql = "CREATE TABLE IF NOT EXISTS SCL9_30M( "
        "longdate INTEGER, time INTEGER, "
        "open DOUBLE, close DOUBLE, high DOUBLE, low DOUBLE, "
        "PRIMARY KEY(longdate, time) )";
    db_conn_->ExecuteSQL(sql.c_str());

    sql = "CREATE TABLE IF NOT EXISTS SCL9_15M( "
        "longdate INTEGER, time INTEGER, "
        "open DOUBLE, close DOUBLE, high DOUBLE, low DOUBLE, "
        "PRIMARY KEY(longdate, time) )";
    db_conn_->ExecuteSQL(sql.c_str());

    sql = "CREATE TABLE IF NOT EXISTS SCL9_5M( "
        "longdate INTEGER, time INTEGER, "
        "open DOUBLE, close DOUBLE, high DOUBLE, low DOUBLE, "
        "PRIMARY KEY(longdate, time) )";
    db_conn_->ExecuteSQL(sql.c_str());

    sql = "CREATE TABLE IF NOT EXISTS SCL9_1M( "
        "longdate INTEGER, time INTEGER, "
        "open DOUBLE, close DOUBLE, high DOUBLE, low DOUBLE, "
        "PRIMARY KEY(longdate, time) )";
    db_conn_->ExecuteSQL(sql.c_str());

    //--------------
	// insert table
	//--------------

    sql = "INSERT OR IGNORE INTO SCL9_MON( "
        "longdate, time, open, close, high, low )"
        "VALUES( ?1, ?2, ?3, ?4, ?5, ?6 )";
    kdata_mon_stm_.CompileSQL(sql.c_str());


    sql = "INSERT OR IGNORE INTO SCL9_WEEK( "
        "longdate, time, open, close, high, low )"
        "VALUES( ?1, ?2, ?3, ?4, ?5, ?6 )";
    kdata_week_stm_.CompileSQL(sql.c_str());

    sql = "INSERT OR IGNORE INTO SCL9_DAY( "
        "longdate, time, open, close, high, low )"
        "VALUES( ?1, ?2, ?3, ?4, ?5, ?6 )";
    kdata_day_stm_.CompileSQL(sql.c_str());

    sql = "INSERT OR IGNORE INTO SCL9_HOUR( "
        "longdate, time, open, close, high, low )"
        "VALUES( ?1, ?2, ?3, ?4, ?5, ?6 )";
    kdata_hour_stm_.CompileSQL(sql.c_str());

    sql = "INSERT OR IGNORE INTO SCL9_30M( "
        "longdate, time, open, close, high, low )"
        "VALUES( ?1, ?2, ?3, ?4, ?5, ?6 )";
    kdata_30m_stm_.CompileSQL(sql.c_str());

    sql = "INSERT OR IGNORE INTO SCL9_15M( "
        "longdate, time, open, close, high, low )"
        "VALUES( ?1, ?2, ?3, ?4, ?5, ?6 )";
    kdata_15m_stm_.CompileSQL(sql.c_str());

    sql = "INSERT OR IGNORE INTO SCL9_5M( "
        "longdate, time, open, close, high, low )"
        "VALUES( ?1, ?2, ?3, ?4, ?5, ?6 )";
    kdata_5m_stm_.CompileSQL(sql.c_str());

    sql = "INSERT OR IGNORE INTO SCL9_1M( "
        "longdate, time, open, close, high, low )"
        "VALUES( ?1, ?2, ?3, ?4, ?5, ?6 )";
    kdata_1m_stm_.CompileSQL(sql.c_str());

    return db_conn_ != nullptr;
}

void DataBase::Open(std::shared_ptr<SQLite::SQLiteConnection>& db_conn/*, const std::string db_file*/)
{
    //db_conn = std::make_shared<SQLite::SQLiteConnection>();

    std::string db_file = "./hqhis.kd";

    if( db_conn->Open(db_file.c_str(), SQLite::SQLiteConnection::OpenMode::READ_WRITE) != SQLite::SQLiteCode::OK )
        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
        , "DBMoudle::Open"
        , "can't open database: " + db_file);

}

#if 0
void DataBase::LoadAllStockBaseInfo(std::shared_ptr<StockMan> &stock_man)
{
    assert(db_conn_);
    assert(stock_man->code_stock_baseinfo_item_map_.empty());
    assert(stock_man->pinyin_stock_baseinfo_item_map_.empty());

    if( !utility::ExistTable("stock", *db_conn_) )
        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
        , "DataBase::LoadAllStockBaseInfo"
        , "can't find table stock: ");

    std::string sql = utility::FormatStr("SELECT code, type, name, pinyin, timeToMarket, industry, area, remark FROM stock ORDER BY code");
    db_conn_->ExecuteSQL(sql.c_str(), [this, &stock_man](int /*num_cols*/, char** vals, char** /*names*/)->int
    {
        auto item = std::make_shared<T_StockBaseInfoItem>();
        try
        {
            item->code = *vals;
            item->type = boost::lexical_cast<int>(*(vals + 1));
            if( *(vals + 2) )
                item->name = *(vals + 2);
            if( *(vals + 3) )
                item->pinyin = *(vals + 3);
            item->time_to_market = boost::lexical_cast<int>(*(vals + 4));
            if( *(vals + 5) )
                item->industry = *(vals + 5);
            if( *(vals + 6) )
                item->area = *(vals + 6);
            if( *(vals + 7) )
                item->remark = *(vals + 7);
        }catch(boost::exception & )
        { 
            return 0;
        }

        stock_man->pinyin_stock_baseinfo_item_map_.insert( std::make_pair(item->pinyin, item) );
        stock_man->code_stock_baseinfo_item_map_.insert( std::make_pair(item->code, std::move(item)) );
        return 0;
    });
}
#endif


void DataBase::LoadTradeDate(void *exchange_calendar)
{
    assert(db_conn_);
    if( !db_conn_ )
        Open(db_conn_);

    if( !utility::ExistTable("ExchangeDate", *db_conn_) )
        throw "DBMoudle::LoadTradeDate can't find table ExchangeDate"; 

    int end_date = ExchangeCalendar::TodayAddDays(7); 
    std::string sql = utility::FormatStr("SELECT date, is_tradeday FROM ExchangeDate WHERE date <= %d ORDER BY date DESC", end_date);
    int num = 0;
    ((ExchangeCalendar*)exchange_calendar)->max_trade_date_ = 0;
    db_conn_->ExecuteSQL(sql.c_str(), [&num, &exchange_calendar, this](int /*num_cols*/, char** vals, char** /*names*/)->int
    { 
        try
        { 
            ++num;
            int date =  boost::lexical_cast<int>(*(vals)); 
            bool is_trade_date = boost::lexical_cast<bool>(*(vals + 1)); 
            ((ExchangeCalendar*)exchange_calendar)->trade_dates_->insert(std::make_pair(date, is_trade_date));

            if( ((ExchangeCalendar*)exchange_calendar)->max_trade_date_ == 0 )
                ((ExchangeCalendar*)exchange_calendar)->max_trade_date_ = date;

            ((ExchangeCalendar*)exchange_calendar)->min_trade_date_ = date;
        }catch(boost::exception& )
        {
            return 0;
        } 
        return 0;
    }); 
}

void DataBase::SaveKbarData(std::shared_ptr<T_KbarDataContainer> &kbar_datas , TlsTypePeriod type)
{
    strand_->PostTask(std::bind(&DataBase::HandleSaveKbarData, this, kbar_datas, type));
}

void DataBase::HandleSaveKbarData(std::shared_ptr<T_KbarDataContainer>  &kbar_datas , TlsTypePeriod type)
{
    switch(type)
    {
        case TlsTypePeriod::PERIOD_5M:
            kdata_5m_waitting_buffer_.push_back(kbar_datas);
            break;
         case TlsTypePeriod::PERIOD_MON:
            kdata_mon_waitting_buffer_.push_back(kbar_datas);
            break;
        default: 
            assert(false);
            break;
    }
    if( process_flag_ )
        return;

    TriggerProcessSave();
}

void DataBase::TriggerProcessSave()
{
    if( kdata_5m_waitting_buffer_.size() || kdata_mon_waitting_buffer_.size() )
    {
        process_flag_ = true;

        kdata_5m_process_buffer_.swap(kdata_5m_waitting_buffer_);
        kdata_mon_process_buffer_.swap(kdata_mon_waitting_buffer_);

        strand_->GetTaskPool().PostTask( std::bind(&DataBase::ProcessSave, this) );
    }else
    {
        process_flag_ = false;
    }

}

void DataBase::ProcessSave()
{
    try
    {
        //-------------
		// Transaction
		//-------------

		BeginTransaction(*db_conn_);

        std::for_each( std::begin(kdata_5m_process_buffer_), std::end(kdata_5m_process_buffer_), [this](const std::shared_ptr<T_KbarDataContainer>& entry)
        {
            std::for_each( std::begin(*entry), std::end(*entry), [this](const T_KbarData& in)
            {
                this->kdata_5m_stm_.BindParam(1, in.date);
                this->kdata_5m_stm_.BindParam(2, in.hhmmss);
                this->kdata_5m_stm_.BindParam(3, in.open);
                this->kdata_5m_stm_.BindParam(4, in.close);
                this->kdata_5m_stm_.BindParam(5, in.high);
                this->kdata_5m_stm_.BindParam(6, in.low);
                this->kdata_5m_stm_.Evaluate();
            });

        });
        // todo

        EndTransaction(*db_conn_);
    }catch(const SQLiteException& e)
	{
		ThrowTException(CoreErrorCategory::ErrorCode::BAD_CONTENT
			, "DataBase::ProcessSave"
			, FormatThirdPartyError("SQLite", static_cast<int>(e.code()), e.what()));
	}
    kdata_5m_process_buffer_.clear();
    kdata_mon_process_buffer_.clear();
    strand_->PostTask(std::bind(&DataBase::TriggerProcessSave, this));
}


#if 0
void DataBase::GetStockCode(const std::string &code, std::vector<T_StockCodeName>& ret)
{
    ret.clear();

    if( !db_conn_ )
    {
        Open(db_conn_);
    }

    std::string sql;
    if( IsStrAlpha(code) )
    {
        sql = utility::FormatStr("SELECT code, name, type, nmarket FROM stock WHERE pinyin like '%s%%' ORDER BY code LIMIT 5", code.c_str());
    }else if( IsStrNum(code) )
    {
        sql = utility::FormatStr("SELECT code, name, type, nmarket FROM stock WHERE code like '%s%%' ORDER BY code LIMIT 5", code.c_str());
    }else
    {
        sql = utility::FormatStr("SELECT code, name, type, nmarket FROM stock WHERE name like '%s%%' ORDER BY code LIMIT 5", code.c_str());
    }

    if( !utility::ExistTable("stock", *db_conn_) )
        return;

    db_conn_->ExecuteSQL(sql.c_str(),[&ret, this](int /*num_cols*/, char** vals, char** /*names*/)->int
    { /*
      T_StockCodeName code_name;
      code_name.code = *vals;
      code_name.name = *(vals + 1);*/
        std::string name = *(vals + 1);
        int type = boost::lexical_cast<int>(*(vals + 2));
        int nmarket = boost::lexical_cast<int>(*(vals + 3));
        utf8ToGbk(name);
        ret.emplace_back(*vals, std::move(name), type, nmarket);
        return 0;
    });
    return;
}
#endif