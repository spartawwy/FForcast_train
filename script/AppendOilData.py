# coding=utf-8
 
import os
import time
import re
import string
import sqlite3  
#import numpy as np  
#import pandas as pd  
import tushare as ts  
import datetime as dt  
from datetime import timedelta
from datetime import datetime
from pypinyin import pinyin, lazy_pinyin
  
ROOT_DIR = './' 
DB_FILE_PATH = '../build/Win32/Debug/hqhis.kd'
#DB_FILE_PATH = './ExchBase.kd'
DATA_FILE_0 = 'D:/ProgramFilesBase/StockData/FutureData/SCL9/2018/SQSC13.csv'
DATA_FILE_1 = 'D:/ProgramFilesBase/StockData/FutureData/SCL9/2018/SQSC14.csv'
DATA_FILE_2 = 'D:/ProgramFilesBase/StockData/FutureData/SCL9/2019/SQSC13.csv'
    
class FUTUREBASIC:  
    def __init__(self):  
        self.cal_dates = ts.trade_cal() #return calendar of exchange center, DataFrame, calendarDate,isOpen  
        self.data_dir = "C:/"
        if "STK_DATA_DIR" in os.environ:
            self.data_dir = os.environ["STK_DATA_DIR"] 
        
        self.file_ok_ext = ".ok"    
        log_dir = self.data_dir + "\\log\\"
        if not os.path.exists(log_dir):
            os.makedirs(log_dir) 
        self.log_file_path = log_dir + "get_oildata_log_" + time.strftime("%Y%m%d", time.localtime()) + ".txt"   
        self.log_file_handle = open(self.log_file_path, 'a')
        self.g_db_conn = False
        self.g_db_conn = self.open_db()
        
    def __del__(self):
        print("del self")
        self.close_db()
        if self.log_file_handle:
            self.log_file_handle.close()
        
    def open_db(self):
        ''' open data base 保存数据库'''
        global DB_FILE_PATH 
        if not os.access(DB_FILE_PATH, os.F_OK):
            #print ("%s not exist\n" % DB_FILE_PATH)
            os._exit(0)
        self.g_db_conn = sqlite3.connect(DB_FILE_PATH, detect_types=sqlite3.PARSE_COLNAMES)
        if self.g_db_conn is not None:
            print ("opened {0} successful!".format(DB_FILE_PATH))
            pass
        else:
            #print ("opened database fail!")
            os._exit(0)
        self.g_db_conn.text_factory = lambda x : str(x, 'utf-8', 'ignore')
        return self.g_db_conn
        
    def close_db(self): 
        print("db commit")
        self.g_db_conn.commit()
        self.g_db_conn.close() 
    
    def getTargetKDataDir(self, code):
        target_path = self.data_dir + "/" + code + "/kline"
        #print("saveCodeTick2File : %s %s" %(code, target_path) )
        if not os.path.isdir(target_path):
            os.makedirs(target_path)
        return target_path
        
    def write_log(self, content):
        if self.log_file_handle:
            self.log_file_handle.write(content + "\n")
            self.log_file_handle.flush()
            
    def is_ascii(self, s):
        return all(ord(c) < 128 for c in s)
        
    def get_stk_baseinfo(self):
        count = 0
        self.cur = self.g_db_conn.cursor()
        for row in self.cur:
            count = row[0]
            break
        stock_info = ts.get_stock_basics() 
        self.cur.execute("SELECT count(code) FROM stock")  
        type = 0; # 0--stock 1--index 
        num = 0
        for i in stock_info.index:
            py_str = ''.join(self.getpinyinhead(stock_info.ix[i]['name'])) 
            sql = "INSERT OR REPLACE INTO stock VALUES(?, ?, ?, ?, ?, ?, ?, '')"
            self.cur.execute(sql, (i, type, stock_info.ix[i]['name'], py_str, str(stock_info.ix[i]['timeToMarket']), stock_info.ix[i]['industry'], stock_info.ix[i]['area'])) 
            num += 1 
        type = 1; # 0--stock 1--index 
        index_array = [('999999', '上证指数', 'SZZS') 
                       ]
        for i in range(0, len(index_array)):
            sql = "INSERT OR REPLACE INTO stock VALUES(?, ?, ?, ?, 0, '', '', '')"
            self.cur.execute(sql, (index_array[i][0], type, index_array[i][1], index_array[i][2]))
            print("insert {} {} {} {}".format(index_array[i][0], type, index_array[i][1], index_array[i][2]))
            num += 1
            
        self.g_db_conn.commit()
        print("has insert or replace {} records".format(num))
        return "ok"
        
    def proc_file(self, file_full_path):
        self.open_db()
        self.cur = self.g_db_conn.cursor()
        sql = '''INSERT OR REPLACE INTO SCL9_1M (longdate, time, open, close, high, low, vol) values (?,?,?,?,?,?,?)'''
        reobj = re.compile("^(\\d{4})-(\\d{1,2})-(\\d{1,2}),(\\d{2}):(\\d{2}),(\\d+\\.\\d+|\\d+),(\\d+\\.\\d+|\\d+),(\\d+\\.\\d+|\\d+),(\\d+\\.\\d+|\\d+),(\\d+),(\\d+)(.*)$")
        re.compile(reobj)
        fh = open(file_full_path)
        is_firt_line = True
        is_last_line = False
        pre_hhmm = 0
        line_count = 0
        pre_record = (0, 0, 0, 0, 0, 0, 0)
        for line in fh:
            try:
                #print(line)
                line_count += 1
                match1 = re.search(reobj, line)
                year = match1.group(1)
                month = match1.group(2)
                day = match1.group(3)
                longdate = int(year)*10000 + int(month)*100 + int(day);
                hour = match1.group(4)
                minute = match1.group(5)
                hhmm = int(hour)*100 + int(minute)
                price_o = match1.group(6)
                price_h = match1.group(7)
                price_l = match1.group(8)
                price_c = match1.group(9)
                vol = match1.group(10)
                hold = match1.group(11)
                if is_firt_line:
                    is_firt_line = False
                    #pre_hhmm = hour*100 + minute
                    next_hhmm = GetNextHHMM(int(year), int(month), int(day), int(hour), int(minute))
                    self.cur.execute(sql, (longdate, hhmm, price_o, price_c, price_h, price_l, vol))
                    pre_record = (longdate, hhmm, price_o, price_c, price_h, price_l, 0)
                else:
                    if next_hhmm != int(hour)*100 + int(minute):
                        print("lack %s" % (next_hhmm))
                        #break
                    next_hhmm = GetNextHHMM(int(year), int(month), int(day), int(hour), int(minute))
                    self.cur.execute(sql, (longdate, hhmm, price_o, price_c, price_h, price_l, vol))
                    pre_record = (longdate, hhmm, price_o, price_c, price_h, price_l, 0)
                #print("next hhmm %s" % next_hhmm)     
            except Exception as e:
                print(e)
                break
        self.g_db_conn.commit()
        print("out proc")   
        
def GetNextHHMM(year, month, day, hour, minute):
    d = datetime(year,month,day, hour,minute)
    #time_val = year*10000 + month*100 + day
    hm = hour*100 + minute
    next_hm = 0
    if hm < 229:
        e = d + timedelta(minutes=1)
        next_hm = e.hour * 100 + e.minute
    elif hm == 230:
        next_hm = 901
    elif hm < 1015:
        e = d + timedelta(minutes=1)
        next_hm = e.hour * 100 + e.minute
    elif hm == 1015:
        next_hm = 1031
    elif hm < 1130:
        e = d + timedelta(minutes=1)
        next_hm = e.hour * 100 + e.minute
    elif hm == 1130:
        next_hm = 1331
    elif hm < 1500:
        e = d + timedelta(minutes=1)
        next_hm = e.hour * 100 + e.minute
    elif hm == 1500:
        next_hm = 2101
    elif hm < 2359:
        e = d + timedelta(minutes=1)
        next_hm = e.hour * 100 + e.minute
    elif hm == 2359:
        next_hm = 0
    return next_hm
    

    
if __name__ == "__main__":  
    print("main")
    if "PYTHONPATH" in os.environ:
        mystr = os.environ["PYTHONPATH"] 
        print(mystr)
    #obj = STOCKBASIC()
    #if 1:
    #    obj.get_stk_baseinfo()
    #time0 = timedelta(days=0, hours=11, minutes=1)
    #year = 2018
    #month = 2
    #day = 3
    #hour = 2
    #minute = 59
    #next_hm = GetNextHHMM(year, month, day, hour, minute)
    #print(next_hm)
    obj = FUTUREBASIC() 
    obj.proc_file(DATA_FILE_0)
        
        