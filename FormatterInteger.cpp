
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
    FormatterInteger(const char *_format) : super(_format, new VirtualCreator()){
        BigIntFlagAt1 = -1;
        BigIntFlagAt2 = -1;
        executeCounter = 0;
    }
    int get_prev_int(){ //Will be called by FormatterIFStatement.
        PERROR(!streamingRecorder.isInitialized(), printf("Error: fails to get_prev_int() in FormatterInteger."););
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
    enum CompressionMode{Mode_drop, Mode_deltaEncoding, Mode_shift, Mode_none} compress_mode;
    inline CompressionMode caculate_compress_mode(){
        if (record_range == 0) return Mode_drop; //SameFlag
        if (streamingRecorder.isAlwaysIncreasing()) return Mode_deltaEncoding;
        if (record_range.isBigInt() == false) return Mode_shift;
        return Mode_none;
    }
private:
    FlexibleInt prev_int;
    SizeFlagManager sizeManager1;
    SizeFlagManager sizeManager2;
    int executeCounter, BigIntFlagAt1, BigIntFlagAt2;
    bool SuccessFlag;
    FlexibleInt record_range;
    DeltaEncoding<FlexibleInt> delta_encoding;
    StreamingRecorder<FlexibleInt> streamingRecorder;
public:
    int execute1(const char **inputStream){
        #ifdef EVALUATE_TIME
            evalu_int.start();
        #endif
        const char *originInput = *inputStream;
        if (BigIntFlagAt1 == -1){
            int value = retrieve(inputStream, format);
            if (SuccessFlag) prev_int = FlexibleInt(value);
            else BigIntFlagAt1 = executeCounter;
        }
        if (BigIntFlagAt1 != -1) prev_int = FlexibleInt(retrieveBInt(inputStream, format));
        if (attr_drop == false){
            if (BigIntFlagAt1 == -1){ //just for speed up. (reduce exec-time from 0.8s to 0.08s for 100k lines of data)
                streamingRecorder.nextData_quick(prev_int);
            }else{
                streamingRecorder.nextData(prev_int);
            }
            if (prev_int.isBigInt()){
                outputer->write(prev_int.getValuePtr());
            }else{
                outputer->write(prev_int.getValue(), sizeManager1.get_write_byte(prev_int.getValue(), executeCounter));
            }
        }
        if (attr_peek == true) *inputStream = originInput;
        executeCounter += 1;
        debug();
        #ifdef EVALUATE_TIME
            evalu_int.stop();
        #endif
        return 0;
    }
    int execute2(){
        unsigned char byte_num = 1;
        bool isBigInt = (BigIntFlagAt1 != -1 && executeCounter >= BigIntFlagAt1);
        if (isBigInt){
            prev_int = FlexibleInt(inputer->read_bigInt());
        }else{
            byte_num = sizeManager1.get_read_byte(executeCounter);
            prev_int = FlexibleInt(inputer->read_n_byte_int(byte_num));
        }
        switch(compress_mode){
        case Mode_drop:{  //do nothing
            break;}
        case Mode_deltaEncoding:{
            FlexibleInt output = delta_encoding.encode(prev_int); //delta encoding
            if (BigIntFlagAt2 == -1){
                output.try_to_cast_to_int();
                if (output.isBigInt()) BigIntFlagAt2 = executeCounter;
            }
            if (output.isBigInt()){
                outputer->write(output.getValuePtr());
            }else{
                int value = output.getValue();
                outputer->write(value, sizeManager2.get_write_byte(value, executeCounter));
            }
            break;}
        case Mode_shift:{
            FlexibleInt output = (prev_int - streamingRecorder.getMinValue());
            bool success = output.try_to_cast_to_int();
            PERROR(success == false, printf("Unable to cast BigInt to int");); //should always be able to.
            int value = output.getValue();
            outputer->write(value, sizeManager2.get_write_byte(value, executeCounter));
            break;}
        case Mode_none:{
            if (prev_int.isBigInt()){
                outputer->write(prev_int.getValuePtr());
            }else{
                outputer->write(prev_int.getValue(), byte_num);
            }
            break;}
        }
        executeCounter += 1;
        debug();
        return 0;
    }
    int execute3(){
        switch(compress_mode){
        case Mode_drop:{  //do nothing
            prev_int = streamingRecorder.getMinValue();
            break;}
        case Mode_deltaEncoding:{
            bool isBigInt = (BigIntFlagAt2 != -1 && executeCounter >= BigIntFlagAt2);
            FlexibleInt delta;
            if (isBigInt){
                delta = FlexibleInt(inputer->read_bigInt());
            }else{
                unsigned char byte_num = sizeManager2.get_read_byte(executeCounter);
                delta = FlexibleInt(inputer->read_n_byte_int(byte_num));
            }
            prev_int = delta_encoding.decode(delta); //delta encoding
            break;}
        case Mode_shift:{
            unsigned char byte_num = sizeManager2.get_read_byte(executeCounter);
            prev_int = streamingRecorder.getMinValue() + FlexibleInt(inputer->read_n_byte_int(byte_num));
            break;}
        case Mode_none:{
            bool isBigInt = (BigIntFlagAt1 != -1 && executeCounter >= BigIntFlagAt1);
            if (isBigInt){
                prev_int = FlexibleInt(inputer->read_bigInt());
            }else{
                unsigned char byte_num = sizeManager1.get_read_byte(executeCounter);
                prev_int = FlexibleInt(inputer->read_n_byte_int(byte_num));
            }
            break;}
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
        int scanfLen = 0, result = 0;
        while (*inputPtr == ' ' || *inputPtr == '\t') inputPtr += 1; //use for speed up. (reduce exec-time from 0.25s to 0.05s for 100k lines of data)
        //sscanf(inputPtr, "%*[ \f\n\r\t\v]%n", &scanfLen);   //remove white-space characters
        //inputPtr += scanfLen;
        bool positive = true;
        if (*inputPtr == '-' || *inputPtr == '+'){  //remove (+-) sign
            if (*inputPtr == '-') positive = false;
            inputPtr += 1;
        }
        while(*inputPtr == '0') inputPtr += 1;      //remove redundant zero
        char n_decimal = format[1];
        switch(n_decimal){
        case 'o':{
            while(1){
                int val = (*inputPtr - '0');
                if (val < 0 || val > 7) break;
                result = result * 8 + val;
                inputPtr += 1;
                scanfLen += 1;
            }
            if (scanfLen <= 10) SuccessFlag = true; //check overflow by length
            break;}
        case 'd':{
            while(1){
                int val = (*inputPtr - '0');
                if (val < 0 || val > 9) break;
                result = result * 10 + val;
                inputPtr += 1;
                scanfLen += 1;
            }
            if (scanfLen <=  9) SuccessFlag = true; //check overflow by length
            break;}
        case 'x':{
            while(1){
                int val = (*inputPtr >= 'a' ? (*inputPtr - 'a') : (*inputPtr >= 'A' ? (*inputPtr - 'A') : (*inputPtr - '0')));
                if (val < 0 || val > 15) break;
                result = result * 16 + val;
                inputPtr += 1;
                scanfLen += 1;
            }
            if (scanfLen <=  7) SuccessFlag = true; //check overflow by length
            break;}
        }
        if (positive){      //check overflow by (+-) sign
            if (result < 0) return -1;
        }else{
            result *= -1;
            if (result > 0) return -1;
        }
        if (SuccessFlag == false){
            inputPtr -= scanfLen;
            static char format2[3] = "%?";                      //compare input's and result's number by "strcmp"
            format2[1] = n_decimal;
            char *buffer = (char *) malloc(20 * sizeof(char));  //20 is enough for max number length in that we have checked overflow by length.
            sprintf(buffer, format2, (result < 0 ? -result : result));
            for(int i = 0; i < scanfLen; ++i){ if (inputPtr[i] != buffer[i]) return -1;}
            free(buffer);
            SuccessFlag = true;
            inputPtr += scanfLen;
        }
        *input = inputPtr;
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
