#include <macrobe/ring_buffer.h>
#include <macrobe/pipe.h>
#include <vector>
using std::vector;

int main() {
    RingBuffer<float> *rb = RingBuffer<float>::shared();
    
    float xs[] = { 1, 3, 3, 7 };
    rb->write(4, xs);
    rb->close();
    
    static Pipe<float,float> p1, p2;
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
    
    return 0;
}
