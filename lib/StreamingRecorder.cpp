#ifndef ___StreamingRecorder_cpp__
#define ___StreamingRecorder_cpp__
#include "FlexibleInt.cpp"

template<typename XXXXX> class StreamingRecorder{
private:
    bool initialized, increasingFuncFlag, decreasingFuncFlag;
    XXXXX record_min, record_max, prev_value;
    inline void Perror(const char *str){
        printf("Error at StreamingRecorder: %s", str);
        exit(3);
    }
protected:
    inline void setFirstValue(XXXXX &value){
        initialized = true;
        increasingFuncFlag = true;
        decreasingFuncFlag = true;
        record_min = value;
        record_max = value;
    }
    inline void updateValue(XXXXX &value){
        if      (value < record_min) record_min = value;
        else if (value > record_max) record_max = value;
        if (increasingFuncFlag && value < prev_value) increasingFuncFlag = false;
        if (decreasingFuncFlag && value > prev_value) decreasingFuncFlag = false;
    }
    inline void updateValue_quick(FlexibleInt &value){
        if      (value.getValue() < record_min.getValue()) record_min.setValue(value.getValue());
        else if (value.getValue() > record_max.getValue()) record_max.setValue(value.getValue());
        if (increasingFuncFlag && value.getValue() < prev_value.getValue()) increasingFuncFlag = false;
        if (decreasingFuncFlag && value.getValue() > prev_value.getValue()) decreasingFuncFlag = false;
    }
    inline void setFirstValue_quick(FlexibleInt &value){
        setFirstValue(value);
    }
public:
//-------------------------------------------------------------------------
//  ACCESS
//-------------------------------------------------------------------------
    bool recordingFlag;
    inline XXXXX& getPrevValue(){       return prev_value; }
    inline XXXXX& getMinValue(){        return record_min; }
    inline XXXXX& getMaxValue(){        return record_max; }
    inline XXXXX getMinMaxRange(){      return record_max - record_min; }
    inline bool& isAlwaysIncreasing(){  return increasingFuncFlag; }
    inline bool& isAlwaysDecreasing(){  return decreasingFuncFlag; }
    inline bool& isInitialized(){       return initialized; }
public:
    StreamingRecorder() : initialized(false), recordingFlag(true){
    }
    inline void nextData_quick(FlexibleInt &value){
        if (recordingFlag) initialized ? updateValue_quick(value) : setFirstValue_quick(value);
        prev_value = value;
    }
    inline void nextData(XXXXX &value){
        if (recordingFlag) initialized ? updateValue(value) : setFirstValue(value);
        prev_value = value;
    }
    inline void reset(){
        initialized = false;
    }
//------------------------------------------------------------
//  save
//------------------------------------------------------------
public:
    inline void save(FILE *file){
        fprintf(file, " %c", (initialized ? 'T' : 'F'));
        if (!initialized) return;
        fprintf(file, " %c", (increasingFuncFlag ? 'I' : (decreasingFuncFlag ? 'D' : '.')));
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
        char tmp1;
        fscanf(file, " %c", &tmp1);
        if (tmp1 == 'F') return; //uninitialize
        if (tmp1 != 'T') Perror("syntax error");
        fscanf(file, " %c", &tmp1);
        switch(tmp1){
        case 'I':{ increasingFuncFlag =  true; decreasingFuncFlag = false; break;}
        case 'D':{ increasingFuncFlag = false; decreasingFuncFlag =  true; break;}
        case '.':{ increasingFuncFlag = false; decreasingFuncFlag = false; break;}
        default:{ Perror("syntax error"); return;}
        }
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
