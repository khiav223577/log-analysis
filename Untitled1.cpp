#define DEBUG 10
//#define GROUP_FORMATTER_DATA
#define EVALUATE_TIME
//#define TEST_THIRD_PASS
#define BLOCK_SIZE 20000
//---------------------------------------------------
#include<stdio.h>
#include<typeinfo>
#include<iostream>
#include "windows.cpp"
//---------------------------------------------------
#include "lib/ShowTime.cpp"
#include "lib/RIPaddr.cpp"
#ifdef EVALUATE_TIME
    EvaluateTime evalu_int;
    EvaluateTime evalu_string;
    EvaluateTime evalu_char;
    EvaluateTime evalu_date;
    EvaluateTime evalu_ip;
    EvaluateTime evalu_discard;
#endif
#include "FormatterController.cpp"
#include "lib/BlockIOManager.cpp"
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
OutputManager *index_file_outputer = NULL;
inline void showFormatList(){
    FormatList &list = ruby_interface->glist;
    for(int i = 0, size = list.size(); i < size; ++i) printf("%d:%s\n", i, typeid(*(list[i])).name());
}
void setOutputer1(BlockIOManager<OutputManager>& blockoutputer){
    FormatList &list = ruby_interface->glist;
    for(int i = 0, size = list.size(); i < size; ++i) list[i]->outputer = blockoutputer.getIOManager();
}
void setInputer2(BlockIOManager<InputManager>& blockinputer){
    FormatList &list = ruby_interface->glist;
    for(int i = 0, size = list.size(); i < size; ++i) list[i]->inputer = blockinputer.getIOManager();
}
void setOutputer2(BlockIOManager<OutputManager>& blockoutputer){
    FormatList &list = ruby_interface->glist;
    for(int i = 0, size = list.size(); i < size; ++i) list[i]->outputer = blockoutputer.getIOManager();
    if (blockoutputer.getCurrentBlock() != 0){
        for(int i = 0, size = list.size(); i < size; ++i) list[i]->output_block_info(index_file_outputer);
    }
}
void setInputer3(BlockIOManager<InputManager>& blockinputer){
    FormatList &list = ruby_interface->glist;
    for(int i = 0, size = list.size(); i < size; ++i) list[i]->inputer = blockinputer.getIOManager();
}

//------------------------------------------------------------
//  first_pass
//------------------------------------------------------------
inline void first_pass(const char *input_path, const char *output_path, const char *input_config, const char *output_config, unsigned int block_size){
    BlockIOManager<OutputManager> *blockoutputer = new BlockIOManager<OutputManager>(output_path, block_size, FILE_MODE_RAW, &setOutputer1);
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
        blockoutputer->nextLine();
        formatter->execute1(buffer);
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
    BlockIOManager<InputManager> *blockinputer = new BlockIOManager<InputManager>(input_path , block_size, FILE_MODE_RAW, &setInputer2);
    #ifdef GROUP_FORMATTER_DATA
        char *output_path2 = (char *) malloc((strlen(output_path) + 1 + 64) * sizeof(char));
        FormatList &global_formatList = ruby_interface->global_formatList;
        for(int i = 0, size = global_formatList.size(); i < size; ++i){
            sprintf(output_path2, "%s_%d_%s", output_path, i, typeid(*(global_formatList[i])).name());
            global_formatList[i]->outputer = new OutputManager(output_path2, FILE_MODE_RAW);
        }
    #else
        char *output_path2 = (char *) malloc((strlen(output_path) + 1 + 6) * sizeof(char));
        sprintf(output_path2, "%s.index", output_path);
        index_file_outputer = new OutputManager(output_path2, FILE_MODE_RAW);
        free(output_path2);
        BlockIOManager<OutputManager> *blockoutputer = new BlockIOManager<OutputManager>(output_path, block_size, FILE_MODE_RAW, &setOutputer2);
    #endif
    SHOW_LINE_COUNT(0);
    for(unsigned int i = 1; i <= line_count; ++i){
        #ifdef DEBUG
            printf("%02d: ", i);
        #endif
        blockinputer->nextLine();
        #ifndef GROUP_FORMATTER_DATA
            blockoutputer->nextLine();
        #endif
        formatter->execute2();
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
    delete index_file_outputer;
}
//------------------------------------------------------------
//  third_pass
//------------------------------------------------------------
inline void third_pass(const char *input_path, const char *output_path, const char *input_config, const char *output_config){
    BlockConfig *config = ruby_interface->load_config2(input_config);
    unsigned int line_count = config->line_count;
    unsigned int block_size = config->block_size;
    BlockIOManager<InputManager> *blockinputer = new BlockIOManager<InputManager>(input_path , block_size, FILE_MODE_RAW, &setInputer3);
    SHOW_LINE_COUNT(0);
    for(unsigned int i = 1; i <= line_count; ++i){
        #ifdef DEBUG
            printf("%02d: ", i);
        #endif
        blockinputer->nextLine();
        formatter->execute3();
        #ifdef DEBUG
            puts("");
        #endif
        if (i % SHOW_LINE_RANGE == 0){ SHOW_LINE_COUNT(i); }
    }
    if (line_count % SHOW_LINE_RANGE != 0){ SHOW_LINE_COUNT(line_count); }
    delete config;
    delete blockinputer;
}

inline unsigned int *createIpArrayBy(VALUE ruby_array){ //ruby_array = [size_xsum, data_array]
    unsigned int ipsize = FIX2INT(rb_ary_entry(ruby_array, 0));
    unsigned int *ip_array = (unsigned int *) malloc(ipsize * sizeof(unsigned int));
    VALUE data_array = rb_ary_entry(ruby_array, 1);
    for(unsigned int i = 0; i < ipsize; ++i) ip_array[i] = FIX2INT(rb_ary_entry(data_array, i));
    return ip_array;
}
inline unsigned int createDateBy(VALUE ruby_array){ //[year, month, day, hour, minute, second]
    unsigned int year   = FIX2INT(rb_ary_entry(ruby_array, 0));
    unsigned int month  = FIX2INT(rb_ary_entry(ruby_array, 1));
    unsigned int day    = FIX2INT(rb_ary_entry(ruby_array, 2));
    unsigned int hour   = FIX2INT(rb_ary_entry(ruby_array, 3));
    unsigned int minute = FIX2INT(rb_ary_entry(ruby_array, 4));
    unsigned int second = FIX2INT(rb_ary_entry(ruby_array, 5));
    RDate date(year, month, day, hour, minute, second);
    return date.toSecond();
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
    const char *ConfigPath = "data/dd/input_large.config";
    //const char *InputPath  = "D:/test/iisfw.log.89"; const char *OutputPath = "D:/test/iisfw.log.89.output";
    const char *InputPath  = "data/dd/input_large";
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
        char *input_path    = (char *) malloc(sizeof(char) * strlen(InputPath) + 1 + 0 + strlen(fileExtension));
        char *output_path   = (char *) malloc(sizeof(char) * strlen(InputPath) + 1 + 6);
        char *input_config  = NULL;
        char *output_config = (char *) malloc(sizeof(char) * strlen(InputPath) + 1 + 8);
        sprintf( input_path  , "%s%s"      , InputPath, fileExtension);
        sprintf(output_path  , "%s.temp1"  , InputPath);
        sprintf(output_config, "%s.config1", InputPath);
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
        char *input_path    = (char *) malloc(sizeof(char) * strlen(InputPath) + 1 + 6);
        char *output_path   = (char *) malloc(sizeof(char) * strlen(InputPath) + 1 + 0);
        char *input_config  = (char *) malloc(sizeof(char) * strlen(InputPath) + 1 + 8);
        char *output_config = (char *) malloc(sizeof(char) * strlen(InputPath) + 1 + 8);
        sprintf(input_path   , "%s.temp1"  , InputPath);
        sprintf(output_path  , "%s"        , InputPath);
        sprintf(input_config , "%s.config1", InputPath);
        sprintf(output_config, "%s.config2", InputPath);
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
    case 3:{ //測試資料是否正確
        #ifdef TEST_THIRD_PASS
            puts("#==========================================================");
            puts("#  Third Pass");
            puts("#==========================================================");
            ruby_interface = new ConfigInterfaceIN1(ruby);
            formatter = ruby_interface->CreateFormatters(ConfigPath, true);
            char *input_path    = (char *) malloc(sizeof(char) * strlen(InputPath) + 1 + 0);
            char *output_path   = (char *) malloc(sizeof(char) * strlen(InputPath) + 1 + 6);
            char *input_config  = (char *) malloc(sizeof(char) * strlen(InputPath) + 1 + 8);
            //char *output_config = (char *) malloc(sizeof(char) * strlen(InputPath) + 1 + 8);
            sprintf(input_path   , "%s"        , InputPath);
            sprintf(output_path  , "%s.temp3"  , InputPath);
            sprintf(input_config , "%s.config2", InputPath);
            //sprintf(output_config, "%s.config3", InputPath);
            third_pass(input_path, output_path, input_config, NULL);
            free(input_path);
            free(output_path);
            free(input_config);
            //free(output_config);
            delete formatter;
            delete ruby_interface;
        #endif
        }
    }
    if (true){ //Read index
        puts("#==========================================================");
        puts("#  Query");
        puts("#==========================================================");
        showtime.show("before query","");
        ruby_interface = new ConfigInterfaceIN1(ruby);
        formatter = ruby_interface->CreateFormatters(ConfigPath, true);
        //showFormatList();
        char *input_path    = (char *) malloc(sizeof(char) * strlen(InputPath) + 1 + 0);
        char *input_config  = (char *) malloc(sizeof(char) * strlen(InputPath) + 1 + 8);
        char *input_index   = (char *) malloc(sizeof(char) * strlen(InputPath) + 1 + 6);
        sprintf(input_path   , "%s"        , InputPath);
        sprintf(input_config , "%s.config2", InputPath);
        sprintf(input_index  , "%s.index"  , InputPath);
        BlockConfig *config = ruby_interface->load_config2(input_config);
        unsigned int line_count = config->line_count;
        unsigned int block_size = config->block_size;
        unsigned int block_num  = config->get_block_num();

        InputManager *index_file_inputer = new InputManager(input_index, FILE_MODE_RAW);
        InputIndexer *input_indexer = new InputIndexer();
        printf("block_num: %d\n", block_num);
        for(unsigned int i = 0; i < block_num; ++i){
            input_indexer->children.push_back(ruby_interface->CreateIndexers(index_file_inputer));
        }

        free(input_config);
        free(input_index);
        delete index_file_inputer;
        delete config;
        //------------------------------------------------------------------
        VALUE ruby_data = rb_funcall(rb_const("QueryInterface"), rb_intern("wait_query"), 0);
        /*
        RDate date(2013, 12, 3, 4, 0, 1);
        //RIPaddr ip("64.4.11.42");
        RIPaddr ip("140.109.1.10");
        date.show();
        ip.show();
        puts("");*/
        if (ruby_data != Qnil){
            VALUE ip_rb_data1 = rb_ary_entry(ruby_data, 0);
            VALUE ip_rb_data2 = rb_ary_entry(ruby_data, 1);
            unsigned int *ip_array1 = createIpArrayBy(ip_rb_data1), ip_array1_size = FIX2INT(rb_ary_entry(ip_rb_data1, 0));
            unsigned int *ip_array2 = createIpArrayBy(ip_rb_data2), ip_array2_size = FIX2INT(rb_ary_entry(ip_rb_data2, 0));
            unsigned int date1      = createDateBy(rb_ary_entry(ruby_data, 2));
            unsigned int date2      = createDateBy(rb_ary_entry(ruby_data, 3));
            unsigned int time_span  = FIX2INT(rb_ary_entry(ruby_data, 4));
            unsigned int *output_array = (unsigned int *) malloc(sizeof(unsigned int) * time_span);

            //unsigned int a = rb_funcall(rb_const("ConfigReaderInterface"), rb_intern("get_buffer_index"), 1, rb_str_new2("Receive_Time"));

            BlockIOManager<InputManager> *blockinputer = new BlockIOManager<InputManager>(input_path , block_size, FILE_MODE_RAW, &setInputer3);
            SHOW_LINE_COUNT(0);
            unsigned int counter = 1;
            unsigned int must_read = 0;
            while(counter <= line_count){
                if (must_read == 0){
                    unsigned int current_block = blockinputer->getCurrentBlock() + 1;
                    InputIndexer *indexers = input_indexer->children[current_block];
                    bool test1 = indexers->indexList[0]->hasValueBetween(date1, date2);
                    bool test2 = indexers->indexList[1]->hasAnyValueEqualIn((int *) ip_array1, ip_array1_size);
                    bool test3 = indexers->indexList[2]->hasAnyValueEqualIn((int *) ip_array2, ip_array2_size);
                    printf("results: %s %s %s\n", test1 ? "true" : "false", test2 ? "true" : "false", test3 ? "true" : "false");
                    if (test1 && test2 && test3){
                        must_read = block_size;
                        printf("must_read = %d\n", must_read);
                    }else{
                        counter += block_size;
                        blockinputer->skipBlock();
                        printf("Skip block%d: %d\n", current_block, counter);
                    }
                    continue;
                }else must_read -= 1;
                #ifdef DEBUG
                    printf("%02d: ", counter);
                #endif
                blockinputer->nextLine();
                formatter->execute3();
                #ifdef DEBUG
                    puts("");
                #endif
                if (counter % SHOW_LINE_RANGE == 0){ SHOW_LINE_COUNT(counter); }
                counter += 1;
            }
            if (line_count % SHOW_LINE_RANGE != 0){ SHOW_LINE_COUNT(line_count); }






            free(ip_array1);
            free(ip_array2);
            free(output_array);
            delete blockinputer;
        }
        //------------------------------------------------------------------
        delete input_indexer;
        free(input_path);
        showtime.show("end query","");
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
}








