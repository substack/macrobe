#ifndef MACROBE_PIPE_H
#define MACROBE_PIPE_H

#include <macrobe/ring_buffer.h>
#include <unistd.h> // fork
#include <sys/wait.h> // waitpid

template <typename Tin, typename Tout>
class Pipe {
    RingBuffer<Tin> *in_buffer;
    RingBuffer<Tout> *out_buffer;
    
public:
    Pipe() {
        out_buffer = RingBuffer<Tout>::shared();
        in_buffer = 0;
    }
    
    // tie two pipes together with |
    template <typename T>
    void operator|(Pipe<Tout,T> & pipe) {
        tie(pipe);
        pipe.in_buffer = out_buffer;
    }
    
    // Block until bytes can be read from the input buffer.
    // Returns the number of bytes
    int ready() {
        int s;
        do s = in_buffer.poll_ready();
        while (s == 0 && out_buffer.closed == false);
        if (out_buffer.closed) return 0;
        return s;
    }
    
    // aliases for buffer routines
    
    // read a single element from the input buffer
    Tin read() {
        return in_buffer.read();
    }
    
    // read n elements from the input buffer
    Tin *read(size_t n) {
        return in_buffer.read(n);
    }
    
    // write a single element to the output buffer
    void write(const Tout & x) {
        out_buffer.write(x);
    }
    
    // write many elements to the output buffer
    void write(size_t n, const Tout * xs) {
        out_buffer.write(n, xs);
    }
    
    // static pipes for macro hack
    static Pipe<Tin,Tout> * current;
    static Pipe<Tin,Tout> * last;
};

template <typename Tin, typename Tout>
Pipe<Tin,Tout> * Pipe<Tin,Tout>::current = new Pipe<Tin,Tout>();

template <typename Tin, typename Tout>
Pipe<Tin,Tout> * Pipe<Tin,Tout>::last = new Pipe<Tin,Tout>();

template <typename Tin, typename Tout>
void operator|(RingBuffer<Tin> & rb, Pipe<Tin,Tout> * pipe) {
    pipe->tie(&rb);
    if (!fork()) {
        while (int s = rb.ready()) {
std::cout << "= s = " << s << std::endl;
            for (int i = 0; i < s; i++) {
                const Tin x = rb.read();
std::cout << "buf x: " << x << std::endl;
                pipe->in_buffer->write(x);
            }
            rb += s;
        }
        pipe->in_buffer->close();
        exit(0);
    }
}

#define yield(expr) _pipe.buffer->write(expr)

#define mapT(Tin,Tout,var,expr) \
    Pipe<Tin,Tout>::current; \
    Pipe<Tin,Tout> & _pipe = *Pipe<Tin,Tout>::current; \
    p1.tie(rb); \
    if (!fork()) { \
        while (int s = p1.tied->ready()) { \
            for (int i = 0; i < s; i++) { \
                float x = p1.tied->read(); \
std::cout << "p1: " << x << std::endl; \
                p1.buffer->write(x + 1); \
            } \
        } \
        p1.buffer->close(); \
        exit(0); \
    } \
    *Pipe<Tin,Tout>::last = *Pipe<Tin,Tout>::current; \
    *Pipe<Tin,Tout>::current = new Pipe<Tin,Tout>; \
    Pipe<Tin,Tout>::last

#define eachT(T,var,expr) \
    Pipe<T,T>::current; \
    if (!fork()) { \
        Pipe<T,T> & _pipe = *Pipe<T,T>::current; \
        while (int s = _pipe.tied->ready()) { \
std::cout << "s = " << s << std::endl; \
            for (int i = 0; i < s; i++) { \
                const T var = _pipe.tied->read(); \
                expr; \
            } \
        } \
std::cout << "close!" << std::endl; \
        _pipe.buffer->close(); \
        exit(0); \
    }

#endif
