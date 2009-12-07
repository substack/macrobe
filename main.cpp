#include "macrobe.h"
#include <iostream>

int main() {
    List<int> xs = makeList(int,10,11,12,13,14);
    xs
        | map(int, x, x + 1)
        | filter(int, x, x % 2 == 0)
        | each(int, x, std::cout << "x=" << x << std::endl);
    
    return 0;
}
