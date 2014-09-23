//#define DEBUG 10
//#define GROUP_FORMATTER_DATA
#define EVALUATE_TIME
#define BLOCK_SIZE 100000
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

class FilePathManager{
public:
    class PathGroup{
    public:
        char *input_path, *output_path, *input_config, *output_config;
        PathGroup(char *a, char *b, char *c, char *d) :
                input_path(a), output_path(b), input_config(c), output_config(d){
        }
        ~PathGroup(){
            free(input_path);
            free(output_path);
            free(input_config);
            free(output_config);
        }
    };
public:
    PathGroup *pass1, *pass2, *pass3;
    const char *InputPath;
    char *indexPath;
public:
    FilePathManager(const char *path) : InputPath(path){
        #if INPUT_MODE == FILE_MODE_BZ2
            const char *fileExt = ".bz2";
        #elif INPUT_MODE == FILE_MODE_RAW
            const char *fileExt = "";
        #endif
        pass1 = new PathGroup(concat(fileExt) , concat(".temp1"), NULL              , concat(".config1"));
        pass2 = new PathGroup(concat(".temp1"), concat("")      , concat(".config1"), concat(".config2"));
        pass3 = new PathGroup(concat("")      , NULL            , concat(".config2"), NULL);
        indexPath = concat(".index");
    }
    ~FilePathManager(){
        delete pass1;
        delete pass2;
        delete pass3;
        free(indexPath);
    }
    inline char *concat(const char *string){
        const static unsigned int input_len = strlen(InputPath) + 1;
        if (string == NULL) return NULL;
        unsigned int len = (input_len + strlen(string));
        char *output = (char *) malloc(sizeof(char) * len);
        sprintf(output, "%s%s", InputPath, string);
        return output;
    }
};

FilePathManager *filePathMgr = NULL;
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
        index_file_outputer = new OutputManager(filePathMgr->indexPath, FILE_MODE_RAW);
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
            if (i == DEBUG) break;
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
            if (i == DEBUG) break;
        #endif
        if (i % SHOW_LINE_RANGE == 0){ SHOW_LINE_COUNT(i); }
    }
    if (line_count % SHOW_LINE_RANGE != 0){ SHOW_LINE_COUNT(line_count); }
    delete config;
    delete blockinputer;
}

inline unsigned int *createIpArrayBy(VALUE ruby_array){ //ruby_array = [size_xsum, data_array]
    unsigned int ipsize = NUM2UINT(rb_ary_entry(ruby_array, 0));
    unsigned int *ip_array = (unsigned int *) malloc(ipsize * sizeof(unsigned int));
    VALUE data_array = rb_ary_entry(ruby_array, 1);
    for(unsigned int i = 0; i < ipsize; ++i) ip_array[i] = NUM2UINT(rb_ary_entry(data_array, i));
    return ip_array;
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
    const char *action     = ((argc > 1) ? argv[1] : "testing");
    const char *ConfigPath = ((argc > 2) ? argv[2] : "data/dd/input_large.config");
    const char *InputPath  = ((argc > 3) ? argv[2] : "data/dd/input_large"); //"D:/test/data2/iisfw.log.89"
    filePathMgr = new FilePathManager(InputPath);
    int start_pass;
    if (strcmp(action, "query") == 0){
        start_pass = 4;
    }else if (strcmp(action, "compress") == 0){
        //printf("")
        start_pass = 1;
    }else if (strcmp(action, "testing") == 0){
        start_pass = 3;
    }else{
        PERROR(true, printf("argument-1 should be 'query' or 'compress' or 'testing'."));
    }
    ruby = new RubyInterpreter();
    switch(start_pass){
    case 1:{
        puts("#==========================================================");
        puts("#  First Pass");
        puts("#==========================================================");
        ruby_interface = new ConfigInterfaceIN1(ruby);
        formatter = ruby_interface->CreateFormatters(ConfigPath, false);
        FilePathManager::PathGroup *pass = filePathMgr->pass1;
        first_pass(pass->input_path, pass->output_path, pass->input_config, pass->output_config, BLOCK_SIZE);
        delete formatter;
        delete ruby_interface;
        }
    case 2:{
        puts("#==========================================================");
        puts("#  Second Pass");
        puts("#==========================================================");
        ruby_interface = new ConfigInterfaceIN1(ruby);
        formatter = ruby_interface->CreateFormatters(ConfigPath, true);
        FilePathManager::PathGroup *pass = filePathMgr->pass2;
        second_pass(pass->input_path, pass->output_path, pass->input_config, pass->output_config);
        delete formatter;
        delete ruby_interface;
        #ifdef GROUP_FORMATTER_DATA
            return 0;
        #endif
        }
    case 3:{ //測試資料是否正確
        if (start_pass != 3) break;
        puts("#==========================================================");
        puts("#  Third Pass");
        puts("#==========================================================");
        ruby_interface = new ConfigInterfaceIN1(ruby);
        formatter = ruby_interface->CreateFormatters(ConfigPath, true);
        FilePathManager::PathGroup *pass = filePathMgr->pass3;
        third_pass(pass->input_path, pass->output_path, pass->input_config, pass->output_config);
        delete formatter;
        delete ruby_interface;
        }
    }
    if (true){ //Read index
        puts("#==========================================================");
        puts("#  Query");
        puts("#==========================================================");
        ruby_interface = new ConfigInterfaceIN1(ruby);
        //showFormatList();
        formatter = ruby_interface->CreateFormatters(ConfigPath, true);
        FilePathManager::PathGroup *pass = filePathMgr->pass3;
        showtime.show("CreateFormatters","");
        BlockConfig *config = ruby_interface->load_config2(pass->input_config);
        showtime.show("Load config","");
        unsigned int line_count = config->line_count;
        unsigned int block_size = config->block_size;
        unsigned int block_num  = config->get_block_num();
        InputManager *index_file_inputer = new InputManager(filePathMgr->indexPath, FILE_MODE_RAW);
        InputIndexer *input_indexer = new InputIndexer();
        for(unsigned int i = 0; i < block_num; ++i){
            input_indexer->children.push_back(ruby_interface->CreateIndexers(index_file_inputer));
            if ((i + 1) % 20 == 0){ printf("Loading indexes(%d/%d)", i + 1, block_num); showtime.show("","");}
        }
        delete index_file_inputer;
        delete config;
        delete formatter;
        showtime.show("done","");
        //------------------------------------------------------------------

        /*
        RDate date(2013, 12, 3, 4, 0, 1);
        //RIPaddr ip("64.4.11.42");
        RIPaddr ip("140.109.1.10");
        date.show();
        ip.show();
        puts("");*/
        while(1){
            VALUE ruby_data = rb_funcall(rb_const("QueryInterface"), rb_intern("wait_query"), 0);
            if (ruby_data == Qnil) break;
            showtime.reset();
            formatter = ruby_interface->CreateFormatters(ConfigPath, true);
            BlockConfig *config = ruby_interface->load_config2(pass->input_config);
            delete config;
            VALUE ip_rb_data1 = rb_ary_entry(ruby_data, 0);
            VALUE ip_rb_data2 = rb_ary_entry(ruby_data, 1);
            unsigned int *ip_array1  = createIpArrayBy(ip_rb_data1), ip_array1_size = NUM2UINT(rb_ary_entry(ip_rb_data1, 0));
            unsigned int *ip_array2  = createIpArrayBy(ip_rb_data2), ip_array2_size = NUM2UINT(rb_ary_entry(ip_rb_data2, 0));
            unsigned int date1       = NUM2UINT(rb_ary_entry(ruby_data, 2));
            unsigned int date2       = NUM2UINT(rb_ary_entry(ruby_data, 3));
            unsigned int time_span   = NUM2UINT(rb_ary_entry(ruby_data, 4));
            unsigned int buffer_size = NUM2UINT(rb_ary_entry(ruby_data, 5));
            unsigned int *buffer_bytes_sent      = (unsigned int *) malloc(buffer_size * sizeof(unsigned int));
            unsigned int *buffer_bytes_rece      = (unsigned int *) malloc(buffer_size * sizeof(unsigned int));
            FormatterController *fc_source_ip    = ruby_interface->getFormatterByName("Source_address");
            FormatterController *fc_destin_ip    = ruby_interface->getFormatterByName("Destination_address");
            FormatterController *fc_receive_time = ruby_interface->getFormatterByName("Receive_Time");
            FormatterController *fc_bytes_sent   = ruby_interface->getFormatterByName("Bytes_Sent");
            FormatterController *fc_bytes_rece   = ruby_interface->getFormatterByName("Bytes_Received");
            memset(buffer_bytes_sent, 0, buffer_size * sizeof(unsigned int));
            memset(buffer_bytes_rece, 0, buffer_size * sizeof(unsigned int));
            int executeCounter, preExecuteCounter = 0;

            BlockIOManager<InputManager> *blockinputer = new BlockIOManager<InputManager>(pass->input_path , block_size, FILE_MODE_RAW, &setInputer3);
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
                        //printf("must_read = %d\n", must_read);
                    }else{
                        counter += block_size;
                        blockinputer->skipBlock();
                        //printf("Skip block%d: %d\n", current_block, counter);
                    }
                    continue;
                }else must_read -= 1;
                #ifdef DEBUG
                    printf("%02d: ", counter);
                #endif
                blockinputer->nextLine();
                formatter->execute3();
                while(1){
                    if ((executeCounter = fc_bytes_sent->getExecuteCounter()) != preExecuteCounter){
                        preExecuteCounter = executeCounter;
                        unsigned int date = (unsigned int) fc_receive_time->get_prev_int();
                        unsigned int sIP  = (unsigned int) fc_source_ip->get_prev_int();
                        unsigned int dIP  = (unsigned int) fc_destin_ip->get_prev_int();
                        if (date < date1 || date > date2) break;
                        unsigned int idx;
                        for(idx = 0; idx < ip_array1_size; ++idx) if (ip_array1[idx] == sIP) break;
                        if (idx == ip_array1_size) break;
                        for(idx = 0; idx < ip_array2_size; ++idx) if (ip_array2[idx] == dIP) break;
                        if (idx == ip_array2_size) break;
                        unsigned int site = (date - date1) / time_span; //
                        PERROR(site >= buffer_size, printf("Unexpected error. %d > %d", site, buffer_size));
                        buffer_bytes_sent[site] += fc_bytes_sent->get_prev_int();
                        buffer_bytes_rece[site] += fc_bytes_rece->get_prev_int();
                    }
                    break;
                }
                #ifdef DEBUG
                    puts("");
                #endif
                if (counter % SHOW_LINE_RANGE == 0){ SHOW_LINE_COUNT(counter); }
                counter += 1;
            }
            if (line_count % SHOW_LINE_RANGE != 0){ SHOW_LINE_COUNT(line_count); }

            VALUE line1 = rb_ary_new();
            VALUE line2 = rb_ary_new();
            for(unsigned int i = 0; i < buffer_size; ++i){
                printf("%d: %d %d\n", i, buffer_bytes_sent[i], buffer_bytes_rece[i]);
                rb_ary_push(line1, UINT2NUM(buffer_bytes_sent[i]));
                rb_ary_push(line2, UINT2NUM(buffer_bytes_rece[i]));
            }
            rb_funcall(rb_const("GraphInterface"), rb_intern("lineChart"), 5, UINT2NUM(date1), UINT2NUM(time_span), UINT2NUM(buffer_size), line1, line2);


            free(ip_array1);
            free(ip_array2);
            free(buffer_bytes_sent);
            free(buffer_bytes_rece);
            delete blockinputer;
            delete formatter;
        }
        //------------------------------------------------------------------
        delete input_indexer;
        delete ruby_interface;
    }

    delete ruby;
    delete filePathMgr;
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
    //system("pause");
    BzipManager::freeBz2Library();
    return 0;
}








