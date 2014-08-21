
#ifndef ___FlexibleFile_cpp__
#define ___FlexibleFile_cpp__

#include "BzipManager.cpp"

class FlexibleFile{
private:
    unsigned int current_file;
    FILE *file1;
    BZFILE *file2;
public:
//---------------------------------------------
//  ACCESS
//---------------------------------------------
    FlexibleFile(            ) : current_file(0){ }
    FlexibleFile(  FILE *file) : current_file(0){ setFile(file); }
    FlexibleFile(BZFILE *file) : current_file(0){ setFile(file); }
    ~FlexibleFile(){
        close();
    }
    inline FlexibleFile& operator =(  FILE * const file){ setFile(file); return *this; }
    inline FlexibleFile& operator =(BZFILE * const file){ setFile(file); return *this; }
    inline void close(){
        switch(current_file){
        case  0:{                   return;}
        case  1:{ closeFile(file1);  break;}
        case  2:{ closeFile(file2);  break;}
        default:{ Perror("Unown type"); }
        }
        //printf("%d\n", current_file);
        current_file = 0;
    }
    inline size_t write(void *buffer, size_t size, size_t count){
        switch(current_file){
        case  1:{ return fwrite(buffer, size, count, file1);        }
        case  2:{ return BZ2_bzwrite(file2, buffer, size * count);  }
        default:{ return Perror("Invalid  write"); }
        }
    }
    inline size_t read(void *buffer, size_t size, size_t count){
        switch(current_file){
        case  1:{ return fread(buffer, size, count, file1);        }
        case  2:{ return BZ2_bzread(file2, buffer, size * count);  }
        default:{ return Perror("Invalid  read"); }
        }
    }
private:
    int Perror(const char *str){
        printf("ERROR in FlexibleFile %s\ncurrent_file = %d\n", str, current_file);
        exit(2);
        return 2;
    }
//---------------------------------------------
//  (1) file
//---------------------------------------------
    inline void setFile(FILE * const file){
        close();
        if (file == NULL) return;
        file1 = file;
        current_file = 1;
    }
    inline void closeFile(FILE * const file){
        if(file != stdout) fclose(file);
    }
//---------------------------------------------
//  (2) Bzip2 file
//---------------------------------------------
    inline void setFile(BZFILE * const file){
        close();
        if (file == NULL) return;
        file2 = file;
        current_file = 2;
        BzipManager::loadBz2Library();
    }
    inline void closeFile(BZFILE * const file){
        BZ2_bzclose(file);
    }
};

/*
int main(){
    FILE *f = NULL;
    FlexibleFile file = f;
}*/

#endif
