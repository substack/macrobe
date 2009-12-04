#include <iostream>
#include <vector>
#include <xmmintrin.h>
#include "macrobe.h"

int main() {
    vfloat v(1,2,3,4);
    std::cout << "sizeof(v) = " << sizeof(v) << std::endl;
    std::cout << "sizeof(__m128) = " << sizeof(__m128) << std::endl;
    std::cout << "v = " << v << std::endl;
    v *= 10.0;
    std::cout << "v = " << v << std::endl;
    v += 1000.0;
    std::cout << "v = " << v << std::endl;
    
    vfloat data[25];
    each_n_with_vfloat(data, 25, x, i) { x = i + 10.0; }
    each_n(data, 25, x) {
        std::cout << "x = " << x << std::endl;
    }
    
    std::vector<float> vec(3);
    each_with_index(vec, x, i) { x = 1337 + i; }
    each(vec, x) {
        std::cout << "vec:x=" << x << std::endl;
    }
    
    float ptr[4] = { 3, 4, 5, 6 };
    each_n(ptr, 4, x) { x *= 5; }
    each_n(ptr, 4, x) {
        std::vector<float> v;
        v.push_back(x);
        v.push_back(-x);
        std::cout << "ptr: ";
        each(v, x) {
            std::cout << "x=" << x << " ";
        }
        std::cout << std::endl;
    }
}
