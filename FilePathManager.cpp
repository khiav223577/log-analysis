
#ifndef ___FilePathManager_cpp__
#define ___FilePathManager_cpp__
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
    char *indexPath, *blockInfoPath1, *blockInfoPath2;
public:
    FilePathManager(const char *path) : InputPath(path){
        pass1 = new PathGroup(concat("")      , concat(".temp1"), NULL              , concat(".config1"));
        pass2 = new PathGroup(concat(".temp1"), concat("")      , concat(".config1"), concat(".config2"));
        pass3 = new PathGroup(concat("")      , NULL            , concat(".config2"), NULL);
        indexPath = concat(".index");
        blockInfoPath1 = concat(".part.info1");
        blockInfoPath2 = concat(".part.info2");
    }
    ~FilePathManager(){
        delete pass1;
        delete pass2;
        delete pass3;
        free(indexPath);
        free(blockInfoPath1);
        free(blockInfoPath2);
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

#endif


