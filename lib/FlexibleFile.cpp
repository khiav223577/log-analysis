
#ifndef ___FlexibleFile_cpp__
#define ___FlexibleFile_cpp__
#define FILE_MODE_ERR 0
#define FILE_MODE_RAW 1
#define FILE_MODE_BZ2 2
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
    FlexibleFile(            ) : current_file(FILE_MODE_ERR){ }
    FlexibleFile(  FILE *file) : current_file(FILE_MODE_ERR){ setFile(file); }
    FlexibleFile(BZFILE *file) : current_file(FILE_MODE_ERR){ setFile(file); }
    ~FlexibleFile(){
        close();
    }
    inline FlexibleFile& operator =(  FILE * const file){ setFile(file); return *this; }
    inline FlexibleFile& operator =(BZFILE * const file){ setFile(file); return *this; }
    inline void close(){
        switch(current_file){
        case  FILE_MODE_ERR:{                   return;}
        case  FILE_MODE_RAW:{ closeFile(file1);  break;}
        case  FILE_MODE_BZ2:{ closeFile(file2);  break;}
        default:{ Perror("Unown type"); }
        }
        //printf("%d\n", current_file);
        current_file = FILE_MODE_ERR;
    }
    inline size_t write(void *buffer, size_t size, size_t count){
        switch(current_file){
        case  FILE_MODE_RAW:{ return fwrite(buffer, size, count, file1);        }
        case  FILE_MODE_BZ2:{ return BZ2_bzwrite(file2, buffer, size * count);  }
        default:{ return Perror("Invalid  write"); }
        }
    }
    inline size_t read(void *buffer, size_t size, size_t count){
        switch(current_file){
        case  FILE_MODE_RAW:{ return fread(buffer, size, count, file1);        }
        case  FILE_MODE_BZ2:{ return BZ2_bzread(file2, buffer, size * count);  }
        default:{ return Perror("Invalid read"); }
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
        current_file = FILE_MODE_RAW;
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
        current_file = FILE_MODE_BZ2;
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
