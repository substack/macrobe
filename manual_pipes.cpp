#include <macrobe/ring_buffer.h>
#include <macrobe/pipe.h>
#include <vector>

#include <ogl/gpgpu.h>
#include <ogl/glsl.cpp>

using std::vector;

int main() {
    RingBuffer<float> *rb = RingBuffer<float>::shared();
    
    float xs[] = { 3, 1, 3, 3, 7 };
    rb->write(5, xs);
    rb->close();
    
    Pipe<float,float> p1, p2, p3;
    
    p1.tie(rb);
    if (!fork()) {
        while (int s = p1.tied->ready()) {
            for (int i = 0; i < s; i++) {
                float x = p1.tied->read();
                std::cout << "p1: " << x << std::endl;
                p1.buffer->write(x);
            }
        }
        p1.buffer->close();
        exit(0);
    }
    
    p2.tie(p1);
    if (!fork()) {
        while (int s = p2.tied->ready()) {
            for (int i = 0; i < s; i++) {
                float x = p2.tied->read();
                std::cout << "p2: " << x << std::endl;
            }
        }
        p2.buffer->close();
        exit(0);
    }
    
    p3.tie(p2);
    if (!fork()) {
        while (int s = p3.tied->ready()) {
            for (int i = 0; i < s; i++) {
                float x = p3.tied->read();
                std::cout << "p3: " << x << std::endl;
            }
        }
        p3.buffer->close();
        exit(0);
    }
    
    return 0;
};
