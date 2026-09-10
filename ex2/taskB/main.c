#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"


int main(){
    /*
    struct Array a = {
        .data = malloc(10 * sizeof(long)),
        .front = 0,
        .back = 0,
        .capacity = 10
    };
    */

    Array a = array_new(3);
    array_print(a);
    for(int i = 0; i < 5; i++){
        array_insertBack(&a, 1);
    };
    array_print(a);

    array_destroy(a);
    
    return 0;
}