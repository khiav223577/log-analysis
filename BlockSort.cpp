#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//Burrows–Wheeler transform
char *decode(const char *input){
    /*
    EX: input = "BNN^AA\0A";
        counting = A:3, B:1, N:2, ^:1, others:0
        length = 7
        cumalArr = 0...3...4...6...7
                         0 1 2 3 4 5 6 7
        ori_to_sorted = [3,4,5,6,0,1,7,2]
        sorted_to_ori = [4,5,7,0,1,2,3,6]
        output = "^BANANA"; //input[3],input[0],input[4],input[1],input[5],input[2],input[7]
    */
    char *read_input = (char *) input;
    int cumalArr[256],counting[256],length = 0;
    memset(counting, 0, sizeof(counting));
    for(; *read_input != '\0'; ++read_input){
        counting[*read_input] += 1;
        length += 1;
    }
    for(++read_input; *read_input != '\0'; ++read_input){
        counting[*read_input] += 1;
        length += 1;
    }
    cumalArr[0] = 0;
    cumalArr[255] = length;
    char *output        = (char *) malloc(sizeof(char) * (length + 1));
    int  *sorted_to_ori = ( int *) malloc(sizeof( int) * (length + 1));
    for(int i = 1; i < 255; ++i) cumalArr[i] = cumalArr[i - 1] + counting[i];
    for(int i = 0; i <= length; ++i) sorted_to_ori[cumalArr[(unsigned char) (input[i] - 1)]++] = i;
    int currIdx = sorted_to_ori[length];
    output[length] = '\0';
    for(int i = 0; i < length; ++i){
        currIdx = sorted_to_ori[currIdx];
        output[i] = input[currIdx];
    }
    free(sorted_to_ori);
    return output;
}
//----------------------------------------------------------------------------------
int main(){
    //char *output = decode("BNN^AA\0A");
    //char *output = decode("d\0aabbc");
    char *output = decode("\0caabbd");
    printf("%s",output);
    free(output);
    return 0;
}
