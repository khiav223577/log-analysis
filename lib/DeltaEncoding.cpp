
#ifndef ___DeltaEncoding_cpp__
#define ___DeltaEncoding_cpp__

template<typename XXXXX> class DeltaEncoding{
protected:
    bool first_value_flag;
    XXXXX first_value;
    XXXXX current_value;
public:
    DeltaEncoding() : first_value_flag(false){
        reset();
    }
    inline void reset(){
        current_value = first_value;
    }
//---------------------------------------
//  Core
//---------------------------------------
    inline XXXXX encode(XXXXX value){
        if (has_first_value() == false){
            set_first_value(value);
            return 0;
        }else{
            XXXXX pre_current_value = current_value;
            current_value = value;
            return current_value - pre_current_value;
        }
    }
    inline XXXXX decode(XXXXX value){
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
    inline bool has_first_value(){ return first_value_flag; }
    inline void set_first_value(XXXXX value){
        first_value = value;
        first_value_flag = true;
        current_value = value;
    }
};



#endif

