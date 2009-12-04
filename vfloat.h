#include <iostream>
#include <vector>
#include <xmmintrin.h>

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
    
    inline const float a() {
        float fp[4];
        _mm_store_ps(fp, vec);
        return fp[0];
    }
    
    inline const float b() {
        float fp[4];
        _mm_store_ps(fp, vec);
        return fp[1];
    }
    
    inline const float c() {
        float fp[4];
        _mm_store_ps(fp, vec);
        return fp[2];
    }
    
    inline const float d() {
        float fp[4];
        _mm_store_ps(fp, vec);
        return fp[3];
    }
    
    inline vfloat & operator*=(const float f) {
        return *(vfloat *) &(vec *= _mm_set1_ps(f));
    }
    
    inline vfloat & operator+=(const float f) {
        return *(vfloat *) &(vec += _mm_set1_ps(f));
    }
    
    inline vfloat & operator/=(const float f) {
        return *(vfloat *) &(vec /= _mm_set1_ps(f));
    }
    
    inline vfloat operator+(const float f) {
        return vfloat(vec + _mm_set1_ps(f));
    }
    
    inline vfloat & operator*=(const __m128 v) {
        return *(vfloat *) &(vec *= v);
    }
    
    inline vfloat & operator+=(const __m128 v) {
        return *(vfloat *) &(vec += v);
    }
    
    inline vfloat & operator/=(const __m128 v) {
        return *(vfloat *) &(vec /= v);
    }
    
    /*
    vfloat_operator_m128(*=)
    vfloat_operator_m128(+=)
    vfloat_operator_m128(/=)
    vfloat_operator_m128(|=)
    vfloat_operator_m128(&=)
    vfloat_operator_m128(^=)
    */
};

// super convenient ostream instance
std::ostream & operator<<(std::ostream & out, vfloat v) {
    out << "{" << v.a() << "," << v.b() << "," << v.c() << "," << v.d() << "}";
}
