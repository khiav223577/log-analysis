
// very basic embedded Ruby interpreter for C++ programs
// thrown together by Jeff Trull <jetrull@sbcglobal.net> based on googling and asking questions on #ruby-lang
#include "RubyInterpreter.cpp"

int main(int argc, char **argv){
    RubyInterpreter ruby;
    ruby.execute_code("$IN_C_CODE = true");
    ruby.execute_file("./test.rb");


    rb_funcall(rb_gv_get("$!"), rb_intern("register_hash"),  2, rb_str_new2("config2"), Qnil);
    rb_funcall(rb_gv_get("$!"), rb_intern("register_hash"),  2, Qtrue, Qfalse);
return 0;
    rb_funcall(rb_gv_get("$!"), rb_intern("read_config"),  1, rb_str_new2("config"));
    VALUE result = rb_funcall(rb_gv_get("$!"), rb_intern("return_string"), 0);
return 0;
    VALUE test1 = rb_ary_entry(result,0);
    printf("%s\n",StringValuePtr(test1));
    VALUE test2 = rb_ary_entry(result,1);
    printf("%s\n",StringValuePtr(test2));
return 0;
/*
    VALUE rbError = rb_funcall(rb_gv_get("$!"), rb_intern("return_string"), 0);
    printf("%s\n",StringValuePtr(rbError));
    rbError = rb_funcall(rb_gv_get("$!"), rb_intern("return_string"), 0);
    printf("%s\n",StringValuePtr(rbError));
    rbError = rb_funcall(rb_gv_get("$!"), rb_intern("return_string"), 0);
    printf("%s\n",StringValuePtr(rbError));*/
    system("pause");
    return 1;
}
