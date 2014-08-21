
#ifndef ___FormatterDate_cpp__
#define ___FormatterDate_cpp__
#include<ctype.h>
#include<string.h>
#include<stdlib.h>
#include "RDate.cpp"

#define DEBUG_SHOW
class FormatterDate : public FormatterController{
public:
    typedef FormatterController super;
//-------------------------------------------------------------------------
//  transform config-format to appropriate format. (for speed up)
//-------------------------------------------------------------------------
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
        char *trans_format(const char *_format){
            char *format = (char *) malloc((strlen(_format) + 1) * sizeof(char));
            strcpy(format, _format);
            return format;
        }
	};
    FormatterDate(const char *_format) : super(_format, new VirtualCreator()){

    }
public:
    virtual void save_config1(FILE *file);
    virtual void load_config1(FILE *file);
//--------------------------------------
//  execute
//--------------------------------------
    int execute1(OutputManager *outputer, const char **inputStream){
        int date = retrieve(inputStream, format);
        outputer->write(date);
        debug(date);
        return 0;
    }
    int execute2(InputManager *inputer){
        int date = inputer->read_int();
        debug(date);
        return 0;
    }
    inline void debug(int date){
        #ifdef DEBUG
            RDate::show(date);
        #endif
    }
//-------------------------------------------------------------------------
//  retrieve data from input according the format.
//-------------------------------------------------------------------------
    inline int retrieve(const char **input, const char *format){
        const char *inputPtr = *input;
        RDate date;
        int scanfLen,counter = 0; //longest common char
        char prev = *format, curr = *format;
        while(prev != '\0'){
            if (curr != prev || counter > 9){
                scanfLen = 0;
                switch(prev){
                case 'y':{
                    switch(counter){
                    case 2:{sscanf(inputPtr, "%02d%n",     &date.year,    &scanfLen); break;} //  year: 99(1999),00(2000),01(2001),...
                    case 4:{sscanf(inputPtr, "%4d%n",      &date.year,    &scanfLen); break;} //  year: 1970,1971,...,2013,2014
                    }
                    if (date.year < 70) date.year += 2000;
                    else if (date.year <= 99) date.year += 1900;
                    break;}
                case 'M':{
                    char tmp[10];
                    switch(counter){
                    case 1:{sscanf(inputPtr, "%d%n",       &date.month,   &scanfLen); break;} // month: 1,2,...,11,12
                    case 2:{sscanf(inputPtr, "%02d%n",     &date.month,   &scanfLen); break;} // month: 01,02,...,11,12
                    case 3:{sscanf(inputPtr, "%3s%n",      tmp,           &scanfLen); break;} // month: Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec
                    case 4:{sscanf(inputPtr, "%8s%n",      tmp,           &scanfLen); break;} // month: January,February,...,November,December
                    }
                    if (counter == 3 || counter == 4){
                        date.month = 0;
                        tmp[0] = tolower(tmp[0]);
                        tmp[1] = tolower(tmp[1]);
                        tmp[2] = tolower(tmp[2]);
                        switch(*tmp){
                        case 'j':{ //Jan,Jun,Jul
                            if (tmp[1] == 'a' && tmp[2] == 'n') date.month = 1;
                            if (tmp[1] == 'u' && tmp[2] == 'n') date.month = 6;
                            if (tmp[1] == 'u' && tmp[2] == 'l') date.month = 7;
                            break;}
                        case 'f':{ //Feb
                            if (tmp[1] == 'e' && tmp[2] == 'b') date.month = 2;
                            break;}
                        case 'm':{ //Mar,May
                            if (tmp[1] == 'a' && tmp[2] == 'r') date.month = 3;
                            if (tmp[1] == 'a' && tmp[2] == 'y') date.month = 5;
                            break;}
                        case 'a':{ //Apr,Aug
                            if (tmp[1] == 'p' && tmp[2] == 'r') date.month = 4;
                            if (tmp[1] == 'u' && tmp[2] == 'g') date.month = 8;
                            break;}
                        case 's':{ //Sep
                            if (tmp[1] == 'e' && tmp[2] == 'p') date.month = 9;
                            break;}
                        case 'o':{ //Oct
                            if (tmp[1] == 'c' && tmp[2] == 't') date.month = 10;
                            break;}
                        case 'n':{ //Nov
                            if (tmp[1] == 'o' && tmp[2] == 'v') date.month = 11;
                            break;}
                        case 'd':{ //Dec
                            if (tmp[1] == 'e' && tmp[2] == 'c') date.month = 12;
                            break;}
                        }
                        if (date.month == 0) goto LABEL_ERROR;
                    }
                    break;}
                case 'd':{
                    switch(counter){
                    case 1:{sscanf(inputPtr, "%d%n",       &date.day,     &scanfLen); break;} //   day: 0,1,2,...,30,31
                    case 2:{sscanf(inputPtr, "%02d%n",     &date.day,     &scanfLen); break;} //   day: 00,01,02,...,30,31
                    case 3:{sscanf(inputPtr, "%*3s%n",     &scanfLen);                break;} //  week: Mon,Tue,Wed,Thu,Fri,Sat,Sun
                    case 4:{sscanf(inputPtr, "%*8[^y]y%n", &scanfLen);                break;} //  week: Monday,Tuesday,...,Saturday,Sunday
                    }
                    break;}
                case 'h': case 'H':{
                    switch(counter){
                    case 1:{sscanf(inputPtr, "%d%n",       &date.hour,    &scanfLen); break;} //  hour: 0,1,2,...,22,23
                    case 2:{sscanf(inputPtr, "%02d%n",     &date.hour,    &scanfLen); break;} //  hour: 00,01,02,...,22,23
                    }
                    break;}
                case 't':{
                    char tmp[3];
                    switch(counter){
                    case 1:{sscanf(inputPtr, "%1s%n",      tmp,           &scanfLen); break;} //P,A
                    case 2:{sscanf(inputPtr, "%2s%n",      tmp,           &scanfLen); break;} //PM,AM
                    }
                    if ((tmp[0] != 'P' && tmp[0] != 'A') || (tmp[1] != 'M' && tmp[1] != '\0')) goto LABEL_ERROR;
                    if (tmp[0] == 'P') date.hour += 12;
                    break;}
                case 'm':{
                    switch(counter){
                    case 1:{sscanf(inputPtr, "%d%n",       &date.minute,  &scanfLen); break;} //minute: 0,1,2,...,58,59
                    case 2:{sscanf(inputPtr, "%02d%n",     &date.minute,  &scanfLen); break;} //minute: 00,01,02,...,58,59
                    }
                    break;}
                case 's':{
                    switch(counter){
                    case 1:{sscanf(inputPtr, "%d%n",       &date.second,  &scanfLen); break;} //second: 0,1,2,...,58,59
                    case 2:{sscanf(inputPtr, "%02d%n",     &date.second,  &scanfLen); break;} //second: 00,01,02,...,58,59
                    }
                    break;}
                default:{
                    static char tmp[] = "%*9[ ]%n";
                    tmp[2] = '0' + counter;
                    tmp[4] = prev;
                    sscanf(inputPtr, tmp, &scanfLen);
                    if (scanfLen != counter){ printf("tmp = %s,scanfLen = %d\n",tmp,scanfLen); goto LABEL_ERROR;}
                    break;}
                }
                if (scanfLen == 0){
                    LABEL_ERROR:
                    PERROR(true, printf("Unknown format in readDate:\nformat=%s, prev=[%%c:%c, %%d:%d], counter=%d\ninput=\"%s\"\n",format, prev, prev, counter,inputPtr););
                }
                inputPtr += scanfLen;
                counter = 1;
            }else counter += 1;
            prev = curr;
            curr = *(++format);
        }
        #ifdef DEBUG_SHOW
            RDate test = RDate(date.toSecond());
            char tmp1[999],tmp2[999];
            date.getShow(tmp1);
            test.getShow(tmp2);
            if (strcmp(tmp1,tmp2) != 0){// || strcmp(tmp2,*input) != 0
                printf("%s\n",*input);
                printf("%s\n",tmp1);
                printf("%s\n",tmp2);
            }else{
                //printf("%s\n",*input);
                //date.show();
                //test.show();
            }
        #endif
        *input = inputPtr;
        return date.toSecond();
    }
};






#endif
