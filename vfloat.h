#include <iostream>
#include <vector>
#include <xmmintrin.h>

#define vfloat_update_operator(OP) \
    vfloat & operator OP (const float f) { \
        return *(vfloat *) &(vec OP _mm_set1_ps(f)); \
    } \
    vfloat & operator OP (const __m128 v) { \
        return *(vfloat *) &(vec OP v); \
    }

#define vfloat_operator(OP) \
    vfloat operator OP (const float f) { \
        return vfloat(vec OP _mm_set1_ps(f)); \
    } \
    vfloat operator OP (const __m128 v) { \
        return vfloat(vec OP v); \
    }

struct vfloat {
    __m128 vec;
    
    vfloat() {}
    
    vfloat(__m128 v) {
        vec = v;
    }
    
    vfloat(const float f) {
        vec = _mm_set1_ps(f);
    }
    
    vfloat(const float a, const float b, const float c, const float d) {
        vec = _mm_setr_ps(a, b, c, d);
    }
    
    const float a() {
        float fp[4];
        _mm_store_ps(fp, vec);
        return fp[0];
    }
    
    const float b() {
        float fp[4];
        _mm_store_ps(fp, vec);
        return fp[1];
    }
    
    const float c() {
        float fp[4];
        _mm_store_ps(fp, vec);
        return fp[2];
    }
    
    const float d() {
        float fp[4];
        _mm_store_ps(fp, vec);
        return fp[3];
    }
    
    vfloat_update_operator(*=)
    vfloat_update_operator(/=)
    vfloat_update_operator(+=)
    vfloat_update_operator(-=)
    
    vfloat_operator(*)
    vfloat_operator(/)
    vfloat_operator(+)
    vfloat_operator(-)
};

// super convenient ostream instance
std::ostream & operator<<(std::ostream & out, vfloat v) {
    out << "{" << v.a() << "," << v.b() << "," << v.c() << "," << v.d() << "}";
}

vfloat operator+(const float f, const vfloat v) {
    return vfloat(v.vec + _mm_set1_ps(f));
}
