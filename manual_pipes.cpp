#include <macrobe/ring_buffer.h>
#include <macrobe/pipe.h>
#include <vector>
#include <sys/wait.h>

#include <ogl/gpgpu.h>
#include <ogl/glsl.cpp>

class ShaderPipe {
// based heavily on http://cs.uaf.edu/2009/fall/cs441/lecture/12_03_GPGPU.html
    gpu_env e;
    int width, height;
    
public:
    gpu_array *src, *dst;
    float *data;
    int filled;
    
    ShaderPipe(int w, int h) {
        data = new float[width * height];
        filled = 0;
    }
    
    bool full() { return filled >= width * height; }
    ShaderPipe & operator<<(float x) {
        data[filled++] = x;
        return *this;
    }
    
    void flush() {
        src = new gpu_array(e, "src", width, height, data, GL_LUMINANCE32F_ARB);
        dst = new gpu_array(e, "dst", width, height, data, GL_LUMINANCE32F_ARB);
        dst->read(data, width, height);
    }
};

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
                p1.buffer->write(x + 1);
            }
        }
        p1.buffer->close();
        exit(0);
    }
    
    p2.tie(p1);
    if (!fork()) {
        ShaderPipe sp(1024,1024);
        while (int s = p2.tied->ready()) {
            for (int i = 0; i < s; i++) {
                float x = p2.tied->read();
                std::cout << "p2: " << x << std::endl;
                sp << x;
                if (sp.full()) {
                    GPU_RUN(*sp.dst,
                         float c = texture2D(src, location).r;
                         gl_FragColor = c * 0.9 + 0.1;
                    );
                    sp.flush();
                    p2.buffer->write(sp.filled, sp.data);
                    sp.filled = 0;
                }
            }
        }
        
        GPU_RUN(*sp.dst,
             float c = texture2D(src, location).r;
             gl_FragColor = c * 0.9 + 0.1;
        );
        sp.flush();
        
        p2.buffer->write(sp.filled, sp.data);
        p2.buffer->close();
        exit(0);
    }
    
    p3.tie(p2);
    if (!fork()) {
        while (int s = p3.tied->ready()) {
            for (int i = 0; i < s; i++) {
                float x = p3.tied->read();
                std::cout << "p3: " << x << std::endl;
                p3.buffer->write(x * 10.0);
            }
        }
        p3.buffer->close();
        exit(0);
    }
    
    return 0;
};
