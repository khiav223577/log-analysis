


#ifndef ___RDate_cpp__
#define ___RDate_cpp__
#include <stdio.h>
class RDate{
public:
    int year, month, day, hour, minute, second;
    RDate(){
        year = 1970, month = 1, day = 1, hour = 0, minute = 0, second = 0;
    }
    RDate(int _second) : month(1), second(_second){
        day    = second / 86400; second -= 86400 * day;
        day   += get_base1970();
        hour   = second /  3600; second -=  3600 * hour;
        minute = second /    60; second -=    60 * minute;
        year   =    day /   366; day    -=   365 * year + getLeapDays(year);
        while(1){
            int year_day = is_leapyear(year) ? 366 : 365;
            if (day < year_day) break;
            day -= year_day;
            year += 1;
        }
        int *month_cumative = get_month_cumative(year) + 1;
        for(month = 1; month <= 12; ++month, ++month_cumative){
            if (day >= *month_cumative) continue;
            day = day - *(--month_cumative) + 1;
            break;
        }
    }
public:
    inline static bool is_leapyear(int year){
        if (year % 4000 == 0) return false;
        if (year %  400 == 0) return true;
        if (year %  100 == 0) return false;
        if (year %    4 == 0) return true;
        return false;
    }
    inline static int getLeapDays(int year){
        year -= 1;
        return ((year / 4) - (year / 100) + (year / 400) - (year / 4000));
    }
    inline static int get_base1970(){
        static int base = 1970 * 365 + getLeapDays(1970);
        return base;
    }
    inline static int *get_month_cumative(bool leap_year = false){
        static int month_cumative1[] = {0,31,60,91,121,152,182,213,244,274,305,335,366};
        static int month_cumative2[] = {0,31,59,90,120,151,181,212,243,273,304,334,365};
        return (leap_year ? month_cumative1 : month_cumative2);
    }
    inline static int *get_month_cumative(int year){ return get_month_cumative(is_leapyear(year)); }
    inline static int toDay(int year, int month, int day){
        return year * 365 + getLeapDays(year) - get_base1970() + get_month_cumative(year)[month - 1] + (day - 1);
    }
    inline static int toSecond(int year, int month, int day, int hour, int minute, int second){
        return toDay(year, month, day) * 86400 + hour * 3600 + minute * 60 + second;
    }
//-----------------------------------------
    inline static void show(int second){RDate d(second); d.show(); }
    inline int toDay(){    return toDay(year, month, day); }
    inline int toSecond(){ return toSecond(year, month, day, hour, minute, second); }
    inline void getShow(char *str){ sprintf(str,"%d/%02d/%02d %02d:%02d:%02d ",year,month,day,hour,minute,second);}
    inline void show(){              printf(    "%d/%02d/%02d %02d:%02d:%02d ",year,month,day,hour,minute,second);}
//-----------------------------------------



};






#endif
