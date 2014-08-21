
#ifndef ___FormatterInteger_cpp__
#define ___FormatterInteger_cpp__
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "FlexibleInt.cpp"
#include "SafeScanf.cpp"

class FormatterInteger : public FormatterController{
public:
    typedef FormatterController super;
//-------------------------------------------------------------------------
//  transform config-format to appropriate format. (for speed up)
//-------------------------------------------------------------------------
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
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
            default:{ PERROR(true, printf("Unknown Int format: %s (must be 8 or 10 or 16)",_format);); }
            }
            return format; //Ex: format = "%d%n"
        }
	};
	FlexibleInt prev_int;
    FormatterInteger(const char *_format) : super(_format, new VirtualCreator()){
        BigIntFlagAt = -1;
        execute1Counter = 0;
    }
    int get_prev_int(){ //Will be called by FormatterIFStatement.
        PERROR(!initialized, printf("Error: fails to get_prev_int() in FormatterInteger."););
        return prev_int.getValue(); // TODO BigInt.
    }
public:
    virtual void save_config1(FILE *file);
    virtual void load_config1(FILE *file);
//--------------------------------------
//  execute
//--------------------------------------
private:
    int execute1Counter, BigIntFlagAt;
    bool SuccessFlag;
    FlexibleInt record_min, record_max;
public:
    int execute1(OutputManager *outputer, const char **inputStream){
        if (BigIntFlagAt == -1){
            int value = retrieve(inputStream, format);
            if (SuccessFlag) prev_int = FlexibleInt(value);
            else BigIntFlagAt = execute1Counter;
        }
        if (BigIntFlagAt != -1) prev_int = FlexibleInt(retrieveBInt(inputStream, format));
        SetColor2(); prev_int.output(); SetColor(7);//DEBUG
        if (initialized){
            if (prev_int < record_min) record_min = prev_int;
            if (prev_int > record_max) record_max = prev_int;
        }else{
            initialized = true;
            record_min = prev_int;
            record_max = prev_int;
        }
        outputer->write(prev_int);
        execute1Counter += 1;
        return 0;
    }
    int execute2(InputManager *inputer){
        bool is_BigInt = (BigIntFlagAt != -1 && execute1Counter >= BigIntFlagAt);
        prev_int = (is_BigInt ? FlexibleInt(inputer->read_bigInt()) : FlexibleInt(inputer->read_int()));
        SetColor2(); prev_int.output(); SetColor(7);//DEBUG
        execute1Counter += 1;
        return 0;
    }
//-------------------------------------------------------------------------
//  retrieve data from input according the format.
//-------------------------------------------------------------------------
    inline int retrieve(const char **input, const char *format){   //format = "%d%n" or "%x%n" or "%o%n"
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
            static char format2[3] = "%?";                      //compare input's and result's number by "strcmp"
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
    _DEF_SafeScanf(readBigInt08, 255, "[0-7]");
    _DEF_SafeScanf(readBigInt10, 255, "[0-9]");
    _DEF_SafeScanf(readBigInt16, 255, "[0-9a-fA-F]");
    inline BigInteger *retrieveBInt(const char **input, const char *format){   //format = "%d%n" or "%x%n" or "%o%n"
        std::string bigIntString;
        int n_digit = -1;
        switch(format[1]){
        case 'o':{ bigIntString = readBigInt08(*input); n_digit =  8; break; }
        case 'd':{ bigIntString = readBigInt10(*input); n_digit = 10; break; }
        case 'x':{ bigIntString = readBigInt16(*input); n_digit = 16; break; }
        }
        int size =  bigIntString.size();
        PERROR(size <= 0, printf("read BigInt fail, format = %s, input = %s", format, *input););
        *input += size;
        return new BigInteger(BigUnsignedInABase(bigIntString, n_digit));
    }
};

#endif
