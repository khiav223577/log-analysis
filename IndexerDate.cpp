
#ifndef ___IndexerDate_cpp__
#define ___IndexerDate_cpp__

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
        if (min > max) return;
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

};

#endif

