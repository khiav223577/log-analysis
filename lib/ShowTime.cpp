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
        reset();
    }
    inline void show(const char *str1, const char *str2){
        int tmp = GetTickCount();
        printf("%-30s%5d/%5dms%s\n",str1,tmp - Prev_time,tmp - Start_time,str2);
        Prev_time = tmp;
    }
    inline void reset(){
        Prev_time = Start_time = GetTickCount();
    }
};
class EvaluateTime{
private:
    int Start_time, Count_time;
public:
    inline EvaluateTime(){
        reset();
    }
    inline void start(){
        Start_time = GetTickCount();
    }
    inline void stop(){
        Count_time += GetTickCount() - Start_time;
    }
    inline void show(const char *str1){
        printf("%-10s: %5dms\n", str1, Count_time);
    }
    inline void reset(){
        Count_time = 0;
    }
};


#endif
