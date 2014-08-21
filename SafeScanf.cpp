
#ifndef ___SafeScanf_cpp__
#define ___SafeScanf_cpp__
#include <stdio.h>
#include <string>
#define _DEF_SafeScanf(FUNCNAME, BUFFERLEN, FORMAT) \
inline std::string FUNCNAME(const char *inputStream){\
    std::string output("");\
    int scanfLen;\
    char buffer[BUFFERLEN + 1];\
    while(*inputStream != '\0'){\
        int flag = sscanf(inputStream, "%" #BUFFERLEN FORMAT "%n",buffer,&scanfLen);\
        if (buffer == 0 || flag == 0) break;\
        if (0) printf("\"%s\" %d %d\n",buffer,scanfLen, flag);\
        inputStream += scanfLen;\
        output += buffer;\
    }\
    return output;\
}
#endif


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