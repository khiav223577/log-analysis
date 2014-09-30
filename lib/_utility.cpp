

#ifndef ____utility_cpp__
#define ____utility_cpp__
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define PERROR(E, CODE)\
    if ((E)){\
        SetColor( 7); puts("\n");                    \
        SetColor(12); printf("ERROR: ");             \
        SetColor( 7); printf("(%s) at ", #E);        \
        SetColor(10); printf("%s ",__func__);        \
        SetColor(14); printf("%d ", __LINE__);       \
        SetColor( 7); printf("in %s\n",__FILE__);    \
        SetColor( 7); CODE; exit(1);\
    }

inline char *get_file_extension(const char *path){
    unsigned int i = 0, counter = 0;
    int site = -1;
    char tmp;
    while((tmp = path[i]) != '\0'){
        if (tmp == '.'){
            site = (int) i;
            counter = 0;
        }
        i += 1;
        counter += 1;
    }
    if (site == -1){
        char *output = (char *) malloc(sizeof(char));
        *output = '\0';
        return output;
    }
    char *output = (char *) malloc((counter + 1) * sizeof(char));
    strcpy(output, path + site + 1);
    return output;
}
inline std::string getOsName(){
    #ifdef _WIN32
    return "Windows 32-bit";
    #elif _WIN64
    return "Windows 64-bit";
    #elif __unix || __unix__
    return "Unix";
    #elif __APPLE__ || __MACH__
    return "Mac OSX";
    #elif __linux__
    reutnr "Linux";
    #elif __FreeBSD__
    return "FreeBSD";
    #else
    return "Other";
    #endif
}
inline int wait_yes_or_no_input(){
    char input[2];
    fgets(input, 2, stdin);
    fflush(stdin);
    if (*input == 'N' || *input == 'n') return 0;
    if (*input == 'Y' || *input == 'y') return 1;
    return -1;
}
#ifdef _WIN32
    #include<windows.h>
    inline void SetColor(unsigned short color){ //0前藍1前綠2前紅3前景高亮4背藍5背綠6背紅7背景高亮8透明9透明10上劃線11左劃線12右劃線13右劃線14透明15透明
        HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hCon,color);
    }
    void ShowColors(int start,int end){
        for(int i = start; i <= end; i++){
            SetColor(i);
            printf("%d ",i);
        }
        SetColor(7);
        puts("");
    }
    #include<io.h>
    int file_exists(const char *filename){
        return (access(filename, F_OK) != -1);
    }
#else
    inline void SetColor(unsigned short color){}
    void ShowColors(int start,int end){}
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
    int file_exists(const char *filename){
        struct stat   buffer;
        return (stat (filename, &buffer) == 0);
    }
#endif
void SetColor2(){
    static int i = 2;
    SetColor(i);
    i = (i == 8 ? 2 : i + 1);
}
#endif

