
#ifndef ___FormatterInteger_cpp__
#define ___FormatterInteger_cpp__
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
class FormatterInteger : public FormatterController{
public:
    typedef FormatterController super;
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
	public:
    //-------------------------------------------------------------------------
    //  transform config-format to appropriate format. (for speed up)
    //-------------------------------------------------------------------------
        char *trans_format(const char *_format){
            char *format = (char *) malloc(5 * sizeof(char));
            format[0] = '%';
            format[1] = '?';
            format[2] = '%';
            format[3] = 'n';
            format[4] = '\0';
            switch(atoi(_format)){
            case  8:{ format[1] = 'o'; break;} //"%o%n"
            case 10:{ format[1] = 'd'; break;} //"%d%n"
            case 16:{ format[1] = 'x'; break;} //"%x%n"
            default:{ printf("Unknown Int format: %s (must be 8 or 10 or 16)",_format); exit(1);}
            }
            return format; //Ex: format = "%d%n"
        }
	};
    FormatterInteger(const char *_format) : super(_format, new VirtualCreator()){

    }
public:
    bool SuccessFlag;
//--------------------------------------
//  execute
//--------------------------------------
    inline int execute(const char ** inputStream){
        int num = FormatterInteger::retrieve(inputStream, format);
        if (!SuccessFlag){ //TODO overflow
            puts("overflow");
            exit(1);
        }
        SetColor2(); printf("[%d] ",num); SetColor(7);//DEBUG
        return 0;
    }
//-------------------------------------------------------------------------
//  retrieve data from input according the format.
//-------------------------------------------------------------------------
    int retrieve(const char **input, const char *format){   //format = "%d%n","%x%n","%o%n"
        SuccessFlag = false;
        const char *inputPtr = *input;
        int scanfLen = 0, result;
        sscanf(inputPtr, "%*[ \f\n\r\t\v]%n", &scanfLen);   //remove white-space characters
        inputPtr += scanfLen;
        sscanf(inputPtr, format, &result, &scanfLen);       //read integer.
        if (scanfLen == 0) return -1;
        if (*inputPtr == '-'){      //check overflow by (+-) sign
            if (result > 0) return -1;
        }else{
            if (result < 0) return -1;
        }
        if (*inputPtr == '-' || *inputPtr == '+'){          //remove (+-) sign
            inputPtr += 1;
            scanfLen -= 1;
        }
        while(*inputPtr == '0'){    //remove redundant zero
            inputPtr += 1;
            scanfLen -= 1;
        }
        char n_decimal = format[1];
        switch(n_decimal){          //check overflow by length
        case 'o':{ if (scanfLen <= 10) SuccessFlag = true; break;}
        case 'd':{ if (scanfLen <=  9) SuccessFlag = true; break;}
        case 'x':{ if (scanfLen <=  7) SuccessFlag = true; break;}
        }
        if (SuccessFlag == false){
            static char format2[3] = "%d";                      //compare input's and result's number by "strcmp"
            format2[1] = n_decimal;
            char *buffer = (char *) malloc(20 * sizeof(char));  //20 is enough for max number length in that we have checked overflow by length.
            sprintf(buffer, format2, (result < 0 ? -result : result));
            for(int i = 0; i < scanfLen; ++i){ if (inputPtr[i] != buffer[i]) return -1;}
            free(buffer);
        }
        *input = inputPtr + scanfLen;
        SuccessFlag = true;
        return result;
    }
};

#endif
