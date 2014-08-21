
#ifndef ___FormatterString_cpp__
#define ___FormatterString_cpp__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "RMap.cpp"
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
            char *format = (char *) malloc((strlen(_format) + 1 + 7 + StringMaxLen) * sizeof(char));
            sprintf(format, "%%%d[^%s\n]%%n", MaxLen, _format);
            return format; //Ex: format = "%25[^,]%n"
        }
	};
	char *prev_result;
	const int MaxLen;
    FormatterString(const char *_format, int maxlen) : super(_format, new VirtualCreator(maxlen)), MaxLen(maxlen){
        prev_result = NULL;
        hashValueCounter = 0;
    }
    ~FormatterString(){
        RMap<MapChar(int)>::FreeClearMap_1(hashTable);
    }
    char *get_prev_string(){ //Will be called by FormatterIFStatement.
        PERROR(prev_result == NULL, printf("Error: fails to get_prev_string() in FormatterString."););
        return prev_result;
    }
public:
    virtual void save_config1(FILE *file);
    virtual void load_config1(FILE *file);
//--------------------------------------
//  execute
//--------------------------------------
    int execute1(OutputManager *outputer, const char **inputStream){
        char *str = retrieve(inputStream, format);
        prev_result = str;
        outputer->write(compress(str));
        debug();
        return 0;
    }
    int execute2(InputManager *inputer){
        unsigned int data = (unsigned int) inputer->read_int();
        prev_result = decompress(data);
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
        int scanfLen = 0;
        char *buffer = (char *) malloc((MaxLen + 1) * sizeof(char));
        sscanf(*input, format, buffer, &scanfLen);
        //sscanf(*input, "%511[^\n]%n", buffer, &scanfLen);
        *input += scanfLen;
        if (**input != '\0') *input += 1; //drop stop-word
        buffer[scanfLen] = '\0'; //avoid error when scanfLen == 0
        return buffer;
    }
//--------------------------------------
//  compress
//--------------------------------------
private:
    MapChar(int) hashTable;
    std::vector<char *> hashKeys;
    int hashValueCounter;
public:
    inline unsigned int compress(char *input){
        int value = RMap<MapChar(int)>::InsertKeyToMap(hashTable, input, hashValueCounter);
        if (value == hashValueCounter){
            hashKeys.push_back(input);
            hashValueCounter += 1; //str is a new key!
        }
        return value;
    }
    inline char *decompress(unsigned int input){
        return hashKeys[input];
    }
};

#endif
