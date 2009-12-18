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
    static const size_t extra = 16;
    
public:
    List() { filled = 0; }
    
    List(size_t n, T *xs) {
        data = xs;
        filled = n;
        capacity = n + extra;
    }
    
    List(const List<T> & xs) {
        filled = xs.filled;
        capacity = xs.capacity;
        data = new T[capacity];
        memcpy(data, xs.data, filled);
    }
    
    template <typename Tout>
    void operator|(Pipe<T,Tout> & pipe) {
        RingBuffer<T> *buffer = RingBuffer<T>::shared();
        pipe.open(buffer);
        for (int i = 0; i < filled; i++) {
            std::cout << "data[" << i << "] = " << data[i] << std::endl;
            buffer->write(data[i]);
        }
        pipe.close();
    }
    
    // concatenate two lists with +
    List<T> & operator+(List<T> ys) {
        List<T> xs(*this);
        xs.push(ys);
        return xs;
    }
    
    size_t size() { return filled; }
    
    void empty() {
        filled = 0;
    }
    
    void push(const T & x) {
        if (++filled > capacity) {
            capacity = filled + extra;
            T * new_data = new T[capacity];
            memcpy(new_data, data, filled);
            data = new_data;
        }
        data[filled] = x;
    }
    
    void push(size_t n, const T *xs) {
        filled += n;
        if (filled > capacity) {
            capacity = filled + extra;
            T * new_data = new T[capacity];
            memcpy(new_data, data, filled);
            data = new_data;
        }
        memcpy(data + filled, xs, n);
    }
    
    List<T> & operator<<(const T & x) {
        push(x);
        return *this;
    }
    
    List<T> & operator<<(const List<T> & xs) {
        push(xs);
        return *this;
    }
    
    T & operator[](int i) {
        return data[i];
    }
};

template <typename Tin, typename Tout>
void operator>>(Pipe<Tin,Tout> & pipe, List<Tout> & xs) {
    pipe.open();
    while (int s = pipe.ready()) {
        xs.push(s, pipe.read(s));
    }
    pipe.close();
}

template <typename Tin, typename Tout>
void operator>(Pipe<Tin,Tout> & pipe, List<Tout> & xs) {
    xs.empty();
std::cout << "operator>" << std::endl;
    while (int s = pipe.ready()) {
std::cout << "> s = " << s << std::endl;
        xs.push(s, pipe.read(s));
    }
    pipe.close();
}

#endif
