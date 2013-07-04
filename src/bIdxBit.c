#include "bIdxBit.h"
bbyte g_bit_val[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};

int bIdxBit_op(sbit* dest, sbit* src, size_t len, int op)
{
    size_t mod = ((size_t)(BYTE_VAL_8 - 1) & (len));  /* pos % 8 */
    size_t mul = (len) >> 3;                          /* pos / 8 */
    size_t i = 0;
    
    long long* ldest = (long long*)dest;
    long long* lsrc = (long long*)src;
    bbyte* cdest = 0;
    bbyte* csrc = 0;
    if(op == BIT_OP_AND)
    {
        while(mul)
        {
            *ldest &= *lsrc;
            ldest += 1;
            lsrc += 1;
            mul --;
        }

        cdest = (bbyte*)ldest;
        csrc = (bbyte*)lsrc;
        
        while(mod)
        {
            *cdest &= *csrc;
            cdest += 1;
            csrc += 1;
            mod --;
        }
    }
    else if (op == BIT_OP_OR)
    {
        while(mul)
        {
            *ldest |= *lsrc;
            ldest += 1;
            lsrc += 1;
            mul --;
        }

        cdest = (bbyte*)ldest;
        csrc = (bbyte*)lsrc;
        
        while(mod)
        {
            *cdest |= *csrc;
            cdest += 1;
            csrc += 1;
            mod --;
        }        
    }
    else if (op == BIT_OP_XOR)
    {
        while(mul)
        {
            *ldest ^= *lsrc;
            ldest += 1;
            lsrc += 1;
            mul --;
        }

        cdest = (bbyte*)ldest;
        csrc = (bbyte*)lsrc;
        
        while(mod)
        {
            *cdest ^= *csrc;
            cdest += 1;
            csrc += 1;
            mod --;
        }        
    }
    else if (op == BIT_OP_SOR)
    {
        while(mul)
        {
            *ldest ^= *lsrc;
            *ldest = ~ (*ldest);
            ldest += 1;
            lsrc += 1;
            mul --;
        }

        cdest = (bbyte*)ldest;
        csrc = (bbyte*)lsrc;
        
        while(mod)
        {
            *cdest ^= *csrc;
            *cdest = ~ (*cdest);
            cdest += 1;
            csrc += 1;
            mod --;
        }
    }
    else if (op == BIT_OP_NOT_AND)
    {
        while(mul)
        {
            *ldest &= ~(*lsrc);
            ldest += 1;
            lsrc += 1;
            mul --;
        }

        cdest = (bbyte*)ldest;
        csrc = (bbyte*)lsrc;
        
        while(mod)
        {
            *cdest &= ~(*csrc);
            cdest += 1;
            csrc += 1;
            mod --;
        }
    }
    return 1;
}


