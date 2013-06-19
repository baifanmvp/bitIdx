#ifndef BIDX_BIT_H
#define BIDX_BIT_H
#include "bIdxDef.h"
#include <stdio.h>
#define BYTE_VAL_8 8
#define BIT_OP_AND 1
#define BIT_OP_OR  2
#define BIT_OP_XOR 3
#define BIT_OP_SOR 4
#define BIT_OP_NOT_AND  5  //取反 再做and

#define  bIdxBit_get_val(ps, pos, val)                                  \
    do                                                                  \
    {                                                                   \
        size_t mod = ((size_t)(BYTE_VAL_8 - 1) & (pos));  /* pos % 8 */ \
        size_t mul = (pos) >> 3; /* pos / 8 */                          \
        byte base = 1;                                                  \
                                                                        \
        base <<= mod;                                                   \
                                                                        \
        base = *((ps) + mul) & base;                                    \
        val = base >> mod;                                              \
    }while(0)

#define bIdxBit_set_val(ps, pos, val)                                   \
    {                                                                   \
        size_t mod = ((size_t)(BYTE_VAL_8 - 1) & pos);  /* pos % 8 */   \
        size_t mul = pos >> 3; /* pos / 8 */                            \
                                                                        \
        byte base = 1;                                                  \
        base <<= mod;                                                   \
        if(val)                                                         \
        {                                                               \
            *(ps + mul) = *(ps + mul) | base;                           \
        }                                                               \
        else                                                            \
        {                                                               \
            *(ps + mul) = *(ps + mul) & ~base;                          \
        }                                                               \
    }




int bIdxBit_op(sbit* dest, sbit* src, size_t len, int op);

#endif
