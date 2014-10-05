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
    StreamingRecorder() : recordingFlag(true){
        reset();
    }
    inline void reset(){
        initialized = false;
    }
    inline void nextData_quick(FlexibleInt &value){
        if (recordingFlag) initialized ? updateValue_quick(value) : setFirstValue_quick(value);
        prev_value = value;
    }
    inline void nextData(XXXXX &value){
        if (recordingFlag) initialized ? updateValue(value) : setFirstValue(value);
        prev_value = value;
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
    inline void save(OutputManager *outputer){
        outputer->write(initialized ? 'T' : 'F');
        if (!initialized) return;
        outputer->write(increasingFuncFlag ? 'I' : (decreasingFuncFlag ? 'D' : '.'));
        outputer->write(record_min);
        outputer->write(record_max);
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
    inline char read_char(FILE *file){ char c; fscanf(file, " %c", &c); return c; }
    inline char read_char(InputManager *inputer){ return inputer->read_char(); }
    template<typename INPUT_TYPE> inline void load(INPUT_TYPE *inputer){
        char tmp1 = read_char(inputer);
        if (tmp1 == 'F') return; //uninitialize
        if (tmp1 != 'T') Perror("syntax error");
        char tmp2 = read_char(inputer);
        switch(tmp2){
        case 'I':{ increasingFuncFlag =  true; decreasingFuncFlag = false; break;}
        case 'D':{ increasingFuncFlag = false; decreasingFuncFlag =  true; break;}
        case '.':{ increasingFuncFlag = false; decreasingFuncFlag = false; break;}
        default:{ Perror("syntax error"); return;}
        }
        inner_load(inputer, prev_value);
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
    inline void inner_load(InputManager *inputer, unsigned int &unused){
        record_min = (unsigned int) inputer->read_int();
        record_max = (unsigned int) inputer->read_int();
    }
    inline void inner_load(InputManager *inputer, FlexibleInt &unused){
        FlexibleInt *tmp1 = inputer->read_flexibleInt();
        FlexibleInt *tmp2 = inputer->read_flexibleInt();
        record_min = *tmp1;
        record_max = *tmp2;
        delete tmp1;
        delete tmp2;
    }
//----------------------------------------------
//  Operators
//----------------------------------------------
public:
    inline StreamingRecorder<XXXXX>& operator =(const StreamingRecorder<XXXXX> &x){
        initialized = x.initialized;
        increasingFuncFlag = x.increasingFuncFlag;
        decreasingFuncFlag = x.decreasingFuncFlag;
        record_min = x.record_min;
        record_max = x.record_max;
        prev_value = x.prev_value;
        return *this;
    }
};


#endif
