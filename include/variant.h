#ifndef NAXX_VARIANT_H
#define NAXX_VARIANT_H

enum DataType
{
    DATA_TYPE_INT,
    DATA_TYPE_UINT,
    DATA_TYPE_CHAR,
    DATA_TYPE_PTR
};

struct Variant
{
    enum DataType type;
    union
    {
        int          as_int;
        unsigned int as_uint;
        char         as_char;
        void*        as_ptr;
    } data;
};

#endif
