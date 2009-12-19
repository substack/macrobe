#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/mman.h>
#include <unistd.h> // sleep

template <typename T>
class RingBuffer {
public:
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
        closed = false;
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
        memcpy((void *) rb, (void *) new RingBuffer<T>(), sizeof(RingBuffer<T>));
        return rb;
    }
    
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
        memcpy(xs, buffer, n * sizeof(T));
        *this += n;
        return xs;
    }
    
    const T * read_buffer() {
        return buffer + r_offset;
    }
    
    // write a single element to the buffer
    void write(const T & x) {
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
        if (n <= 0) {}
        else if (n + w_offset > size) {
            // spans boundary, recursivly write
            write(size - w_offset, xs);
            write(n - (size - w_offset), xs + size - w_offset);
        }
        else if (w_offset < r_offset && n + w_offset >= r_offset) {
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
            memcpy(buffer + w_offset, xs, n * sizeof(T));
            w_offset = (w_offset + n) % size;
        }
    }
    
    void close() { closed = true; }
};

#endif
