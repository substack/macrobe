#ifndef MACROBE_VECTOR_H
#define MACROBE_VECTOR_H

#include <macrobe.h>
#include <vector>

// hideous trick to create lists without explicit lengths or sentinels:
#define makeVector(T, ...) std::vector<T>( \
    __va_array(T, __VA_ARGS__), __va_array(T, __VA_ARGS__) + \
    sizeof(__va_array(T, __VA_ARGS__)) / sizeof(__va_array(T, __VA_ARGS__)[0]) \
)
#define __va_array(T, ...) ((T []){ __VA_ARGS__ })

template <typename Tin, typename Tout>
void operator|(std::vector<Tin> & xs, Pipe<Tin,Tout> & pipe) {
    RingBuffer<Tin> *buffer = RingBuffer<Tin>::shared();
    pipe.open(buffer);
    typename std::vector<Tin>::const_iterator iter, end;
    for (iter = xs.begin(), end = xs.end(); iter != end; iter++) {
        buffer->write(*iter);
    }
    pipe.close();
}
    
template <typename Tin, typename Tout>
void operator>>(Pipe<Tin,Tout> & pipe, std::vector<Tout> & xs) {
    pipe.open();
    while (int s = pipe.ready()) {
        xs.insert(xs.back(), pipe.read_buffer(), pipe.read_buffer() + s);
        pipe += s;
    }
    pipe.close();
}

template <typename Tin, typename Tout>
void operator>(Pipe<Tin,Tout> & pipe, std::vector<Tout> & xs) {
    RingBuffer<Tout> *rb = RingBuffer<Tout>::shared();
    pipe.open(rb);
    
    xs.empty();
    while (int s = pipe.ready()) {
std::cout << "> s = " << s << std::endl;
        xs.insert(
            xs.end(),
            pipe.read_buffer(),
            pipe.read_buffer() + s
        );
        pipe += s;
    }
std::cout << "> close" << std::endl;
    pipe.close();
}

#endif
