#include <macrobe/ring_buffer.h>
#include <macrobe/pipe.h>
#include <vector>
using std::vector;

int main() {
    RingBuffer<float> *rb = RingBuffer<float>::shared();
    
    float xs[] = { 2, 4, 4, 8 };
    rb->write(4, xs);
    rb->close();
    //rb | eachT(float, x, std::cout << x << std::endl);
    *rb | new Pipe<float,float>;
    
    /*
    *rb | *Pipe<float,float>::current;
    if (!fork()) {
        Pipe<float,float> & _pipe = *Pipe<float,float>::current;
        while (!_pipe.tied);
        while (int s = _pipe.tied->ready()) {
            for (int i = 0; i < s; i++) {
                const float x = _pipe.tied->read();
                std::cout << x << std::endl;
            }
        }
        x.buffer->close();
        exit(0);
    };
    */
    
    return 0;
}
