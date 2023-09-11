//demo file
#include <stdio.h>
#include "windows.h"
int bar(){
    printf("bar");
    return 69;
}
void foo(int depth) {
    MessageBoxA(NULL, "foo", "foo", MB_OK);
    if(depth==0){
        printf("%d",bar());
        return;
    }
    foo(depth-1);
}

int fakemain(){
    foo(3);
    return 420;
}
