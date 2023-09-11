#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "windows.h"
int fakemain();
int mark, mark2;
int keys[100];
void* callstack[100];
void (*newenc)(void*, int, const char*);
int sptr=-1;
int getfunc(void* func, int mark){
    int i=0;
    while(1){
        if (*(unsigned char*)(func+i)==0xe8){
            int a=*(int *)(func+i+1);
            a+=(int)func+i;
            if(a+5==mark) break;
        }
        i++;
    }
    return i;
}
int keylookup(void* func){
    int j=0;
    while(keys[j]!=0 && keys[j]!=(int)func) j+=2;
    if(keys[j]==0) keys[j]=(int)func;
    return j;
}
void encfn(void* func, int i, const char* key){
    DWORD a;
    unsigned char* code=func;
    VirtualProtect (code, i, PAGE_READWRITE, &a);
    for(int j=0;j<i;j++) code[j]^=key[j%4];
    VirtualProtect (code, i, PAGE_EXECUTE_READ, &a);
}
void __cyg_profile_func_enter (void *func,  void *caller){
    printf("enter %x, caller %x\n", func, callstack[sptr]);
    int i, j;
    unsigned char key[4];
    if(sptr>-1){
        i=getfunc(callstack[sptr], mark);
        j=keylookup(callstack[sptr]);
        keys[j+1]=rand();
        memcpy(key, &keys[j+1], 4);
        (*newenc)(callstack[sptr], i, key);
    }
    func+=getfunc(func, mark2)+5;
    callstack[++sptr]=func;
    i=getfunc(func, mark);
    j=keylookup(func);
    memcpy(key, &keys[j+1], 4);
    keys[j+1]=rand();
    (*newenc)(func, i, key);
}
void __cyg_profile_func_exit (void *func, void *caller){
    printf("exit %x, back to %x\n", func, callstack[sptr-1]);
    int i, j;
    unsigned char key[4];
    i=getfunc(callstack[sptr], mark);
    j=keylookup(callstack[sptr]);
    memcpy(key, &keys[j+1], 4);
    (*newenc)(callstack[sptr--], i, key);
    if(sptr>-1){
        i=getfunc(callstack[sptr], mark);
        j=keylookup(callstack[sptr]);
        memcpy(key, &keys[j+1], 4);
        (*newenc)(callstack[sptr], i, key);
    }
}

int main() {
    srand(time(NULL));
    memset(keys, 0, 100*sizeof(int));
    memset(callstack, 0, 100*sizeof(void*));
    mark=((int)__cyg_profile_func_exit);
    mark2=((int)__cyg_profile_func_enter);
    newenc = VirtualAlloc(0, 256, MEM_COMMIT, PAGE_READWRITE);
    memcpy(newenc, encfn, 256);
    DWORD a;
    VirtualProtect(newenc, 256, PAGE_EXECUTE_READ, &a);
    printf("%d",fakemain());
    VirtualFree(newenc, 0, MEM_RELEASE);
    return 0;
}
