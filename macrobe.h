#include "vfloat.h"

#define let(expr) \
    if (int _let_flip = 1) \
    for (expr; _let_flip == 1; _let_flip = 0)

#define each_n(_each_obj, n, var) \
    for (size_t _each_i = 0; _each_i < n; _each_i++) \
    let(typeof(_each_obj[0]) & var = _each_obj[_each_i])

#define map(_map_obj, var) \
    let(size_t _map_size = _map_obj.size()) \
    for (size_t _map_i = 0; _map_i < _map_size; _map_i++) \
    let(typeof(_map_obj[0]) & var = _map_obj[_map_i])

#define each(_each_obj, var) \
    let(size_t _each_size = _each_obj.size()) \
    for (size_t _each_i = 0; _each_i < _each_size; _each_i++) \
    let(typeof(_each_obj[0]) & var = _each_obj[_each_i])

#define each_with_index(_each_obj, var, iter) \
    let(size_t _each_size = _each_obj.size()) \
    for (size_t iter = 0; iter < _each_size; iter++) \
    let(typeof(_each_obj[0]) & var = _each_obj[iter])

#define each_n_with_index(_each_obj, n, var, iter) \
    for (size_t iter = 0; iter < n; iter++) \
    let(typeof(_each_obj[0]) & var = _each_obj[iter])

#define each_n_with_vfloat(_each_obj, n, var, vf) \
    let (vfloat vf = vfloat(0,1,2,3)) \
    for (size_t _each_i = 0; _each_i < n; vf += 4, _each_i++) \
    let (typeof(_each_obj[0]) & var = _each_obj[_each_i])
