namespace Expression {
    enum Op {
        VAL_INT8,
        VAL_INT16,
        VAL_FLOAT,
        VAL_VARIABLE,
        VAL_PREVIOUS,

        OP_EQ,
        OP_NE,
        OP_LT,
        OP_GT,
        OP_LE,
        OP_GE,

        OP_NOT,
        OP_OR,
        OP_AND,

        OP_ADD,
        OP_SUB,
        OP_MULT,
        OP_DIV,
        OP_NEG,

        OP_IN,
        OP_IFELSE,
        OP_BETWEEN,
    };
}

/* Helper macros for inline expressions */

#define DEF_EXPR(expr) ((uint8_t[]) { expr })
#define DEF_INT8(val) VAL_INT8, (val)
#define DEF_INT16(val) VAL_INT16, \
    (uint16_t) (val) >> 8, \
    (uint16_t) (val) >> 0
#define DEF_FLOAT(val) VAL_FLOAT, \
    ((uint8_t *) (float[]) { val })[3], \
    ((uint8_t *) (float[]) { val })[2], \
    ((uint8_t *) (float[]) { val })[1], \
    ((uint8_t *) (float[]) { val })[0],
#define DEF_CURRENT(svcId, type, num) VAL_VARIABLE, svcId, type, num
#define DEF_PREVIOUS(svcId, type, num) VAL_PREVIOUS, svcId, type, num

#define DEF_UNARY(op, subexpr0) op, subexpr0
#define DEF_BINARY(op, subexpr0, subexpr1) op, subexpr0, subexpr1
#define DEF_TRINARY(op, subexpr0, subexpr1, subexpr2) \
    op, subexpr0, subexpr1, subexpr2

#define DEF_IN(num, params) OP_IN, num, params
