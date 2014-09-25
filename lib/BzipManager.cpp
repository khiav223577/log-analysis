

#ifndef ___BzipManager_cpp__
#define ___BzipManager_cpp__

#define BZ_IMPORT
#include <stdio.h>
#include <stdlib.h>
#include "bzip2-1.0.6/bzlib.h"

#ifdef _WIN32
    #include <windows.h>
#endif



class BzipManager{
public:
    BzipManager(const char *path){
        if (path != NULL) BzipManager::loadBz2Library(path);
    }
    ~BzipManager(){
    }
//-------------------------------------------
//  load Bz2 Library
//-------------------------------------------
private:
    static bool bz2Loaded;
    #ifdef _WIN32
        static HINSTANCE bz2hLib;
    #endif
public:

    #ifdef _WIN32
        static int loadBz2Library(const char *path){
            if (bz2Loaded == true) return 0;
            HINSTANCE lib = LoadLibrary(path);
            if (lib == NULL){
                fprintf(stderr, "Can't load %s\n", path);
                return -1;
            }
            BZ2_bzlibVersion = (const char * (__stdcall *)(       void                    )) GetProcAddress(lib, "BZ2_bzlibVersion");
            BZ2_bzopen       = (    BZFILE * (__stdcall *)(const char* , const char *     )) GetProcAddress(lib, "BZ2_bzopen");
            BZ2_bzdopen      = (    BZFILE * (__stdcall *)(        int , const char *     )) GetProcAddress(lib, "BZ2_bzdopen");
            BZ2_bzread       = (         int (__stdcall *)(    BZFILE* ,       void *, int)) GetProcAddress(lib, "BZ2_bzread");
            BZ2_bzwrite      = (         int (__stdcall *)(    BZFILE* ,       void *, int)) GetProcAddress(lib, "BZ2_bzwrite");
            BZ2_bzflush      = (         int (__stdcall *)(    BZFILE*                    )) GetProcAddress(lib, "BZ2_bzflush");
            BZ2_bzclose      = (        void (__stdcall *)(    BZFILE*                    )) GetProcAddress(lib, "BZ2_bzclose");
            BZ2_bzerror      = (const char * (__stdcall *)(    BZFILE* ,        int *     )) GetProcAddress(lib, "BZ2_bzerror");
            if(!BZ2_bzlibVersion || !BZ2_bzopen || !BZ2_bzdopen || !BZ2_bzread || !BZ2_bzwrite || !BZ2_bzflush || !BZ2_bzclose || !BZ2_bzerror) {
                fprintf(stderr, "GetProcAddress failed.\n");
                return -1;
            }
            bz2hLib = lib;
            bz2Loaded = true;
            return 0;
        }
       static void freeBz2Library(){
            if (bz2Loaded == true) return;
            FreeLibrary(bz2hLib);
            bz2Loaded = false;
        }
    #else
        static void loadBz2Library(const char *path){
            bz2Loaded = true;
        }
        static void freeBz2Library(){
            bz2Loaded = false;
        }
    #endif
//-------------------------------------------
//  File methods
//-------------------------------------------
public:
    static FILE *openFile(const char *path, const char *mode){
        FILE *file = (path == NULL ? stdout : fopen(path, mode));
        if (file == NULL){
            printf("can't open [%s]\n", path);
            perror("reason:");
            exit(1);
        }
        return file;
    }
    static BZFILE *openBz2File(const char *path, const char *mode){
        BZFILE *file = (path == NULL ? BZ2_bzdopen(fileno(stdin), mode) : BZ2_bzopen(path, mode));
        if (file == NULL){
            printf("can't bz2openstream\n");
            exit(1);
        }
        return file;
    }
    static void closeFile(FILE *file){
        if(file != stdout) fclose(file);
    }
    static void closeFile(BZFILE *file){
        BZ2_bzclose(file);
    }
//-------------------------------------------
//  compress / decompress
//-------------------------------------------
public:
    void decompress(const char *input_path, const char *output_path){
        FILE   *wfile = openFile(output_path, "wb");
        BZFILE *rfile = openBz2File(input_path, "rb");
        int len;
        char buff[0x1000];
        while((len = BZ2_bzread(rfile, buff, 0x1000)) > 0) fwrite(buff, 1, len, wfile);
        closeFile(wfile);
        closeFile(rfile);
    }
    void compress(const char *input_path, const char *output_path, int level = 9){
        char mode[10] = "w0";
        mode[1] += level;
        BZFILE *wfile = openBz2File(output_path, mode);;
        FILE   *rfile = openFile(input_path, "rb");
        int len;
        char buff[0x1000];
        while((len = fread(buff, 1, 0x1000, rfile)) > 0) BZ2_bzwrite(wfile, buff, len);
        closeFile(rfile);
        closeFile(wfile);
    }
};
bool BzipManager::bz2Loaded = false;
#ifdef _WIN32
    HINSTANCE BzipManager::bz2hLib = NULL;
#endif
/*
int main(){
    BzipManager bzManager;
    bzManager.compress("sdjfljoi.txt", "12345.bz2");
    BzipManager::freeBz2Library();
}*/

#endif

