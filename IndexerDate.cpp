
#ifndef ___IndexerDate_cpp__
#define ___IndexerDate_cpp__
#include "lib/RDate.cpp"
class IndexerDate : public IndexerBase{
public:
    typedef IndexerBase super;
private:
    StreamingRecorder<unsigned int> streamingRecorder;
public:
    IndexerDate(){
    }
    IndexerDate(StreamingRecorder<unsigned int> &recorder){
        streamingRecorder = recorder;
    }
    inline void addIndex(unsigned int value){
       streamingRecorder.nextData(value);
    }
//-------------------------------------------------------------------------
//  save / load / clear
//-------------------------------------------------------------------------
    void load(InputManager *inputer){
        unsigned int min = inputer->read_int();
        unsigned int max = inputer->read_int();
        PERROR(min > max, printf("Error occurs when loading IndexerDate"); );
        streamingRecorder.nextData(min);
        streamingRecorder.nextData(max);
    }
    void save(OutputManager *outputer){
        //if (recorder.isAlwaysIncreasing() || recorder.isAlwaysDecreasing()) outputer->write('C');
        //else outputer->write('.');
        unsigned int min = (streamingRecorder.isInitialized() ? streamingRecorder.getMinValue() : 999999);
        unsigned int max = (streamingRecorder.isInitialized() ? streamingRecorder.getMaxValue() : 0);
        outputer->write(min);
        outputer->write(max);
    }
    void clear(){
        streamingRecorder.reset();
    }
//-------------------------------------------------------------------------
//  ACCESS
//-------------------------------------------------------------------------
    bool hasValueEqualTo(int val){
        //RDate d1( streamingRecorder.getMinValue());RDate d2( val);RDate d3( streamingRecorder.getMaxValue());d1.show();d2.show();d3.show();
        return (streamingRecorder.getMinValue() <= (unsigned int) val && (unsigned int) val <= streamingRecorder.getMaxValue());
    }
    bool hasValueBetween(int val1, int val2){ //val1 <= val2
        unsigned int uval1 = val1, uval2 = val2;
        if (streamingRecorder.getMinValue() > uval2) return false;
        if (streamingRecorder.getMaxValue() < uval1) return false;
        if (uval1 > uval2) return false;
        return true;
    }
};

#endif

