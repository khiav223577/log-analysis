//#define DEBUG 20
//#define GROUP_FORMATTER_DATA
#define EVALUATE_TIME
#define BLOCK_SIZE 2220000
//---------------------------------------------------
#include<stdio.h>
#include<typeinfo>
#include<iostream>
#include "windows.cpp"
//---------------------------------------------------
#include "lib/ShowTime.cpp"
#ifdef EVALUATE_TIME
    EvaluateTime evalu_int;
    EvaluateTime evalu_string;
    EvaluateTime evalu_char;
    EvaluateTime evalu_date;
    EvaluateTime evalu_ip;
    EvaluateTime evalu_discard;
#endif
#include "FormatterController.cpp"
#include "BlockIOManager.cpp"
//#include "RMap.cpp"
//#include "testing.cpp"
//#include "FlexibleInt.cpp"

#define MAX_LOG_SIZE 8192
#define INPUT_MODE FILE_MODE_RAW
RubyInterpreter *ruby;
ConfigInterfaceIN1 *ruby_interface;
InputFormatter *formatter;
//---------------------------------------------------
ShowTime showtime;
#define SHOW_LINE_RANGE 20000
#define SHOW_LINE_COUNT(COUNT) printf("%8d", (COUNT)); showtime.show("","");
inline void showFormatList(){
    FormatList &global_formatList = ruby_interface->global_formatList;
    for(int i = 0, size = global_formatList.size(); i < size; ++i) printf("%d:%s\n", i, typeid(*(global_formatList[i])).name());
}
inline void setInputer(InputManager *inputer){
    FormatList &global_formatList = ruby_interface->global_formatList;
    for(int i = 0, size = global_formatList.size(); i < size; ++i) global_formatList[i]->inputer  = inputer;
}
inline void setOutputer(OutputManager *outputer){
    FormatList &global_formatList = ruby_interface->global_formatList;
    for(int i = 0, size = global_formatList.size(); i < size; ++i) global_formatList[i]->outputer  = outputer;
}
inline void setInOutputer(InputManager *inputer, OutputManager *outputer){
    FormatList &global_formatList = ruby_interface->global_formatList;
    for(int i = 0, size = global_formatList.size(); i < size; ++i){
        global_formatList[i]->inputer  = inputer;
        global_formatList[i]->outputer = outputer;
    }
}
//------------------------------------------------------------
//  first_pass
//------------------------------------------------------------
inline void first_pass(const char *input_path, const char *output_path, const char *input_config, const char *output_config, unsigned int block_size){
    BlockIOManager<OutputManager> *blockoutputer = new BlockIOManager<OutputManager>(output_path, block_size, FILE_MODE_RAW, &setOutputer);
    InputManager *inputer = new InputManager(input_path, INPUT_MODE);
    unsigned int line_count = 0;
    char buffer[MAX_LOG_SIZE];
    SHOW_LINE_COUNT(0);
    while(inputer->readLine(buffer, sizeof(buffer)) != NULL){
        if (buffer[0] == '\0' || buffer[0] == '\n') continue;
        line_count = blockoutputer->getLineCount() + 1;
        #ifdef DEBUG
            printf("%02d: ", line_count);
        #endif
        formatter->execute1(buffer);
        blockoutputer->nextLine();
        #ifdef DEBUG
            puts("");
            if (line_count == DEBUG) break;
        #endif
        if (line_count % SHOW_LINE_RANGE == 0){ SHOW_LINE_COUNT(line_count); }
        //if (line_count > 1000000) break;
    }
    if (line_count % SHOW_LINE_RANGE != 0){ SHOW_LINE_COUNT(line_count); }
    BlockConfig *config = blockoutputer->createConfig();
    ruby_interface->save_config1(output_config, config);
    delete config;
    delete inputer;
    delete blockoutputer;
    #ifdef EVALUATE_TIME
        evalu_int.show("Int");
        evalu_string.show("String");
        evalu_char.show("Char");
        evalu_date.show("Date");
        evalu_ip.show("IP");
        evalu_discard.show("Discard");
    #endif
}
//------------------------------------------------------------
//  second_pass
//------------------------------------------------------------
inline void second_pass(const char *input_path, const char *output_path, const char *input_config, const char *output_config){
    BlockConfig *config = ruby_interface->load_config1(input_config);
    unsigned int line_count = config->line_count;
    unsigned int block_size = config->block_size;
    BlockIOManager<InputManager > *blockinputer  = new BlockIOManager<InputManager >(input_path , block_size, FILE_MODE_RAW, &setInputer );
    #ifdef GROUP_FORMATTER_DATA
        char *output_path2 = (char *) malloc((strlen(output_path) + 1 + 64) * sizeof(char));
        FormatList &global_formatList = ruby_interface->global_formatList;
        for(int i = 0, size = global_formatList.size(); i < size; ++i){
            sprintf(output_path2, "%s_%d_%s", output_path, i, typeid(*(global_formatList[i])).name());
            global_formatList[i]->outputer = new OutputManager(output_path2, FILE_MODE_RAW);
        }
    #else
        BlockIOManager<OutputManager> *blockoutputer = new BlockIOManager<OutputManager>(output_path, block_size, FILE_MODE_RAW, &setOutputer);
    #endif
    SHOW_LINE_COUNT(0);
    for(unsigned int i = 1; i <= line_count; ++i){
        #ifdef DEBUG
            printf("%02d: ", i);
        #endif
        formatter->execute2();
        blockinputer->nextLine();
        #ifndef GROUP_FORMATTER_DATA
            blockoutputer->nextLine();
        #endif
        #ifdef DEBUG
            puts("");
        #endif
        if (i % SHOW_LINE_RANGE == 0){ SHOW_LINE_COUNT(i); }
    }
    if (line_count % SHOW_LINE_RANGE != 0){ SHOW_LINE_COUNT(line_count); }
    ruby_interface->save_config2(output_config, config);
    delete config;
    delete blockinputer;
    #ifdef GROUP_FORMATTER_DATA
        CopyFilesManager *copy_file = new CopyFilesManager(output_path);
        for(int i = 0, size = global_formatList.size(); i < size; ++i){ //merge all files
            delete global_formatList[i]->outputer;
            sprintf(output_path2, "%s_%d_%s", output_path, i, typeid(*(global_formatList[i])).name());
            copy_file->copy(output_path2, false);
        }
        free(output_path2);
    #else
        delete blockoutputer;
    #endif
}
//------------------------------------------------------------
//  third_pass
//------------------------------------------------------------
inline void third_pass(const char *input_path, const char *output_path, const char *input_config, const char *output_config){
    BlockConfig *config = ruby_interface->load_config2(input_config);
    unsigned int line_count = config->line_count;
    unsigned int block_size = config->block_size;
    BlockIOManager<InputManager > *blockinputer  = new BlockIOManager<InputManager >(input_path , block_size, FILE_MODE_RAW, &setInputer );
    SHOW_LINE_COUNT(0);
    for(unsigned int i = 1; i <= line_count; ++i){
        #ifdef DEBUG
            printf("%02d: ", i);
        #endif
        formatter->execute3();
        blockinputer->nextLine();
        #ifdef DEBUG
            puts("");
        #endif
        if (i % SHOW_LINE_RANGE == 0){ SHOW_LINE_COUNT(i); }
    }
    if (line_count % SHOW_LINE_RANGE != 0){ SHOW_LINE_COUNT(line_count); }
    delete config;
    delete blockinputer;
}
//------------------------------------------------------------
//  main
//------------------------------------------------------------
int main(int argc, char **argv){
    BzipManager::loadBz2Library("lib/bzip2-1.0.6/libbz2-1.0.2.DLL");


    /*
    PERROR(1 == 1, printf("read BigInt fail, format = %s, input = %s", "000", ":???"););
    return 0;
    std::cout << bigIntegerToString(BigInteger(BigUnsignedInABase("01Aa", 16)));
    return 0;
    */
    const char *ConfigPath = "data/input.config";
    //const char *InputPath  = "D:/test/iisfw.log.89"; const char *OutputPath = "D:/test/iisfw.log.89.output";
    const char *InputPath  = "data/input_large"; const char *OutputPath = "data/output_min";
    #if INPUT_MODE == FILE_MODE_BZ2
        const char *fileExtension = ".bz2";
    #elif INPUT_MODE == FILE_MODE_RAW
        const char *fileExtension = "";
    #endif
    const int start_pass = 1;
    ruby = new RubyInterpreter();
    switch(start_pass){
    case 1:{
        puts("#==========================================================");
        puts("#  First Pass");
        puts("#==========================================================");
        ruby_interface = new ConfigInterfaceIN1(ruby);
        formatter = ruby_interface->CreateFormatters(ConfigPath, false);
        //return 0;
        char *input_path    = (char *) malloc(sizeof(char) * strlen( InputPath) + 1 + 0 + strlen(fileExtension));
        char *output_path   = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 5);
        char *input_config  = NULL;
        char *output_config = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 7);
        sprintf( input_path  , "%s%s"      ,  InputPath, fileExtension);
        sprintf(output_path  , "%s.temp1"  , OutputPath);
        sprintf(output_config, "%s.config1", OutputPath);
        first_pass(input_path, output_path, input_config, output_config, BLOCK_SIZE);
        free(input_path);
        free(output_path);
        free(input_config);
        free(output_config);
        delete formatter;
        delete ruby_interface;
        }
    case 2:{
        puts("#==========================================================");
        puts("#  Second Pass");
        puts("#==========================================================");
        ruby_interface = new ConfigInterfaceIN1(ruby);
        formatter = ruby_interface->CreateFormatters(ConfigPath, true);
        char *input_path    = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 5);
        char *output_path   = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 5);
        char *input_config  = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 7);
        char *output_config = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 7);
        sprintf(input_path   , "%s.temp1"  , OutputPath);
        sprintf(output_path  , "%s.temp2"  , OutputPath);
        sprintf(input_config , "%s.config1", OutputPath);
        sprintf(output_config, "%s.config2", OutputPath);
        second_pass(input_path, output_path, input_config, output_config);
        free(input_path);
        free(output_path);
        free(input_config);
        free(output_config);
        delete formatter;
        delete ruby_interface;
        #ifdef GROUP_FORMATTER_DATA
            return 0;
        #endif
        }
    case 3:{
        puts("#==========================================================");
        puts("#  Third Pass");
        puts("#==========================================================");
        ruby_interface = new ConfigInterfaceIN1(ruby);
        formatter = ruby_interface->CreateFormatters(ConfigPath, true);
        char *input_path    = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 5);
        char *output_path   = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 5);
        char *input_config  = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 7);
        char *output_config = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 7);
        sprintf(input_path   , "%s.temp2"  , OutputPath);
        sprintf(output_path  , "%s.temp3"  , OutputPath);
        sprintf(input_config , "%s.config2", OutputPath);
        sprintf(output_config, "%s.config3", OutputPath);
        third_pass(input_path, output_path, input_config, output_config);
        free(input_path);
        free(output_path);
        free(input_config);
        free(output_config);
        delete formatter;
        delete ruby_interface;
        }
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
    system("pause");
    BzipManager::freeBz2Library();
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

