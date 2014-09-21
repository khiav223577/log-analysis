
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
        initialized = false;
    }
    bool initialized;
    int get_prev_int(){
        PERROR(!initialized, printf("Error: fails to get_prev_int() in FormatterIPaddr."););
        return prev_ip;
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
    unsigned int prev_ip;
    HashCompressor< unsigned int, std::map<unsigned int, unsigned int> > hashCompressor;
    IndexerIpaddr indexer;
    SizeFlagManager sizeManager;
public:
    int execute1(const char **inputStream){
        #ifdef EVALUATE_TIME
            evalu_ip.start();
        #endif
        const char *originInput = *inputStream;
        prev_ip = retrieve(inputStream, format);
        initialized = true;
        if (attr_drop == false){
            unsigned int output = hashCompressor.compress(prev_ip);
            outputer->write(output, sizeManager.get_write_byte(output, executeCounter));
        }
        if (attr_peek == true) *inputStream = originInput;
        executeCounter += 1;
        debug(prev_ip);
        #ifdef EVALUATE_TIME
            evalu_ip.stop();
        #endif
        return 0;
    }
    int execute2(){
        unsigned char byte_num = sizeManager.get_read_byte(executeCounter);
        unsigned int output = (unsigned int) inputer->read_int(byte_num);
        prev_ip = hashCompressor.decompress(output);
        initialized = true;
        if (attr_index == true) indexer.addIndex(prev_ip); //store ip in hash.
        outputer->write(output, byte_num);
        executeCounter += 1;
        debug(prev_ip);
        return 0;
    }
    int execute3(){
        unsigned char byte_num = sizeManager.get_read_byte(executeCounter);
        unsigned int output = (unsigned int) inputer->read_int(byte_num);
        prev_ip = hashCompressor.decompress(output);
        initialized = true;
        executeCounter += 1;
        debug(prev_ip);
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
//-------------------------------------------------------------------------
//  add index.
//-------------------------------------------------------------------------
    IndexerBase *inner_create_indexer(){ return new IndexerIpaddr();}
    void inner_output_block_info(OutputManager *outputer){
        indexer.save(outputer);
        indexer.clear();
    }
    void inner_output_whole_info(OutputManager *outputer){
        IndexerIpaddr indexer(hashCompressor.getHashKeys());
        indexer.save(outputer);
    }
};



#endif
