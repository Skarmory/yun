#ifndef NAXX_BIT_FLAGS_H
#define NAXX_BIT_FLAGS_H

#define BIT_FLAG(x) (1u << x)

#define SET_FLAG(field, flag) (field |= flag)
#define HAS_FLAG(field, flag) ((field & flag) != 0)

#endif
