const int MAP = __COUNTER__;
#define map(T, var, expr) registerT(MAP, T, var, expr)

// for multi-line expressions:
const int MAPT = __COUNTER__;
#define mapT(Tin, Tout, var, expr) \
    registerTT(MAPT, Tin, varIn, Tout, varOut, expr)
