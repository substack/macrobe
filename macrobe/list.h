#include <cstring> // memcpy
#include <uninstd.h> // fork
#include <sys/shm.h> // shmget

// hideous trick to create lists without explicit lengths or sentinels:
#define makeList(T, ...) List<T>( \
    (sizeof va_array(T, __VA_ARGS__)) / (sizeof va_array(T, __VA_ARGS__)[0]), \
    ((T []){ __VA_ARGS__ })
)

template <class T>
class List : Readable<T>, RandomlyAccessible<T> {
protected:
    T *data;
    size_t filled, capacity;
    
public:
    List() { filled = 0; }
    List(size_t n, T xs[]) { data = xs; filled = capacity = n }
    
    template <typename Tout>
    
    fork_pipe(T, mem,
        for (size_t i = 0; i < filled; i++) {
            
        }
    );
    
    Pipe<T, Tout> & operator|(Pipe<T, Tout> & p) {
        pid_t pid = fork();
        shmget();
        
        if (pid == 0) {
            exit(0);
        }
        return p;
    }
    
    T & operator[](int i) {
        return data[i];
    }
};
