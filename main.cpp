#include "macrobe.h"
#include <iostream>

int main() {
    List<int> xs = makeList(int,10,11,12,13,14);
    List<int> ys;
    xs
        | map(int, x, x + 1)
        | filter(int, x, x % 2 == 0)
        | each(int, x, std::cout << "x=" << x << std::endl)
        > ys;
    
    ys | each_index(int, i, std::cout
        << "ys[" << i << "] = " << ys[i]
    << std::endl);
    return 0;
}
