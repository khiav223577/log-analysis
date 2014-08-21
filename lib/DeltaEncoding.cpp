
#ifndef ___DeltaEncoding_cpp__
#define ___DeltaEncoding_cpp__

class DeltaEncoding{
protected:
    bool first_value_flag;
    int first_value;
    int current_value;
public:
    DeltaEncoding() : first_value_flag(false){
    }
//---------------------------------------
//  Core
//---------------------------------------
    inline int encode(int value){
        if (has_first_value() == false){
            set_first_value(value);
            return 0;
        }else{
            int pre_current_value = current_value;
            current_value = value;
            return current_value - pre_current_value;
        }
    }
    inline int decode(int value){
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
    inline  int get_first_value(){ return first_value; }
    inline bool has_first_value(){ return first_value_flag; }
    inline void set_first_value(int value){
        first_value = value;
        first_value_flag = true;
        current_value = value;
    }
};



#endif

