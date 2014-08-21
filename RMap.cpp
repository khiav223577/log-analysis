
#ifndef ___RMap_cpp__
#define ___RMap_cpp__

#include <map>
#define MapChar(XXXXX) std::map<char *, XXXXX, char_cmp>
struct char_cmp{
   bool operator()(char const *a, char const *b) const{
      return strcmp(a, b) < 0;
   }
};

template<typename MAP> class RMap{
public:
    typedef typename MAP::mapped_type  VALUE;
    typedef typename MAP::key_type     KEY;
    typedef typename MAP::value_type   PAIR;
    typedef typename MAP::iterator     ITERATOR;
//----------------------------------------------------------------
//  Insert (key, value) into a map if map[key] doesn't exist. And return map[key].
//----------------------------------------------------------------
    inline static VALUE &InsertKeyToMap(MAP &map, KEY const& key, VALUE const& value) {
        return map.insert(PAIR(key, value)).first->second; //insert(XXX).first is the iterator of map[key] which points to std::pair<key, value>
    }
//----------------------------------------------------------------
//  Clear map and free elements.
//----------------------------------------------------------------
    inline static void FreeClearMap_1(MAP & map){
        for(ITERATOR it = map.begin(); it != map.end(); ++it){ free(it->first);  }
        map.clear();
    }
    inline static void FreeClearMap_2(MAP & map){
        for(ITERATOR it = map.begin(); it != map.end(); ++it){ free(it->second); }
        map.clear();
    }
    inline static void FreeClearMap_1_2(MAP & map){
        for(ITERATOR it = map.begin(); it != map.end(); ++it){ free(it->first); free(it->second); }
        map.clear();
    }
};




#endif
