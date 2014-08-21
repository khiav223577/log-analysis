
#ifndef ___FormatterString_cpp__
#define ___FormatterString_cpp__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<memory.h>
class FormatterString : public FormatterController{
public:
    typedef FormatterController super;
//-------------------------------------------------------------------------
//  transform config-format to appropriate format. (for speed up)
//-------------------------------------------------------------------------
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
    public:
        const int MaxLen, StringMaxLen;
        VirtualCreator(const int maxlen) : MaxLen(maxlen), StringMaxLen(maxlen < 10 ? 1 : maxlen < 100 ? 2 : maxlen < 1000 ? 3 : maxlen < 10000 ? 4 : maxlen < 100000 ? 5 :
                                                                        maxlen < 1000000 ? 6 : maxlen < 10000000 ? 7 : maxlen < 100000000 ? 8 : maxlen < 1000000000 ? 9 : 10){
        }
        char *trans_format(const char *_format){
            char *format = (char *) malloc((strlen(_format) + 1) * sizeof(char));
            strcpy(format,  _format);
            return format; //Ex: format = ", "
            /*
            char *format = (char *) malloc((strlen(_format) + 1 + 7 + StringMaxLen) * sizeof(char));
            sprintf(format, "%%%d[^%s\n]%%n", MaxLen, _format);
            return format; //Ex: format = "%25[^,]%n"*/
        }
	};
	char *prev_result;
	const int MaxLen;
	int format_len;
    FormatterString(const char *_format, int maxlen) : super(_format, new VirtualCreator(maxlen)), MaxLen(maxlen){
        prev_result = NULL;
        executeCounter = 0;
        bit_num = 32;
        format_len = strlen(format);
    }
    ~FormatterString(){
        free(prev_result);
    }
    char *get_prev_string(){ //Will be called by FormatterIFStatement.
        PERROR(prev_result == NULL, printf("Error: fails to get_prev_string() in FormatterString."););
        return prev_result;
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
    HashCompressor<char *, MapChar(unsigned int)> hashCompressor;
    SizeFlagManager sizeManager;
    int executeCounter;
    unsigned char bit_num;
public:
    int execute1(const char **inputStream){
        #ifdef EVALUATE_TIME
            evalu_string.start();
        #endif
        char *str = retrieve(inputStream, format);
        free(prev_result);
        prev_result = str;
        unsigned int output = hashCompressor.compress(str);
        outputer->write(output, sizeManager.get_write_byte(output, executeCounter));
        executeCounter += 1;
        debug();
        #ifdef EVALUATE_TIME
            evalu_string.stop();
        #endif
        return 0;
    }
    int execute2(){
        unsigned char byte_num = sizeManager.get_read_byte(executeCounter);
        unsigned int output = (unsigned int) inputer->read_int(byte_num);
        prev_result = hashCompressor.decompress(output);
        if (bit_num == 0); //do nothing
        else if ((byte_num << 3) >= bit_num) outputer->write_bits(output, bit_num);
        else outputer->write(output, byte_num);
        executeCounter += 1;
        debug();
        return 0;
    }
    int execute3(){
        unsigned char byte_num = sizeManager.get_read_byte(executeCounter);
        unsigned int output;
        if (bit_num == 0) output = 0;
        else if ((byte_num << 3) >= bit_num) output = inputer->read_bits(bit_num);
        else output = (unsigned int) inputer->read_int(byte_num);
        prev_result = hashCompressor.decompress(output);

        executeCounter += 1;
        debug();
        return 0;
    }
    inline void debug(){
        #ifdef DEBUG
            SetColor2(); printf("[%s] ",prev_result); SetColor(7);//DEBUG
        #endif
    }
//-------------------------------------------------------------------------
//  retrieve data from input according the format.
//-------------------------------------------------------------------------
    inline char *retrieve(const char **input, const char *format){
        /*
        int scanfLen = 0;
        char *buffer = (char *) malloc((MaxLen + 1) * sizeof(char));
        sscanf(*input, format, buffer, &scanfLen);
        *input += scanfLen;*/
        char *buffer = (char *) malloc((MaxLen + 1) * sizeof(char));
        const char *inputStr = *input;
        int scanfLen = 0;
        for(scanfLen = 0; scanfLen < MaxLen; ++scanfLen){
            for(int i = 0; i < format_len; ++i) if (*inputStr == format[i]) goto BREAK;
            buffer[scanfLen] = *inputStr;
            inputStr += 1;
        } BREAK:
        *input = inputStr;
        if (**input != '\0') *input += 1; //drop stop-word
        buffer[scanfLen] = '\0'; //avoid error when scanfLen == 0
        return buffer;
    }
};

#endif
