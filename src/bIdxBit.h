#ifndef BIDX_BIT_H
#define BIDX_BIT_H
#include "bIdxDef.h"
#include <stdio.h>
#define BYTE_VAL_7 7
#define BYTE_VAL_8 8
#define BIT_OP_AND 1
#define BIT_OP_OR  2
#define BIT_OP_XOR 3
#define BIT_OP_SOR 4
#define BIT_OP_NOT_AND  5  //取反 再做and

#define  bIdxBit_get_val(ps, pos, val)                                  \
    do                                                                  \
    {                                                                   \
      val = *((ps) + (size_t)((pos) >> 3) ) & (1<<(BYTE_VAL_7 & (pos)));	\
    }while(0)

/* #define bIdxBit_set_val(ps, pos, val)                                   \ */
/*     do                                                                  \ */
/*     {                                                                   \ */
/*                                                                         \ */
/*         *(ps + ((pos) >> 3) ) = val ? *(ps + ((pos) >> 3) ) | (1<< ((size_t)(BYTE_VAL_8 - 1) & pos)) : \ */
/*             *(ps + ((pos) >> 3) ) & ~(1<< ((size_t)(BYTE_VAL_8 - 1) & pos)); \ */
/*     }while(0) */


#define bIdxBit_set_val(ps, pos, val)                                   \
    do{                                                                 \
        if(val)                                                         \
        {                                                               \
            *(ps + ((pos) >> 3) ) |= (1 << ((size_t)(BYTE_VAL_7) & pos) ); \
        }                                                               \
        else                                                            \
        {                                                               \
            *(ps + ((pos) >> 3) ) &= ~(1 << ((size_t)(BYTE_VAL_7) & pos) ); \
        }                                                               \
    }while(0)





int bIdxBit_op(sbit* dest, sbit* src, size_t len, int op);

#endif
