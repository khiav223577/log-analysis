
//--------------------------------------------
//  執行方法
//--------------------------------------------
  ●(windows) 
    AnalyzeLog.exe [CONFIG] [INPUT] [ACTION]
  ●(linux 32bit) 
    LD_LIBRARY_PATH=./lib/shared_library/i686-linux ./AnalyzeLog [CONFIG] [INPUT] [ACTION]
  ●(linux 64bit) 
    LD_LIBRARY_PATH=./lib/shared_library/x86_64-linux ./AnalyzeLog_x86_64_linux [CONFIG] [INPUT] [ACTION]
     
    ※ [CONFIG] 是config檔所在的路徑
    ※ [INPUT]  是input檔所在的路徑
    ※ [ACTION] 可以是compress或query
    
  例如: 
  AnalyzeLog.exe data/config data/input_large.bz2 compress
  AnalyzeLog.exe data/config data/input_large.bz2 query
  (註：程式會去抓[INPUT]的副檔名，若是bz2的話會視為bz2壓縮檔，否則視為純文字檔。直接從純文字檔讀會比較快)
  
//--------------------------------------------
//  query指令
//--------------------------------------------
  輸入exit：結束程式。
  輸入help：查看幫助。
  輸入：[Source IP],[Destination IP],[Start time],[End time],[time span]
    ※ 注意，以逗號分隔。
    ※ [Source IP]      是來源IP，格式XXX.XXX.XXX.XXX，XXX是數字、也可以是星號。
    ※ [Destination IP] 是目標IP，格式同上。例如：140.112.*.*。(註：最多二個星號)
    ※ [Start time]     起始時間，格式 year/month/day hour:minute:second
    ※ [End time]       結束時間，格式同上。例如：2014/1/09 04:00:00。
    ※ [time span]      時間區間，從起始時間到結束時間為止，計算每個時間區間內的流量。
  
  例如： 
  140.109.20.250, 140.109.1.10, 2014/1/09 04:00:00, 2014/1/09 05:00:00, 60
  140.109.21.*,140.109.*.*, 2013/12/03 04:00:00, 2013/12/03 04:02:00, 10
  (註：時間的格式支援很多種寫法，詳情參考： http://www.ruby-doc.org/stdlib-1.9.3/libdoc/date/rdoc/DateTime.html#method-c-parse ) 

//--------------------------------------------
//  產生的檔案
//--------------------------------------------
  1st pass:
    [INPUT].config1          #記錄壓縮資訊的config檔
    [INPUT].temp1.partXXX    #壓縮後的檔案(XXX = 0,1,2,3,...)
    [INPUT].part.info1       #記錄每個block各別的壓縮資訊
  2nd pass:
    [INPUT].config2          #記錄壓縮資訊的config檔
    [INPUT].partXXX          #壓縮後的檔案(XXX = 0,1,2,3,...)
    [INPUT].part.info2       #記錄每個block各別的壓縮資訊
    [INPUT].index            #索引檔，query時會用到。
  query：
    output.html              #流量圖表

  
  
  
  
