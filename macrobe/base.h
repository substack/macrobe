#ifndef MACROBE_BASE_H
#define MACROBE_BASE_H

#include <iostream>

#include <cstdlib>
#include <cstring>
#include <unistd.h> // fork, usleep
#include <sys/mman.h> // mmap
#include <sys/wait.h> // waitpid
#include <csignal>

template <typename T>
class RingBuffer {
    // same size as Linux kernel's pipe buffer
    static const size_t capacity = 65536;
    static const size_t size = capacity / sizeof(T);
    T buffer[size];
    
    size_t r_offset, w_offset;
    bool closed;
    
    // private constructor since these should be built in shared memory only
    RingBuffer() {
        r_offset = 0;
        w_offset = 0;
    }
    
public:
    // build a ring buffer in shared memory
    static RingBuffer<T> * shared() {
        RingBuffer<T> *rb = (RingBuffer<T> *) mmap(
            NULL, // kernel chooses the address
            sizeof(RingBuffer<T>),
            PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
            -1, 0 // fd, offset not used since there is no file
        );
        if (rb == (RingBuffer<T> *) -1) {
            std::cerr << "Couldn't allocate shared memory" << std::endl;
            exit(1);
        }
        *rb = RingBuffer<T>();
        return rb;
    }
    
    void close() { closed = true; }
    
    // seek r_offset ahead
    void operator++() { r_offset++; }
    void operator+=(size_t n) { r_offset += n; }
    
    // block until there is data to be read from the buffer
    // returns how many elements can be read
    size_t ready() {
        useconds_t t = 1; // short naps increase to the upper bound
        unsigned int i = 0;
        while (r_offset == w_offset) {
            if (closed && r_offset == w_offset) return 0;
            usleep(t); // sleep until more data is written
            if (t < 1024) t *= 2; // ramp up wait time for bits
        }
        return (size + w_offset - r_offset) % size;
    }
    
    T read() {
        T x = buffer[r_offset];
        ++(*this);
        return x;
    }
    
    T * read(size_t n) {
        T *xs = new T[n];
        memcpy(xs, buffer, n);
        *this += n;
        return xs;
    }
    
    const T * read_buffer() {
        return buffer + r_offset;
    }
    
    // write a single element to the buffer
    void write(const T & x) {
        if (closed) return;
        
        useconds_t t = 1; // short naps increase to the upper bound
        while ((w_offset + 1) % size == r_offset) {
            usleep(t); // sleep until more data is read
            if (t < 1024) t *= 2; // ramp up sleep time
        }
        buffer[w_offset] = x;
        w_offset = (w_offset + 1) % size;
    }
    
    // write many elements to the buffer
    void write(size_t n, const T * xs) {
        if (closed) return;
        
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
    
};

template <typename Tin, typename Tout>
class Pipe {
    void *fp; // code that gets executed after pipes are set up
    
public:
    pid_t pid; // public for macro access
    RingBuffer<Tout> * out_buffer;
    RingBuffer<Tin> * in_buffer;
    
    Pipe(void *fp_) {
        fp = fp_;
        pid = 0;
        out_buffer = RingBuffer<Tout>::shared();
    }
    
    // handy aliases to make macros simpler
    size_t ready() { return in_buffer->ready(); }
    Tin * read(size_t n) { return in_buffer->read(n); }
    const Tin * read_buffer() { return in_buffer->read_buffer(); }
    Tin read() { return in_buffer->read(); }
    void write(const Tout & x) { out_buffer->write(x); }
    void write(size_t n, const Tout * xs) { out_buffer->write(n, xs); }
    
    void close() { in_buffer->close(); out_buffer->close(); }
    
    template <typename Tnext>
    Pipe<Tout,Tnext> & operator|(Pipe<Tout,Tnext> & pipe) {
        pipe.open(out_buffer);
    }
    
    Pipe<Tin,Tout> * open(RingBuffer<Tin> * in) {
        in_buffer = in;
        last = this;
        
        pid = fork();
        if (pid == 0) {
            signal(SIGSEGV, _trap_sig);
            goto *fp; // fp must exit(0) when finished
        }
        signal(SIGPIPE, _trap_sig);
        return last;
    }
    
    static Pipe<Tin,Tout> * last; // necessary for macro hack
    
private:
    static void _trap_sig(int sig) {
        if (sig == SIGPIPE) {
            wait(0);
        }
        exit(1);
    }
};

template <typename Tin, typename Tout>
Pipe<Tin,Tout> * Pipe<Tin,Tout>::last; // necessary for macro hack

#define pipeT(unique, T, var, expr) \
    Pipe<T,typeof(expr)>::shared(&& __map_label_#unique); \
    { \
        Pipe<T,typeof(expr)> & var = * Pipe<T,typeof(expr)>::last; \
        __map_label_#unique: \
        if (var.pid == 0) { \
            expr; \
            exit(0); \
        } \
    } \
    * Pipe<T,typeof(expr)>::last

#define pipeTT(unique, Tin, Tout, var, expr) \
    *new Pipe<Tin,Tout>(&&__map_label_ ## unique); \
    { \
        __map_label_ ## unique: \
        Pipe<Tin,Tout> & var = * Pipe<Tin,Tout>::last; \
        if (var.pid == 0) { \
            expr; \
            exit(0); \
        } \
    } \
    * Pipe<Tin,Tout>::last

#endif
