#include <iostream>
#include <cstring>

#define let(...) \
    if (int _let_flip = 1) \
    for (__VA_ARGS__; _let_flip == 1; _let_flip = 0)

#define va_length(T, ...) \
    (sizeof va_array(T, __VA_ARGS__)) / (sizeof va_array(T, __VA_ARGS__)[0])

#define va_array(T, ...) ((T []){ __VA_ARGS__ })

#define makeList(T, ...) List<T>( \
    va_length(T, __VA_ARGS__), \
    va_array(T, __VA_ARGS__) \
)

class ListPipe {};

template <class T>
class List {
    T *data;
    size_t size_;
public:
    typedef T* iterator;
    
    List() {}
    
    List(size_t s, T * xs) {
        data = new T[s];
        memcpy(data, xs, s * sizeof(T));
        size_ = s;
    }
    
    ~List() {}
    
    size_t size() {
        return size_;
    }
    
    iterator begin() {
        return data;
    }
    
    iterator end() {
        return data + size_;
    }
    
    void operator|(ListPipe) {
        List<T>::anchor = List<T>(*this);
    }
    
    void operator>(List & xs) {
        xs = List<T>(*this);
    }
     
    const T & operator[](size_t index) {
        return data[index];
    }
    
    static List<T> anchor;
};

template <class T>
List<T> List<T>::anchor;

#define map(T, var, expr) \
    ListPipe(); \
    for ( \
        List<T>::iterator _imap_i = List<T>::anchor.begin(), \
            _imap_end = List<T>::anchor.end(); \
        _imap_i != _imap_end; ++_imap_i \
    ) { \
        T & var = *_imap_i; \
        var = expr; \
    } \
    List<T>::anchor

#define each(T, var, expr) \
    ListPipe(); \
    for ( \
        List<T>::iterator _imap_i = List<T>::anchor.begin(), \
            _imap_end = List<T>::anchor.end(); \
        _imap_i != _imap_end; ++_imap_i \
    ) { \
        T & var = *_imap_i; \
        expr; \
    } \
    List<T>::anchor
