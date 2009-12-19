#ifndef MACROBE_PIPE_H
#define MACROBE_PIPE_H

#include <macrobe/ring_buffer.h>
#include <unistd.h> // fork, usleep
#include <sys/wait.h> // waitpid

template <typename T>
class Pipe {
public:
    RingBuffer<T> *buffer;
    RingBuffer<T> *tied;
    
    Pipe() {
        buffer = RingBuffer<T>::shared();
    }
    
    template <typename Tin>
    void tie(RingBuffer<Tin> *rb) {
        tied = rb;
    }
    
    template <typename Tin>
    void tie(Pipe<Tin> & pipe) {
        tied = pipe.buffer;
    }
};

#endif
