#ifndef ___StreamingRecorder_cpp__
#define ___StreamingRecorder_cpp__
#include "FlexibleInt.cpp"

template<typename XXXXX> class StreamingRecorder{
private:
    bool initialized, increasingFuncFlag, decreasingFuncFlag;
    XXXXX record_min, record_max, prev_value;
    void Perror(const char *str){
        printf("Error at StreamingRecorder: %s", str);
        exit(3);
    }
public:
//-------------------------------------------------------------------------
//  ACCESS
//-------------------------------------------------------------------------
    bool recordingFlag;
    inline XXXXX getPrevValue(){       return prev_value; }
    inline XXXXX getMinValue(){        return record_min; }
    inline XXXXX getMaxValue(){        return record_max; }
    inline XXXXX getMinMaxRange(){     return record_max - record_min; }
    inline bool isAlwaysIncreasing(){  return increasingFuncFlag; }
    inline bool isInitialized(){       return increasingFuncFlag; }
public:
    StreamingRecorder() : initialized(false), recordingFlag(true){
    }
    inline void nextData_quick(FlexibleInt value){ //for custom speed up methods.
        if (recordingFlag && initialized){
            if      (value.getValue() < record_min.getValue()) record_min.setValue(value.getValue());
            else if (value.getValue() > record_max.getValue()) record_max.setValue(value.getValue());
            if (increasingFuncFlag && value.getValue() < prev_value.getValue()) increasingFuncFlag = false;
            if (decreasingFuncFlag && value.getValue() > prev_value.getValue()) decreasingFuncFlag = false;
            prev_value = value;
            return;
        }
        nextData(value);
    }
    inline void nextData(XXXXX value){
        if (recordingFlag){
            if (initialized){
                if      (value < record_min) record_min = value;
                else if (value > record_max) record_max = value;
                if (increasingFuncFlag && value < prev_value) increasingFuncFlag = false;
                if (decreasingFuncFlag && value > prev_value) decreasingFuncFlag = false;
            }else{
                initialized = true;
                increasingFuncFlag = true;
                decreasingFuncFlag = true;
                record_min = value;
                record_max = value;
            }
        }
        prev_value = value;
    }
//------------------------------------------------------------
//  save
//------------------------------------------------------------
public:
    inline void save(FILE *file){
        fprintf(file, " %c", (increasingFuncFlag ? 'T' : 'F'));
        fprintf(file, " %c", (decreasingFuncFlag ? 'T' : 'F'));
        inner_save(file, prev_value);
    }
protected:
    inline void inner_save(FILE *file, unsigned int &unused){
        fprintf(file, " %u %u", record_min, record_max);
    }
    inline void inner_save(FILE *file, FlexibleInt &unused){
        record_min.save(file);
        record_max.save(file);
    }
//------------------------------------------------------------
//  load
//------------------------------------------------------------
public:
    inline void load(FILE *file){
        char tmp1, tmp2;
        fscanf(file, " %c", &tmp1);
        fscanf(file, " %c", &tmp2);
        if (tmp1 != 'T' && tmp1 != 'F') Perror("syntax error");
        if (tmp2 != 'T' && tmp2 != 'F') Perror("syntax error");
        increasingFuncFlag = (tmp1 == 'T' ? true : false);
        decreasingFuncFlag = (tmp1 == 'T' ? true : false);
        inner_load(file, prev_value);
        initialized = true;
        recordingFlag = false;
    }
protected:
    inline void inner_load(FILE *file, unsigned int &unused){
        fscanf(file, " %u %u", &record_min, &record_max);
    }
    inline void inner_load(FILE *file, FlexibleInt &unused){
        record_min = FlexibleInt::load(file);
        record_max = FlexibleInt::load(file);
    }
};


#endif
