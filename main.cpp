#include <macrobe.h>
#include <macrobe/list.h>
#include <iostream>

int main() {
    List<int> xs = makeList(int,10,11,12,13,14);
    xs | pipeTT(1, int, int, p,
        while (int s = p.read()) {
            std::cout << "s=" << s << std::endl;
            p += s;
        }
    );
    
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
