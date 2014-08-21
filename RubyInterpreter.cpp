

#ifndef ___RubyInterpreter_cpp__
#define ___RubyInterpreter_cpp__
#include <ruby.h>
RUBY_GLOBAL_SETUP
class RubyInterpreter{
public:
    bool had_execed;
    char **ini_argv;
    int error_status, ini_argc;
    RubyInterpreter() : had_execed(false){
        ruby_sysinit(&ini_argc, &ini_argv);
        //VALUE variable_in_this_stack_frame;
        //ruby_init_stack(&variable_in_this_stack_frame);  //
        RUBY_INIT_STACK;
        ruby_init();
        //ruby_init_loadpath();
        rb_set_safe_level(0);
        ruby_script("ruby"); //設定腳本名字。若不設定，在使用到__FILE__ 的地方就會出錯。
    }
    ~RubyInterpreter(){
        if (!had_execed) ruby_finalize();
    }
    void error_handling(){
        if (error_status){
            VALUE rbError = rb_funcall(rb_gv_get("$!"), rb_intern("message"), 0);
            printf("%s\n",StringValuePtr(rbError));
        };
    }
//--------------------------------------------------------
//  exec
//--------------------------------------------------------
    void exec(const char *path){
        const char *argv[] = {path};
        run_node(1, argv);
    }
    void exec_irb(){
        const char *argv[] = {"-e", "require \"irb\"; IRB.start"};
        run_node(2, argv);
    }
    void run_node(int argc, const char **argv){
        if (had_execed == true){
            printf("This interpreter had execed... interrupt!");
            return;
        }
        had_execed = true;
        int new_argc = ini_argc + argc;
        const char **new_argv = (const char**) malloc(new_argc * sizeof(char*));
        for(int i = 0; i < ini_argc; ++i) new_argv[i]            = ini_argv[i];
        for(int i = 0; i <     argc; ++i) new_argv[ini_argc + i] =     argv[i];
        ruby_run_node(ruby_options(new_argc, (char **) new_argv));
        free(new_argv);

    }
//--------------------------------------------------------
//  Run code
//--------------------------------------------------------
    void execute_code(const char *code){
        rb_eval_string_protect(code, &error_status);
        error_handling();
    }
    void execute_file(const char *path){ //will not exit C program when ruby raise RuntimeError.
        rb_load_protect(rb_str_new2(path), 0, &error_status);
        error_handling();
    }
};



#endif


