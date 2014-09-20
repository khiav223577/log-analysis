


#ifndef ___RIPaddr_cpp__
#define ___RIPaddr_cpp__
#include <stdio.h>
#include <ctype.h>
class RIPaddr{
private:
    unsigned int ip;
public:
    RIPaddr(unsigned int _ip) : ip(_ip){
    }
    RIPaddr(unsigned int a, unsigned int b, unsigned int c, unsigned int d){
        ip = get_ip(a, b, c, d);
    }
    RIPaddr(const char *ip_str){ //EX: RIPaddr ip("140.109.1.10");
        const char **input = &ip_str;
        unsigned int a = read_a_int(input); *input += 1; //TODO check '\0'
        unsigned int b = read_a_int(input); *input += 1;
        unsigned int c = read_a_int(input); *input += 1;
        unsigned int d = read_a_int(input);
        ip = get_ip(a, b, c, d);
    }
    inline void show(){ printf("%d.%d.%d.%d ",(ip >> 24) & 255,(ip >> 16) & 255,(ip >> 8) & 255,(ip >> 0) & 255); }
    inline unsigned int get_ip(unsigned int a, unsigned int b, unsigned int c, unsigned int d){ return (a << 24) | (b << 16) | (c << 8) | (d << 0); }
    inline unsigned int to_int(){ return ip; }
private:
    inline unsigned int read_a_int(const char **input){
        unsigned int output = 0;
        const char *inputStream = *input;
        PERROR(!isdigit(*inputStream), printf("retrieve IP failed. input = %s", inputStream););
        while(isdigit(*inputStream)){
            output = output * 10 + (*inputStream - '0');
            inputStream += 1;
        }
        *input = inputStream;
        return output;
    }
};






#endif
