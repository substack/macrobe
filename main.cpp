#include "macrobe.h"
#include <iostream>

int main() {
    List<int> xs = makeList(int,10,11,12,13,14), ys;
    //List<int> xs(50000000, 0.0), ys;
    
    // in-place mutable update:
    xs < mapM(float, x, x = x + 1);
    
    // immutable chaining, just like a unix shell!
    xs
        | filter(int, x, x % 2 == 0)
        | show(int) // print out each of the values in the chain on its own line
        > ys; // redirect chain output into ys
    
    // print out contents of x and y with indices
    
    std::cout << std::endl;
    
    xs | tap_with_index(int, x, i,
        std::cout << "xs[" << i << "] = " << x << std::endl
    );
    
    std::cout << std::endl;
    
    ys | tap_with_index(int, y, i,
        std::cout << "ys[" << i << "] = " << y << std::endl
    );
    return 0;
}
