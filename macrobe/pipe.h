#ifndef MACROBE_PIPE_H
#define MACROBE_PIPE_H

#include <macrobe/ring_buffer.h>
#include <unistd.h> // fork
#include <sys/wait.h> // waitpid

template <typename Tin, typename Tout>
class Pipe {
public:
    RingBuffer<Tin> *tied;
    RingBuffer<Tout> *buffer;
    
    Pipe() {
        buffer = RingBuffer<Tout>::shared();
        tied = 0;
    }
    
    void tie(RingBuffer<Tin> *rb) {
        tied = rb;
    }
    
    // tie two pipes together
    template <typename T>
    void tie(Pipe<T,Tin> & pipe) {
        tied = pipe.buffer;
    }
    
    // tie two pipes together with |
    template <typename T>
    void operator|(Pipe<Tout,T> & pipe) {
        pipe.tie(*this);
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
                pipe->tied->write(x);
            }
            rb += s;
        }
        pipe->tied->close();
        exit(0);
    }
}

#define yield(expr) _pipe.buffer->write(expr)

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

#define mapT(Tin,Tout,var,expr) \
    Pipe<T,T>::current; \
    Pipe<T,T> & _pipe = *Pipe<T,T>::current; \
    if (!fork()) { \
        while (!_pipe.tied); /* wait until pipe is tied */ \
        while (int s = _pipe.tied->ready()) { \
            for (int i = 0; i < s; i++) { \
                const T var = _pipe.tied->read(); \
                yield(expr); \
            } \
        } \
        _pipe.buffer->close(); \
        exit(0); \
    } \
    *Pipe<T,typeof(expr)>::last = *Pipe<T,typeof(expr)>::current; \
    *Pipe<T,typeof(expr)>::current = new Pipe<T,typeof(expr)>; \
    Pipe<T,typeof(expr)>::last

#endif
