
#include<stdio.h>
#include<iostream>
#include "windows.cpp"
#include "FormatterController.cpp"
//#include "RMap.cpp"
//#include "testing.cpp"
//#include "FlexibleInt.cpp"

#define MAX_LOG_SIZE 8192
RubyInterpreter *ruby;
ConfigInterfaceIN1 *ruby_interface;
InputFormatter *formatter;
inline void first_pass(const char *input_path, const char *output_path, const char *input_config, const char *output_config){
    OutputManager *outputer = new OutputManager(output_path);
    FILE *file = fopen2(input_path,"r");
    int line_count = 0;
    char buffer[MAX_LOG_SIZE];
    while(fgets(buffer, sizeof(buffer), file) != NULL){
        if (buffer[0] == '\0' || buffer[0] == '\n') continue;
        printf("%02d: ", ++line_count);
        formatter->execute1(outputer, buffer);
        puts("");
    }
    ruby_interface->save_config1(line_count, output_config);
    fclose(file);
    delete outputer;
}
inline void second_pass(const char *input_path, const char *output_path, const char *input_config, const char *output_config){
    InputManager *inputer = new InputManager(input_path);
    int line_count = ruby_interface->load_config1(input_config);
    for(int i = 0; i < line_count; ++i){
        printf("%02d: ", i);
        formatter->execute2(inputer);
        puts("");
    }
    delete inputer;
}
int main(int argc, char **argv){

    /*
    PERROR(1 == 1, printf("read BigInt fail, format = %s, input = %s", "000", ":???"););
    return 0;
    std::cout << bigIntegerToString(BigInteger(BigUnsignedInABase("01Aa", 16)));
    return 0;
    */
    const char *ConfigPath = "data/test_config2";
    const char *InputPath  = "data/test_input2";
    const char *OutputPath = "data/test_output2";
    ruby = new RubyInterpreter();
    {
        ruby_interface = new ConfigInterfaceIN1(ruby);
        formatter = ruby_interface->CreateFormatters(ConfigPath);
        char *input_path    = (char *) malloc(sizeof(char) * strlen( InputPath) + 1 + 0);
        char *output_path   = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 5);
        char *input_config  = NULL;
        char *output_config = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 7);
        sprintf( input_path  , "%s"        ,  InputPath);
        sprintf(output_path  , "%s.temp1"  , OutputPath);
        sprintf(output_config, "%s.config1", OutputPath);
        first_pass(input_path, output_path, input_config, output_config);
        free( input_path);
        free(output_path);
        free(input_config);
        free(output_config);
        delete formatter;
        delete ruby_interface;
    }
    puts("==============================================");
    {
        ruby_interface = new ConfigInterfaceIN1(ruby);
        formatter = ruby_interface->CreateFormatters(ConfigPath);
        char *input_path    = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 5);
        char *output_path   = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 5);
        char *input_config  = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 7);
        char *output_config = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 7);
        sprintf(input_path   , "%s.temp1"  , OutputPath);
        sprintf(output_path  , "%s.temp2"  , OutputPath);
        sprintf(input_config , "%s.config1", OutputPath);
        sprintf(output_config, "%s.config2", OutputPath);
        second_pass(input_path, output_path, input_config, output_config);
        free( input_path);
        free(output_path);
        free(input_config);
        free(output_config);
        delete formatter;
        delete ruby_interface;
    }
    delete ruby;
    //test_InputFormatter();
    //test_FormatterDate();



/*
    MapChar(int) BlahBlah;
    char key[] = "123";
    printf("%s => %d\n",key, BlahBlah[key]);
    printf("set %s = %d\n",key, 3);
    BlahBlah[key] = 3;
    key[0] = 'A';
    key[1] = 'B';
    key[2] = 'C';
    printf("%s => %d\n",key, BlahBlah[key]); //store reference
    printf("%s => %d\n","123", BlahBlah["123"]);
    return 0;
*/


return 0;
    /*
    char string[] = "a string,of ,,tokens";
    char *token = strtok(string,"o")
    while (token != NULL){
        printf("%s\n", token);
        token = strtok((char *) NULL,"[^ ]");
    }*/
    return 0;
    /*
    FILE *f1 = fopen2("config","r");
    char config[MAX_CONFIG_SIZE];
    fclose(f1);*/
    /*
    const char *filename = "test";
    char buffer[MAX_LOG_SIZE];
    FILE *f2 = fopen2(filename,"r");

    while(fgets(buffer, MAX_LOG_SIZE, f2) != NULL){



    }
    fclose(f2);
    printf("%d",'\n');*/
    return 0;
}

