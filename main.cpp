#include "macrobe.h"
#include <iostream>

int main() {
    //std::cout << list(int,10,11,12,13,14) << std::endl;
    List<int> xs = makeList(int,10,11,12,13,14);
    xs
        | map(int, x, x + 1)
        | map(int, x, std::cout << x << std::endl);
    
    return 0;
}
