#ifndef ___SimpleFlagManager_cpp__
#define ___SimpleFlagManager_cpp__
class SimpleFlagManager{
private:
    int FlagAt;
public:
    inline void setFlagAt(int val){ FlagAt = val; }
    inline bool getFlag(int val){ return (FlagAt != -1 && val >= FlagAt); }
public:
    SimpleFlagManager(){
        reset();
    }
    inline void reset(){
        FlagAt = -1;
    }
    inline void save(FILE *file){
        fprintf(file, " %d", FlagAt);
    }
    inline void load(FILE *file){
        fscanf(file, " %d", &FlagAt);
    }
    inline void save(OutputManager *outputer){
        outputer->write(FlagAt);
    }
    inline void load(InputManager *inputer){
        FlagAt = inputer->read_int();
    }
};


#endif
