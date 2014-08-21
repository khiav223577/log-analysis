
#ifndef ___FormatterInteger_cpp__
#define ___FormatterInteger_cpp__
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "lib/FlexibleInt.cpp"
#include "lib/SafeScanf.cpp"

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
        BigIntFlagAt1 = -1;
        BigIntFlagAt2 = -1;
        Size4FlagAt1 = -1;
        Size4FlagAt2 = -1;
        executeCounter = 0;
        byte_num = 1;
        increasingFuncFlag = true;
    }
    int get_prev_int(){ //Will be called by FormatterIFStatement.
        PERROR(!initialized, printf("Error: fails to get_prev_int() in FormatterInteger."););
        PERROR(prev_int.isBigInt(), printf("Unable to compare with BigInteger in IF-Statement."););
        return prev_int.getValue(); // TODO BigInt.
    }
public:
    virtual void save_config1(FILE *file);
    virtual void save_config2(FILE *file);
    virtual void load_config1(FILE *file);
    virtual void load_config2(FILE *file);
//--------------------------------------
//  execute
//--------------------------------------
private:
    unsigned char byte_num;
    int executeCounter, BigIntFlagAt1, BigIntFlagAt2, Size4FlagAt1, Size4FlagAt2;
    bool SuccessFlag, SameFlag, increasingFuncFlag;
    FlexibleInt record_min, record_max, record_range;
    DeltaEncoding<FlexibleInt> delta_encoding;
public:
    int execute1(const char **inputStream){
        FlexibleInt prev_int_sav = prev_int;
        if (BigIntFlagAt1 == -1){
            int value = retrieve(inputStream, format);
            if (SuccessFlag) prev_int = FlexibleInt(value);
            else BigIntFlagAt1 = executeCounter;
        }
        if (BigIntFlagAt1 != -1) prev_int = FlexibleInt(retrieveBInt(inputStream, format));
        if (initialized){
            if (BigIntFlagAt1 == -1){ //just for speed up. (reduce exec-time from 0.8s to 0.08s for 100k lines of data)
                if (prev_int.getValue() < record_min.getValue())  record_min.setValue(prev_int.getValue());
                if (prev_int.getValue() > record_max.getValue())  record_max.setValue(prev_int.getValue());
            }else{
                if (prev_int < record_min) record_min = prev_int;
                if (prev_int > record_max) record_max = prev_int;
            }
            if (increasingFuncFlag && (prev_int - prev_int_sav) < 0) increasingFuncFlag = false;

        }else{
            initialized = true;
            record_min = prev_int;
            record_max = prev_int;
        }
        if (prev_int.isBigInt()){
            outputer->write(prev_int.getValuePtr());
        }else{
            if (Size4FlagAt1 == -1 && (prev_int > 127 || prev_int < -128)) Size4FlagAt1 = executeCounter;
            outputer->write(prev_int.getValue(), (Size4FlagAt1 == -1 ? 1 : 4));
        }

        executeCounter += 1;
        debug();
        return 0;
    }
    int execute2(){
        bool isBigInt = (BigIntFlagAt1 != -1 && executeCounter >= BigIntFlagAt1);
        if (isBigInt){
            prev_int = FlexibleInt(inputer->read_bigInt());
        }else{
            if (Size4FlagAt1 != -1 && executeCounter >= Size4FlagAt1) byte_num = 4;
            prev_int = FlexibleInt(inputer->read_n_byte_int(byte_num));
        }
        if (SameFlag){
            //do nothing
        }else if (increasingFuncFlag == true){
            FlexibleInt output = delta_encoding.encode(prev_int); //delta encoding
            if (BigIntFlagAt2 == -1){
                output.try_to_cast_to_int();
                if (output.isBigInt()) BigIntFlagAt2 = executeCounter;
            }
            if (output.isBigInt()){
                outputer->write(output.getValuePtr());
            }else{
                if (Size4FlagAt2 == -1 && output > 255) Size4FlagAt2 = executeCounter;
                outputer->write(output.getValue(), (Size4FlagAt2 == -1 ? 1 : 4));
            }
        }else if (record_range.isBigInt() == false){
            FlexibleInt output = (prev_int - record_min);
            bool success = output.try_to_cast_to_int();
            PERROR(success == false, printf("Unable to cast BigInt to int");); //should always be able to.
            int value = output.getValue();
            if (Size4FlagAt2 == -1 && value > 255) Size4FlagAt2 = executeCounter;
            outputer->write(value, (Size4FlagAt2 == -1 ? 1 : 4));
        }else{
            if (prev_int.isBigInt()){
                outputer->write(prev_int.getValuePtr());
            }else{
                outputer->write(prev_int.getValue(), byte_num);
            }
        }
        executeCounter += 1;
        debug();
        return 0;
    }
    int execute3(){
        if (SameFlag){
            prev_int = record_min;
        }else if (increasingFuncFlag == true){
            bool isBigInt = (BigIntFlagAt2 != -1 && executeCounter >= BigIntFlagAt2);
            FlexibleInt delta;
            if (isBigInt){
                delta = FlexibleInt(inputer->read_bigInt());
            }else{
                if (Size4FlagAt2 != -1 && executeCounter >= Size4FlagAt2) byte_num = 4;
                delta = FlexibleInt(inputer->read_n_byte_int(byte_num));
            }
            prev_int = delta_encoding.decode(delta); //delta encoding
        }else if (record_range.isBigInt() == false){
            if (Size4FlagAt2 != -1 && executeCounter >= Size4FlagAt2) byte_num = 4;
            prev_int = record_min + FlexibleInt(inputer->read_n_byte_int(byte_num));
        }else{
            bool isBigInt = (BigIntFlagAt1 != -1 && executeCounter >= BigIntFlagAt1);
            if (isBigInt){
                prev_int = FlexibleInt(inputer->read_bigInt());
            }else{
                if (Size4FlagAt1 != -1 && executeCounter >= Size4FlagAt1) byte_num = 4;
                prev_int = FlexibleInt(inputer->read_n_byte_int(byte_num));
            }
        }
        executeCounter += 1;
        debug();
        return 0;
    }
    inline void debug(){
        #ifdef DEBUG
            SetColor2(); prev_int.output(); SetColor(7);
        #endif
    }
//-------------------------------------------------------------------------
//  retrieve data from input according the format.
//-------------------------------------------------------------------------
    inline int retrieve(const char **input, const char *format){   //format = "%d%n" or "%x%n" or "%o%n"
        SuccessFlag = false;
        const char *inputPtr = *input;
        int scanfLen = 0, result;
        if (*inputPtr == ' ' || *inputPtr == '\t') inputPtr += 1; //use for speed up. (reduce exec-time from 0.25s to 0.05s for 100k lines of data)
        //sscanf(inputPtr, "%*[ \f\n\r\t\v]%n", &scanfLen);   //remove white-space characters
        //inputPtr += scanfLen;
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
    inline BigInteger *retrieveBInt(const char **input, const char *format){   //format = "%d%n" or "%x%n" or "%o%n"
        std::string bigIntString;
        int n_digit = -1;
        switch(format[1]){
        case 'o':{ bigIntString = SafeScanf::readBigInt08(*input); n_digit =  8; break; }
        case 'd':{ bigIntString = SafeScanf::readBigInt10(*input); n_digit = 10; break; }
        case 'x':{ bigIntString = SafeScanf::readBigInt16(*input); n_digit = 16; break; }
        }
        int size =  bigIntString.size();
        PERROR(size <= 0, printf("read BigInt fail, format = %s, input = %s", format, *input););
        *input += size;
        return new BigInteger(BigUnsignedInABase(bigIntString, n_digit));
    }
};

#endif
