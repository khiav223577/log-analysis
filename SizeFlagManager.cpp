#ifndef ___SizeFlagManager_cpp__
#define ___SizeFlagManager_cpp__
class SizeFlagManager{
private:
    int Size2FlagAt;
    int Size3FlagAt;
    int Size4FlagAt;
public:
    inline void setSize2FlagAt(int val){ Size2FlagAt = val; }
    inline void setSize3FlagAt(int val){ Size3FlagAt = val; }
    inline void setSize4FlagAt(int val){ Size4FlagAt = val; }
public:
    SizeFlagManager(){
        Size2FlagAt = -1;
        Size3FlagAt = -1;
        Size4FlagAt = -1;
    }
    inline unsigned char get_write_byte(int output, unsigned int executeCounter){
        if (output >= (1 << 23) || output < -(1 << 23)){
            if (Size2FlagAt == -1) Size2FlagAt = executeCounter;
            if (Size3FlagAt == -1) Size3FlagAt = executeCounter;
            if (Size4FlagAt == -1) Size4FlagAt = executeCounter;
        }else if (output >= (1 << 15) || output < -(1 << 15)){
            if (Size2FlagAt == -1) Size2FlagAt = executeCounter;
            if (Size3FlagAt == -1) Size3FlagAt = executeCounter;
        }else if (output >= (1 <<  7) || output < -(1 <<  7)){
            if (Size2FlagAt == -1) Size2FlagAt = executeCounter;
        }
        if (Size2FlagAt == -1) return 1;
        if (Size3FlagAt == -1) return 2;
        if (Size4FlagAt == -1) return 3;
        return 4;
    }
    inline unsigned char get_write_byte(unsigned int output, unsigned int executeCounter){
        if (output >= (1 << 24)){
            if (Size2FlagAt == -1) Size2FlagAt = executeCounter;
            if (Size3FlagAt == -1) Size3FlagAt = executeCounter;
            if (Size4FlagAt == -1) Size4FlagAt = executeCounter;
        }else if (output >= (1 << 16)){
            if (Size2FlagAt == -1) Size2FlagAt = executeCounter;
            if (Size3FlagAt == -1) Size3FlagAt = executeCounter;
        }else if (output >= (1 <<  8)){
            if (Size2FlagAt == -1) Size2FlagAt = executeCounter;
        }
        if (Size2FlagAt == -1) return 1;
        if (Size3FlagAt == -1) return 2;
        if (Size4FlagAt == -1) return 3;
        return 4;
    }
    inline unsigned char get_read_byte(unsigned int executeCounter){
        if (Size2FlagAt == -1 || executeCounter < (unsigned int) Size2FlagAt) return 1;
        if (Size3FlagAt == -1 || executeCounter < (unsigned int) Size3FlagAt) return 2;
        if (Size4FlagAt == -1 || executeCounter < (unsigned int) Size4FlagAt) return 3;
        return 4;
    }
    inline void save(FILE *file){
        fprintf(file, " %d %d %d", Size2FlagAt, Size3FlagAt, Size4FlagAt);
    }
    inline void load(FILE *file){
        fscanf(file, " %d %d %d", &Size2FlagAt, &Size3FlagAt, &Size4FlagAt);
    }
};


#endif
