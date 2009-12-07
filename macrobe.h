#include <iostream>
#include <cstring>

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
    
    List(const List<T> & xs) {
        size_ = xs.size_;
        memcpy(data, xs.data, size_ * sizeof(T));
    }
    
    List(size_t s, T * xs) {
        data = new T[s];
        memcpy(data, xs, s * sizeof(T));
        size_ = s;
    }
    
    ~List() {}
    
    size_t size() {
        return size_;
    }
    
    void resize(size_t s) {
        size_ = s;
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
        const T var = *_imap_i; \
        expr; \
    } \
    List<T>::anchor

#define filter(T, var, expr) \
    ListPipe(); \
    for ( \
        List<T>::iterator \
            _imap_m = List<T>::anchor.begin(), \
            _imap_i = List<T>::anchor.begin(), \
            _imap_end = List<T>::anchor.end(); \
        _imap_i != _imap_end; ++_imap_i \
    ) { \
        const T var = *_imap_i; \
        if (expr) { \
            *_imap_m = var; \
            ++_imap_m; \
        } \
        if (_imap_i == _imap_end - 1) { \
            List<T>::anchor.resize( \
                List<T>::anchor.size() + _imap_m - _imap_i - 1 \
            ); \
        } \
    } \
    List<T>::anchor
