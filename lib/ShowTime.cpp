/*
<REQUIRE>
    <1>

<USAGE>
    <1> show the time
        inline void show(const char *str1, const char *str2);
        <EX>
            ShowTime showtime;
            showtime.show("str1","str2");

*/
#ifndef ___ShowTime_cpp__
#define ___ShowTime_cpp__


#include <stdio.h>
#ifndef _WIN32
    unsigned int GetTickCount() {
        timespec tp;
        clock_gettime(CLOCK_MONOTONIC,&tp);
        return (unsigned int)tp.tv_sec*1000+(unsigned int)(tp.tv_nsec/1000000);
    }
#else
    #include<windows.h>
#endif
class ShowTime{
private:
    int Start_time,Prev_time;
public:
    inline ShowTime(){
        Start_time = GetTickCount();
        Prev_time = Start_time;
    }
    inline void show(const char *str1, const char *str2){
        int tmp = GetTickCount();
        printf("%-30s%5d/%5dms%s\n",str1,tmp - Prev_time,tmp - Start_time,str2);
        Prev_time = tmp;
    }
};



#endif
