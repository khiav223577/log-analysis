
#ifndef ___SafeScanf_cpp__
#define ___SafeScanf_cpp__
#include <stdio.h>
#include <string>
#define _DEF_SafeScanf(HEADER, FUNCNAME, BUFFERLEN, PRE_FORMAT, FORMAT, REMOVE_SPACE) \
HEADER std::string FUNCNAME(const char *inputStream){\
    std::string output("");\
    int scanfLen;\
    char buffer[BUFFERLEN + 1];\
    if (REMOVE_SPACE) while (*inputStream == ' ' || *inputStream == '\t') inputStream += 1;\
    while(*inputStream != '\0'){\
        int flag = sscanf(inputStream, PRE_FORMAT "%" #BUFFERLEN FORMAT "%n",buffer,&scanfLen);\
        if (scanfLen == 0 || flag == 0) break;\
        if (0) printf("\"%s\" %d %d\n",buffer,scanfLen, flag);\
        inputStream += scanfLen;\
        output += buffer;\
    }\
    return output;\
}\
HEADER std::string FUNCNAME(FILE *file){\
    std::string output("");\
    int scanfLen;\
    char buffer[BUFFERLEN + 1];\
    if (REMOVE_SPACE) fscanf(file," ");\
    while(1){\
        int flag = fscanf(file, PRE_FORMAT "%" #BUFFERLEN FORMAT "%n",buffer,&scanfLen);\
        if (scanfLen == 0 || flag == 0) break;\
        output += buffer;\
    }\
    return output;\
}\
HEADER std::string FUNCNAME(){\
    std::string output("");\
    int scanfLen;\
    char buffer[BUFFERLEN + 1];\
    if (REMOVE_SPACE) scanf(" ");\
    while(1){\
        int flag = scanf(PRE_FORMAT "%" #BUFFERLEN FORMAT "%n",buffer,&scanfLen);\
        if (scanfLen == 0 || flag == 0) break;\
        output += buffer;\
    }\
    return output;\
}


class SafeScanf{
public:
    _DEF_SafeScanf(static inline, readBigInt08, 255, "", "[0-7]", true);
    _DEF_SafeScanf(static inline, readBigInt10, 255, "", "[0-9]", true);
    _DEF_SafeScanf(static inline, readBigInt16, 255, "", "[0-9a-fA-F]", true);
    _DEF_SafeScanf(static inline, readHashString, 255, "", "[^\n]", false);
};


/*
_DEF_SafeScanf(readString, 5, "[0-9]");
#include <iostream>
int main(){
    std::cout << readString("1\n") << std::endl;
    std::cout << readString("12\n") << std::endl;
    std::cout << readString("123\n") << std::endl;
    std::cout << readString("1234\n") << std::endl;
    std::cout << readString("12345\n") << std::endl;
    std::cout << readString("123456\n") << std::endl;
    std::cout << readString("12345678\n") << std::endl;
    std::cout << readString("123456789\n") << std::endl;
    std::cout << readString("123456789012345678901234567abcdefgh\n") << std::endl;
    printf("%d",readString("123456789012345678901234567abcdefgh\n").size());
    return 0;
}
*/

#endif
