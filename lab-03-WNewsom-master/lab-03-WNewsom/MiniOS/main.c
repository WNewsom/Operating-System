//
//  main.c
//  MiniOS
//
//  Created by Aaron David Block on 1/31/19.
//  Copyright © 2019 Austin College. All rights reserved.
//

#include <stdio.h>
#include "MemoryTests.h"
#include "ACMemory.h"

int main(int argc, const char * argv[]) {
    // insert code here...
   // printf("size of node = %d, size of storage block = %d", sizeof(Node) ,sizeof(StorageBlock));
    
    
    /* Stack Tests */
    basic_stack_test();
    basic_int_test();
    basic_double_test();
    basic_mixed_test();
    basic_struct_test();
    
    
    /* Heap Tests */
    basic_heap_test();
    multisize_heap_test();
    array_heap_test();
    
    printf("All Tests passed\n");
    
    return 0;
}
