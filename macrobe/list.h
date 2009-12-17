#ifndef MACROBE_LIST_H
#define MACROBE_LIST_H

#include <macrobe.h>

// hideous trick to create lists without explicit lengths or sentinels:
#define makeList(T, ...) List<T>( \
    sizeof(__va_array(T, __VA_ARGS__)) / sizeof(__va_array(T, __VA_ARGS__)[0]), \
    __va_array(T, __VA_ARGS__) \
)

#define __va_array(T, ...) ((T []){ __VA_ARGS__ })

template <typename T>
class List {
protected:
    T *data;
    size_t filled, capacity;
    
public:
    List() { filled = 0; }
    List(size_t n, T xs[]) { data = xs; filled = capacity = n; }
    
    template <typename Tout>
    void operator|(Pipe<T,Tout> pipe) {
        pipe.spawn();
        
        for (int i = 0; i < filled; i++) {
            std::cout << "data[" << i << "] = " << data[i] << std::endl;
            pipe.write(data[i]);
        }
        
        pipe.close();
    }
    
    T & operator[](int i) {
        return data[i];
    }
};

#endif
