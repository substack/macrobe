#ifndef MACROBE_BASE_H
#define MACROBE_BASE_H

#include <iostream>

#include <cstdlib>
#include <cstring>
#include <unistd.h> // fork, usleep
#include <sys/mman.h> // mmap
#include <sys/wait.h> // waitpid
#include <csignal>

#include <macrobe/ring_buffer.h>

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
    void operator++() { ++(*in_buffer); }
    void operator+=(size_t n) { (*in_buffer) += n; }
    void close() {
        out_buffer->close();
    }
    
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
