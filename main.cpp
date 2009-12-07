#include "macrobe.h"
#include <iostream>

int main() {
    //List<int> xs = makeList(int,10,11,12,13,14), ys;
    List<int> xs(50000000, 0.0), ys;
    
    // in-place mutable update:
    xs < mapM(float, x, x = x + 1);
    
    // filter 
    xs
        | filter(int, x, x % 2 == 0)
        // | tap(int, x, std::cout << "x=" << x << std::endl)
        > ys; // populate ys from chain results
    
    //ys | tap_with_index(int, y, i,
    //    std::cout << "ys[" << i << "] = " << y << std::endl
    //);
    return 0;
}
