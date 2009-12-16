#include <cstring>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

template <typename Tin, typename Tout>
class Pipe {
    void *fn;
    
protected:
    size_t & r_offset, & w_offset, size;
    pid_t pid;
    
public:
    Tout *buffer;
    static const size_t capacity = 65536; // same as the Linux kernel
    
    Pipe(void *fn_) {
        fn = fn_;
        size = capacity / sizeof(Tout);
        
        void *ptr = mmap(
            NULL, // kernel chooses the address
            2 * sizeof(size_t) + capacity, // offsets plus data buffer
            PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
            -1, 0 // fd, offset not used since there is no file
        );
        buffer = ptr + 2 * sizeof(size_t);
        r_offset = *((size_t *) ptr);
        w_offset = *((size_t *) ptr + sizeof(size_t));
    }
    
    // block until there is data to be read from the buffer
    // returns how many elements can be read
    size_t read() {
        useconds_t t = 1; // short naps increase to the upper bound
        unsigned int i = 0;
        while (r_offset == w_offset) {
            usleep(t); // sleep until more data is written
            if (t < 1024) t *= 2; // ramp up wait time for bits
            // occasionally make sure that the worker process is still alive
            if (i++ % 10 == 0 && waitpid(pid, 0, WNOHANG) <= 0) {
                // process terminated, unmap memory and return 0
                size_t h = 2 * sizeof(size_t);
                munmap((void *) (buffer - h), capacity + h);
                return 0;
            }
        }
        return (size + r_offset - w_offset) % size;
    }
    
    // write a single element to the buffer
    void write(const Tout & x) {
        useconds_t t = 1; // short naps increase to the upper bound
        while ((w_offset + 1) % size != r_offset) {
            usleep(t); // sleep until more data is read
            if (t < 1024) t *= 2; // ramp up sleep time
        }
        buffer[w_offset] = x;
        w_offset = (w_offset + 1) % size;
    }
    
    // write many elements to the buffer
    void write(size_t n, const Tout * xs) {
        if (n + w_offset > size) {
            // spans boundary, recursivly write
            write(size - w_offset, xs);
            write(n - (size - w_offset), xs + size - w_offset);
        }
        else if (n + w_offset >= r_offset) {
            // spans read offset
            size_t r = r_offset;
            write(r - w_offset, xs);
            
            useconds_t t = 1;
            while (r_offset == r) {
                usleep(t); // sleep until more data is read
                if (t < 1024) t *= 2; // ramp up sleep time
            }
            // recursively write more data
            write(n - (r - w_offset), xs);
        }
        else {
            memcpy(buffer + w_offset, xs, n);
            w_offset = (w_offset + n) % size;
        }
    }
    
    template <typename Tnext>
    Pipe<Tout, Tnext> & operator|(Pipe<Tout,Tnext> & pipe) {
        pid = fork();
        if (pid == 0) fn(); // fn should exit(0) when finished
        return pipe;
    }
};

template <typename T>
class Readable {
    template <typename Tout>
    Pipe<T,Tout> & operator|(Pipe<T,Tout>);
};

template <typename T> class Writable;
template <typename Tin, typename Tout>
Writable<Tout> & operator>(Pipe<Tin,Tout>, Writable<Tout>);

template <typename T> class Appendable;
template <typename Tin, typename Tout>
Appendable<Tout> & operator>>(Pipe<Tin,Tout>, Appendable<Tout>);

#define pipeT(unique, T, var, expr) \
    PipeTransform<T, typeof(expr)>(&&__map_label_#unique); \
    { \
        const PipeTransform<T, typeof(expr)> pt; \
        if (0) { \
            const T & var = *pt.in; \
            __map_label_#unique: *pt.out = expr; \
            exit(0); \
        } \
    } \
    PipeTransform<T, typeof(expr)>::last

#define pipeTT(unique, Tin, varIn, Tout, varOut, expr) \
    & PipeTransform<Tin, Tout>(&&__map_label_#unique); \
    { \
        const PipeTransform<Tin, Tout> pt; \
        if (0) { \
            const Tin & varIn = *pt.in; \
            Tout & varOut = *pt.out; \
            __map_label_#unique: expr; \
            goto * pt.jump; \
        } \
    } \
    PipeTransform<Tin, Tout>::last
