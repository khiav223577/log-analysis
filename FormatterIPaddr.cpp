
#ifndef ___FormatterIPaddr_cpp__
#define ___FormatterIPaddr_cpp__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
class FormatterIPaddr : public FormatterController{
public:
    typedef FormatterController super;
//-------------------------------------------------------------------------
//  transform config-format to appropriate format. (for speed up)
//-------------------------------------------------------------------------
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
        char *trans_format(const char *_format){
            PERROR(strlen(_format) != 1, printf("Only support one char delimiter"););
            char *format = (char *) malloc(2);
            format[0] = _format[0];
            format[1] = '\0';
            return format;
            /*
            char *format = (char *) malloc(18);
            sprintf(format, "%%3d%c%%3d%c%%3d%c%%3d%%n", _format[0], _format[0], _format[0]);
            return format; //Ex: format = "%3d.%3d.%3d.%3d%n"*/
        }
	};
    FormatterIPaddr(const char *_format) : super(_format, new VirtualCreator()){
        executeCounter = 0;
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
    HashCompressor< unsigned int, std::map<unsigned int, unsigned int> > hashCompressor;
    SizeFlagManager sizeManager;
    int executeCounter;
    int execute1(const char **inputStream){
        #ifdef EVALUATE_TIME
            evalu_ip.start();
        #endif
        unsigned int result = retrieve(inputStream, format);
        unsigned int output = hashCompressor.compress(result);
        outputer->write(output, sizeManager.get_write_byte(output, executeCounter));
        executeCounter += 1;
        debug(result);
        #ifdef EVALUATE_TIME
            evalu_ip.stop();
        #endif
        return 0;
    }
    int execute2(){
        unsigned char byte_num = sizeManager.get_read_byte(executeCounter);
        unsigned int output = (unsigned int) inputer->read_int(byte_num);
        unsigned int result = hashCompressor.decompress(output);
        outputer->write(output, byte_num);
        executeCounter += 1;
        debug(result);
        return 0;
    }
    int execute3(){
        unsigned char byte_num = sizeManager.get_read_byte(executeCounter);
        unsigned int output = (unsigned int) inputer->read_int(byte_num);
        unsigned int result = hashCompressor.decompress(output);
        executeCounter += 1;
        debug(result);
        return 0;
    }
    inline void debug(int result){
        #ifdef DEBUG
            printf("%d.%d.%d.%d ",(result >> 24) & 255,(result >> 16) & 255,(result >> 8) & 255,(result >> 0) & 255);
        #endif
    }
//-------------------------------------------------------------------------
//  retrieve data from input according the format.
//-------------------------------------------------------------------------
    inline unsigned int retrieve(const char **input, const char *format){ //EX: input = 140.109.23.120
        int scanfLen = 0;
        //int a = -1,b = -1,c = -1,d = -1;
        int a = read_a_int(input); *input += 1; //TODO check format?
        int b = read_a_int(input); *input += 1;
        int c = read_a_int(input); *input += 1;
        int d = read_a_int(input);
        //sscanf(*input, format, &a, &b, &c, &d, &scanfLen);
        //PERROR(a == -1 || b == -1 || c == -1 || d == -1, printf("retrieve IP failed. input = %s", *input););
        *input += scanfLen;
        return (a << 24) | (b << 16) | (c << 8) | (d << 0);
    }
    inline unsigned int read_a_int(const char **input){
        unsigned int output = 0;
        const char *inputStream = *input;
        PERROR(!isdigit(*inputStream), printf("retrieve IP failed. input = %s, executeCounter = %d", *input, executeCounter););
        while(isdigit(*inputStream)){
            output = output * 10 + (*inputStream - '0');
            inputStream += 1;
        }
        *input = inputStream;
        return output;
    }
};

#endif
