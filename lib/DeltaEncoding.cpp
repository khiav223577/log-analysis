
#ifndef ___DeltaEncoding_cpp__
#define ___DeltaEncoding_cpp__

template<typename XXXXX> class DeltaEncoding{
protected:
    bool first_value_flag;
    XXXXX first_value;
    XXXXX current_value;
public:
    DeltaEncoding(){
        reset();
    }
    inline void reset(){
        first_value_flag = false;
    }
//---------------------------------------
//  Core
//---------------------------------------
    inline XXXXX encode(XXXXX &value){
        if (has_first_value() == false){
            set_first_value(value);
            return 0;
        }else{
            XXXXX pre_current_value = current_value;
            current_value = value;
            return current_value - pre_current_value;
        }
    }
    inline XXXXX decode(XXXXX &value){
        if (has_first_value() == false){
            set_first_value(value);
            return value;
        }else{
            current_value += value;
            return current_value;
        }
    }
//---------------------------------------
//  ACCESS
//---------------------------------------
    inline XXXXX get_first_value(){ return first_value; }
    inline XXXXX *get_first_valuePtr(){ return &first_value; }
    inline bool has_first_value(){ return first_value_flag; }
    inline void set_first_value(XXXXX &value){
        first_value = value;
        first_value_flag = true;
        current_value = value;
    }
//---------------------------------------
//  save
//---------------------------------------
    inline void save(OutputManager *outputer){
        outputer->write(first_value_flag ? 'T' : 'F');
        if (first_value_flag) inner_save(outputer, first_value);
    }
    inline void inner_save(OutputManager *outputer, int &unused){
        outputer->write(get_first_value());
    }
    inline void inner_save(OutputManager *outputer, FlexibleInt &unused){
        outputer->write(get_first_valuePtr());
    }
//---------------------------------------
//  load
//---------------------------------------
    inline void load(InputManager *inputer){
        char flag = inputer->read_char();
        PERROR(flag != 'F' && flag != 'T', printf("syntax error"));
        if (flag == 'F') return; //uninitialize
        inner_load(inputer, first_value);
    }
    inline void inner_load(InputManager *inputer, int &unused){
        int first = inputer->read_int();
        set_first_value(first);
    }
    inline void inner_load(InputManager *inputer, FlexibleInt &unused){
        FlexibleInt *tmp = inputer->read_flexibleInt();
        set_first_value(*tmp);
        delete tmp;
    }
};



#endif

