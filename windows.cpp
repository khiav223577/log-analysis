

#ifndef ___windows_cpp__
#define ___windows_cpp__


#ifdef _WIN32
    #include<windows.h>
    void SetColor(unsigned short color){ //0�e��1�e��2�e��3�e�����G4�I��5�I��6�I��7�I�����G8�z��9�z��10�W���u11�����u12�k���u13�k���u14�z��15�z��
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

#else
    void SetColor(unsigned short color){}
    void ShowColors(int start,int end){}
    unsigned int GetTickCount() {
        timespec tp;
        clock_gettime(CLOCK_MONOTONIC,&tp);
        return (unsigned int)tp.tv_sec*1000+(unsigned int)(tp.tv_nsec/1000000);
    }
#endif










#endif