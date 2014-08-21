
#ifndef ___RMap_cpp__
#define ___RMap_cpp__

#include <map>
#define MapChar(XXXXX) std::map<char *, XXXXX, char_cmp>
struct char_cmp{
   bool operator()(char const *a, char const *b) const{
      return strcmp(a, b) < 0;
   }
};

#endif
