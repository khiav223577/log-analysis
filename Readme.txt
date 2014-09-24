執行方法：
  AnalyzeLog.exe [CONFIG] [INPUT] [ACTION]
    ※ [CONFIG] 是config檔所在的路徑
    ※ [INPUT]  是input檔所在的路徑
    ※ [ACTION] 可以是compress或query
    
  例如: AnalyzeLog.exe data/config data/input_large.bz2 compress
  (註：程式會去抓[INPUT]的副檔名，若是bz2的話會視為bz2壓縮檔，否則視為純文字檔。)
  

壓縮後會產生：(XXX是數字)
  [INPUT].config1          #1st pass產生的config，可刪除
  [INPUT].config2          #2nd pass產生的config，不可刪除，否則無法query
  [INPUT].index            #儲存的index檔，不可刪除，否則無法query
  [INPUT].temp1.partXXX    #1st pass產生的資料，可刪除
  [INPUT].partXXX          #2st pass產生的資料，不可刪除，否則無法query
  
  
query方式：
  輸入exit：結束程式。
  輸入help：查看幫助。
  輸入：[Source IP] [Destination IP] [Start time] [End time] [time span]
    ※ [Source IP]      是來源IP，格式XXX.XXX.XXX.XXX，XXX是數字、也可以是星號。
    ※ [Destination IP] 是目標IP，格式同上。例如：140.112.*.*。(註：最多二個星號)
    ※ [Start time]     起始時間，格式 year/month/day hour:minute:second
    ※ [End time]       結束時間，格式同上。例如：2014/1/09 04:00:00。
    ※ [time span]      時間區間，從起始時間到結束時間為止，計算每個時間區間內的流量。
  
  例如： 140.109.20.250, 140.109.1.10, 2014/1/09 04:00:00, 2014/1/09 05:00:00, 60
  (註：格式支援很多種寫法，詳情參考： http://www.ruby-doc.org/stdlib-1.9.3/libdoc/date/rdoc/DateTime.html#method-c-parse ) 

 
query後會產生：
  output.html              #流量圖表

  
  
  
  
