#include <iostream>
#include <cstring>

#define va_length(T, ...) \
    (sizeof va_array(T, __VA_ARGS__)) / (sizeof va_array(T, __VA_ARGS__)[0])

#define va_array(T, ...) ((T []){ __VA_ARGS__ })

#define makeList(T, ...) List<T>( \
    va_length(T, __VA_ARGS__), \
    va_array(T, __VA_ARGS__) \
)

class ListImmutable {};
class ListMutable {};

template <class T>
class List {
    T *data;
    size_t size_;
public:
    typedef T* iterator;
    
    List() {}
    
    List(const List<T> & xs) {
        size_ = xs.size_;
        data = new T[size_];
        memcpy(data, xs.data, size_ * sizeof(T));
    }
    
    List(size_t s, T * xs) {
        data = new T[s];
        memcpy(data, xs, s * sizeof(T));
        size_ = s;
    }
    
    // fill (SSE here mayhaps)
    List(size_t s, T x) {
        data = new T[s];
        size_ = s;
        for (int i = 0; i < s; i++) {
            data[i] = x;
        }
    }
    
    ~List() {}
    
    size_t size() {
        return size_;
    }
    
    void resize(size_t s) {
        size_ = s;
    }
    
    void fill(size_t s, T x) {
        if (s < size_) data = new T[s];
        if (s != size_) resize(s);
        for (int i = 0; i < s; i++) {
            data[i] = x; // SSE here mayhaps
        }
    }
    
    iterator begin() {
        return data;
    }
    
    iterator end() {
        return data + size_;
    }
    
    void operator|(ListImmutable) {
        List<T>::iAnchor = List<T>(*this);
    }
    
    void operator<(ListMutable) {
        List<T>::mAnchor = *this;
    }
    
    void operator>(List & xs) {
        xs = *this;
    }
     
    const T & operator[](size_t index) {
        return data[index];
    }
    
    static List<T> iAnchor;
    static List<T> mAnchor;
};

template <class T> List<T> List<T>::iAnchor;
template <class T> List<T> List<T>::mAnchor;

#define map(T, var, expr) \
    ListImmutable(); \
    for ( \
        List<T>::iterator _macrobe_i = List<T>::iAnchor.begin(), \
            _macrobe_end = List<T>::iAnchor.end(); \
        _macrobe_i != _macrobe_end; ++_macrobe_i \
    ) { \
        const T var = *_macrobe_i; \
        *_macrobe_i = expr; \
    } \
    List<T>::iAnchor

#define mapM(T, var, expr) \
    ListMutable(); \
    for ( \
        List<T>::iterator _macrobe_i = List<T>::mAnchor.begin(), \
            _macrobe_end = List<T>::mAnchor.end(); \
        _macrobe_i != _macrobe_end; ++_macrobe_i \
    ) { \
        T & var = *_macrobe_i; \
        expr; \
    } \
    List<T>::mAnchor

#define tap(T, var, expr) \
    ListImmutable(); \
    for ( \
        List<T>::iterator _macrobe_i = List<T>::iAnchor.begin(), \
            _macrobe_end = List<T>::iAnchor.end(); \
        _macrobe_i != _macrobe_end; ++_macrobe_i \
    ) { \
        const T var = *_macrobe_i; \
        expr; \
    } \
    List<T>::iAnchor

#define tap_with_index(T, var, index, expr) \
    ListImmutable(); \
    for ( \
        int index = 0, _macrobe_size = List<T>::iAnchor.size(); \
        index < _macrobe_size; ++index \
    ) { \
        const T var = List<T>::iAnchor[index]; \
        expr; \
    } \
    List<T>::iAnchor

#define tap_index(T, index, expr) \
    ListImmutable(); \
    for ( \
        int index = 0, _macrobe_size = List<T>::iAnchor.size(); \
        index < _macrobe_size; ++index \
    ) { expr; } \
    List<T>::iAnchor

#define filter(T, var, expr) \
    ListImmutable(); \
    for ( \
        List<T>::iterator \
            _macrobe_m = List<T>::iAnchor.begin(), \
            _macrobe_i = List<T>::iAnchor.begin(), \
            _macrobe_end = List<T>::iAnchor.end(); \
        _macrobe_i != _macrobe_end; ++_macrobe_i \
    ) { \
        const T var = *_macrobe_i; \
        if (expr) { \
            *_macrobe_m = var; \
            ++_macrobe_m; \
        } \
        if (_macrobe_i == _macrobe_end - 1) { \
            List<T>::iAnchor.resize( \
                List<T>::iAnchor.size() + _macrobe_m - _macrobe_i - 1 \
            ); \
        } \
    } \
    List<T>::iAnchor
