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
