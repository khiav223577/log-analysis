
#ifndef ___FormatterInteger_cpp__
#define ___FormatterInteger_cpp__
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
class FormatterInteger{
public:
    static bool SuccessFlag;
//-------------------------------------------------------------------------
//  transform config-format to appropriate format. (for speed up)
//-------------------------------------------------------------------------
    static char *trans_format(const char *_format){
        char *format = (char *) malloc(5 * sizeof(char));
        format[0] = '%';
        format[1] = '?';
        format[2] = '%';
        format[3] = 'n';
        format[4] = '\0';
        if (_format[0] == '8' && _format[1] == '\0'                     ) format[1] = 'o';  //"%o%n"
        if (_format[0] == '1' && _format[1] == '0' && _format[2] == '\0') format[1] = 'd';  //"%d%n"
        if (_format[0] == '1' && _format[1] == '6' && _format[2] == '\0') format[1] = 'x';  //"%x%n"
        return format; //Ex: format = "%d%n"
    }
//-------------------------------------------------------------------------
//  retrieve data from input according the format.
//-------------------------------------------------------------------------
    static int retrieve(const char **input, const char *format){ //format = "%d%n","%x%n","%o%n"
        SuccessFlag = false;
        const char *inputPtr = *input;
        int scanfLen, result;
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
            static char format2[3] = "%d";                  //compare input's and result's number by "strcmp"
            format2[1] = n_decimal;
            char *buffer = (char *) malloc(20 * sizeof(char));
            sprintf(buffer, format2, (result < 0 ? -result : result));
            for(int i = 0; i < scanfLen; ++i){ if (inputPtr[i] != buffer[i]) return -1;}
            free(buffer);
        }
        *input = inputPtr + scanfLen;
        SuccessFlag = true;
        return result;
        /*
        int scanfLen = 0;
        char *buffer = (char *) malloc((MAXLEN + 1) * sizeof(char));
        sscanf(*input, format, buffer, &scanfLen);
        *input += scanfLen;
        buffer[scanfLen] = '\0'; //avoid error when scanfLen == 0
        return buffer;*/
    }
};

#endif
