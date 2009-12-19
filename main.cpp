#include <macrobe.h>
//#include <macrobe/list.h>
#include <macrobe/vector.h>
#include <iostream>
using std::vector;

int main() {
    //vector<int> xs = makeVector(int,10,11,12,13,14), ys;
    vector<int> xs, ys;
    xs.push_back(1);
    xs.push_back(3);
    xs.push_back(3);
    xs.push_back(7);
std::cout << xs.size() << std::endl;
    
    xs | pipeTT(map, int, int, pipe,
std::cout << "pipe" << std::endl;
        while (int s = pipe.ready()) {
std::cout << "| s = " << s << std::endl;
            for (int i = 0; i < s; i++) {
                int x = pipe.read();
                pipe.write(x);
            }
        }
        std::cout << "done" << std::endl;
        pipe.close();
    ) > ys;
    
    for (int i = 0; i < ys.size(); i++)
        std::cout << "y=" << ys[i] << std::endl;
    
    //List<int> xs(50000000, 0.0), ys;
    
    /*
    // in-place mutable update:
    xs <= map(int, x, x + 1);
    
    // immutable chaining, just like a unix shell!
    xs
        | filter(int, x, x % 2 == 0)
        | show(int) // print out each of the values in the chain on its own line
        > ys; // redirect chain output into ys
    
    // print out contents of x and y with indices
    
    std::cout << std::endl;
    xs | tee(int, x, std::cout << x << std::endl);
    
    std::cout << std::endl;
    ys | tee(int, y, std::cout << y << std::endl);
    */
    
    return 0;
}
