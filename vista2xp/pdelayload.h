/* pstdint.h -- portable delayload.h */
#ifndef PDELAYLOAD_H
#define PDELAYLOAD_H

enum DLAttr {
    dlattrRva = 0x1
};

typedef DWORD RVA;
typedef struct {
    DWORD grAttrs;
    RVA rvaDLLName;
    RVA rvaHmod;
    RVA rvaIAT;
    RVA rvaINT;
    RVA rvaBoundIAT;
    RVA rvaUnloadIAT;
    DWORD dwTimeStamp;
} ImgDelayDescr;

#endif  /* ndef PDELAYLOAD_H */
