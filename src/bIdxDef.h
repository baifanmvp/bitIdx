#ifndef BIDX_DEF_H
#define BIDX_DEF_H
#include <stdio.h>
typedef char baddr;
typedef  int bbool;

typedef char bbyte;
typedef bbyte sbit;

typedef unsigned short bflag_t;

typedef size_t bindex_t;
typedef int  bid_t;
#define TRUE 1
#define FALSE 0


#define BIDX_JSONKEY_op            "op"
#define BIDX_JSONKEY_org           "org"
#define BIDX_JSONKEY_eggTbl        "eggTbl"
#define BIDX_JSONKEY_fieldId       "fieldId"
#define BIDX_JSONKEY_tagId         "tagId"
#define BIDX_JSONKEY_needDoc       "needDoc"
#define BIDX_JSONKEY_needFields    "needFields"
#define BIDX_JSONKEY_matchDocs     "matchDocs"
#define BIDX_JSONKEY_matchFields   "matchFields"
#define BIDX_JSONKEY_matchEql      "matchEql"
#define BIDX_JSONKEY_orderEql      "orderEql"
#define BIDX_JSONKEY_offset        "offset"
#define BIDX_JSONKEY_maxLen        "maxLen"
#define BIDX_JSONKEY_addField      "addField"
#define BIDX_JSONKEY_rmField       "rmField"
#define BIDX_JSONKEY_record        "record"
#define BIDX_JSONKEY_data          "data"
#define BIDX_JSONKEY_code          "code"
#define BIDX_JSONKEY_failTag       "failTag"
#define BIDX_JSONKEY_docId         "docId"
#define BIDX_JSONKEY_total         "total"


#define BIDX_JSONKEY_LABEL         "LABEL"
#define BIDX_JSONKEY_READ          "READ"
#define BIDX_JSONKEY_STAR          "STAR"
#define BIDX_JSONKEY_IMP           "IMP"

#define BIDX_TAGTYPE_LABEL         0x0001
#define BIDX_TAGTYPE_READ          0x0002
#define BIDX_TAGTYPE_STAR          0x0004
#define BIDX_TAGTYPE_IMP           0x0008



#define BIDX_JSONVAL_ADDTAG            "ADDTAG"
#define BIDX_JSONVAL_RMTAG             "RMTAG"
#define BIDX_JSONVAL_QRYID             "QRYID"
#define BIDX_JSONVAL_QRYCND            "QRYCND"
#define BIDX_JSONVAL_UTAGID            "UTAGID"
#define BIDX_JSONVAL_UTAGCND           "UTAGCND"

#define BIDX_JSONVAL_TRUE              1
#define BIDX_JSONVAL_FALSE             0

#define BIDX_JSONVAL_LABEL         "LABEL"
#define BIDX_JSONVAL_READ          "READ"
#define BIDX_JSONVAL_STAR          "STAR"
#define BIDX_JSONVAL_IMP           "IMP"


#define ALLSTR_SIGN "." 
#define dup_all_string(pre, suf, all)                   \
    do                                                  \
    {                                                   \
        int n_pre_len = strlen(pre);                    \
        int n_suf_len = strlen(suf);                    \
        int n_all_len = n_pre_len + n_suf_len + 2;      \
        all = (char*)malloc(n_all_len);                 \
        memcpy(all, pre, n_pre_len);                    \
        memcpy((all) + n_pre_len, ".", 1);              \
        memcpy((all) + n_pre_len + 1, suf, n_suf_len);  \
        (all)[n_all_len - 1] = '\0';                    \
    }while(FALSE)



#define split_all_string(pre, suf, all)                 \
    do                                                  \
    {                                                   \
        bbyte* p = strchr(all, '.');                     \
        pre = all;                                      \
        suf = p + 1;                                    \
        *p = '\0';                                      \
    }while(FALSE)


#endif
