#include <macrobe/ring_buffer.h>
#include <vector>
#include <unistd.h> // fork, usleep
#include <sys/wait.h> // waitpid
using std::vector;

int main() {
    RingBuffer<int> *xb, *yb, *zb;
    xb = RingBuffer<int>::shared();
    yb = RingBuffer<int>::shared();
    zb = RingBuffer<int>::shared();
    
    int xs[] = { 1, 3, 3, 7 };
    
    pid_t pid = fork();
    if (pid == 0) {
        xb->write(4, xs);
        xb->close();
        exit(0);
    }
    
    pid_t xpid = fork();
    if (xpid == 0) {
        while (int s = xb->ready()) {
            for (int i = 0; i < s; i++) {
                int x = xb->read();
                sleep(1);
                std::cout << "x: " << x << std::endl;
                yb->write(x);
            }
        }
        yb->close();
        exit(0);
    }
    
    pid_t ypid = fork();
    if (ypid == 0) {
        while (int s = yb->ready()) {
            for (int i = 0; i < s; i++) {
                int y = yb->read();
                sleep(1);
                std::cout << "y: " << y << std::endl;
                zb->write(y);
            }
        }
        zb->close();
        exit(0);
    }
    
    while (int s = zb->ready()) {
        for (int i = 0; i < s; i++) {
            int z = zb->read();
            std::cout << "z: " << z << std::endl;
        }
    }
    
    return 0;
}
