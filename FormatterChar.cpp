
#ifndef ___FormatterChar_cpp__
#define ___FormatterChar_cpp__

class FormatterChar : public FormatterController{
public:
    typedef FormatterController super;
//-------------------------------------------------------------------------
//  transform config-format to appropriate format. (for speed up)
//-------------------------------------------------------------------------
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
        char *trans_format(const char *_format){
            return NULL;
        }
	};
    FormatterChar(const char *_format) : super(_format, new VirtualCreator()){
        prev_result[0] = '\0';
        prev_result[1] = '\0';
    }
    char prev_result[2];
    char *get_prev_string(){ //Will be called by FormatterIFStatement.
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
public:
    int execute1(const char **inputStream){
        #ifdef EVALUATE_TIME
            evalu_char.start();
        #endif
        const char *originInput = *inputStream;
        char result = retrieve(inputStream, format);
        prev_result[0] = result;
        if (attr_drop == false){
            outputer->write(result);
        }
        if (attr_peek == true) *inputStream = originInput;
        executeCounter += 1;
        debug(result);
        #ifdef EVALUATE_TIME
            evalu_char.stop();
        #endif
        return 0;
    }
    int execute2(){
        char result = inputer->read_char();
        prev_result[0] = result;
        outputer->write(result);
        executeCounter += 1;
        debug(result);
        return 0;
    }
    int execute3(){
        char result = inputer->read_char();
        prev_result[0] = result;
        executeCounter += 1;
        debug(result);
        return 0;
    }
    inline void debug(char result){
        #ifdef DEBUG
            SetColor2(); printf("'%c'", result); SetColor(7);
        #endif
    }
//-------------------------------------------------------------------------
//  retrieve data from input according the format.
//-------------------------------------------------------------------------
    inline char retrieve(const char **input, const char *format){
        const char *inputStr = *input;
        *input += 1;
        return *inputStr;
    }
//-------------------------------------------------------------------------
//  block info
//-------------------------------------------------------------------------
    void inner_save_block_info(OutputManager *outputer){
    }
    void inner_load_block_info(InputManager *inputer){
    }
};

#endif
