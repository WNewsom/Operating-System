//
//  Memory420.c
//  Lab02
//
//  Created by Aaron David Block on 1/28/19.
//  Copyright © 2019 Aaron David Block. All rights reserved.
//

#include "ACMemory.h"
#include <assert.h>
#include <stdio.h>

const size_t sizet_size = sizeof(size_t);
const size_t overhead_size = sizeof(void*) + sizet_size;


#define element_size_heap(X) *(size_t*)(X - sizet_size)
#define next_element_heap(X) *(void**)(X - overhead_size)

int initialize_memory(OS_Memory* Omemory, size_t stack_size, size_t heap_size){
    if(!Omemory){
        return 0;
    }
    Omemory->size_of_stack = stack_size;
    Omemory->start_of_stack = malloc(stack_size*overhead_size+overhead_size);
    Omemory->data = &Omemory->size_of_stack;
    *(char*)Omemory->data = 0;
    Omemory->stack_pointer = &Omemory->start_of_stack+overhead_size;
    Omemory->size_of_heap = heap_size;
    Omemory->start_of_heap = malloc(heap_size+sizeof(StorageBlock)); //32 nodes, can add more later
    
    
    
    Omemory->start_of_free_list= Omemory->start_of_heap;
    
    if (heap_size > 0) {
        
        StorageBlock first;
        first.size = (heap_size - sizeof(StorageBlock));
        first.ptr = NULL;
        
    new_StorageNode(Omemory, &first, Omemory->start_of_free_list);
        
    }
    
 
    
    return 1;
}

// Frees the stack
void free_memory(OS_Memory* Omemory){
    
    
    free(Omemory->start_of_stack);
    if(Omemory->size_of_heap != 0){
        free(Omemory->start_of_heap);
    }
    
}


//This will push memeory in bytes to a loction in memory, one byte at a time.
void* push_bytes(OS_Memory* Omemory, void* data, size_t size){
    
    
    byte* databytes = calloc(size, sizeof(byte));
    byte* ptr = (char*)data;
    *ptr = *(char*)data;
    
    int partitions = 1;
    if (size > overhead_size){
        partitions = (int)(size/overhead_size);
        if(size%overhead_size != 0){
            partitions ++;
        }
    }
    *(char*)Omemory->data = partitions;
    Omemory->data ++;
    
    
    for(int i = 0;i < size;i++){
        databytes[i] = (byte)ptr[i];   //the data to be pushed in bytes
        
    }
    
    void* pointer = Omemory->stack_pointer;
    
    /*
     * Puhing the data
     */
    for(int i = 0;i < size;i++){
        *(char*)pointer = ptr[i];
        pointer ++;
        
    }
    pointer = Omemory->stack_pointer;
    
    free(databytes);
    Omemory->stack_pointer = Omemory->stack_pointer +(partitions*overhead_size);
    return pointer;
}

// This will get the bytes from the stack without aloing it to be over written next push.
void* get_bytes(OS_Memory* Omemory){
    
    Omemory->data --;
    int x = overhead_size * (int)*(char*)Omemory->data;
    void* ptr =  Omemory->stack_pointer;
    ptr -= x;
    if(*(char*)Omemory->data == (char)0){
        Omemory->data++;
        return 0;
    }
    Omemory->data ++;
    
    return ptr;
}
//This will return a pointer to the bytes, an allow that space to be written over.
void* pop_bytes(OS_Memory* Omemory){
    
    Omemory->data --;
    int x = overhead_size * (int)*(char*)Omemory->data;
    Omemory->stack_pointer -= x;
    if(*(char*)Omemory->data == (char)0){
        Omemory->data++;
        Omemory->stack_pointer = &Omemory->start_of_stack+overhead_size;
        return 0;
    }
    return Omemory->stack_pointer;
}


/*
 * 1 is allocated 0 is not allocated. byte before that is the size.
 */

void* memory_alloc(OS_Memory* memory, size_t size){
    StorageBlock* head = (StorageBlock*)memory->start_of_free_list;
    *head = * (StorageBlock*)memory->start_of_free_list;
    int sizeREM = (int)memory->size_of_heap;
    
    int block = 0;
    int distfromstart = 0;
//    while (head->ptr != NULL && head->size <= size)  {
//
//        distfromstart+=sizeof(StorageBlock)+head->size;
//        StorageBlock* temp = (StorageBlock*)(memory->start_of_free_list + distfromstart);
//        if (temp != NULL && temp->size != 0) {
//            head->ptr = head+ sizeof(StorageBlock);
//            return head->ptr;
//        }
//
//
//        block++;
//        //printf("The size of the %d block is %zu\n", block,head->size);
//
//        head = (StorageBlock*)(memory->start_of_free_list + distfromstart);
//    }
    //printf("size of the last Block is %d\n", (int)head->size);
    while (head && head->size != 0) {
        distfromstart+=sizeof(StorageBlock)+head->size;
        StorageBlock* temp = (StorageBlock*)(memory->start_of_free_list + distfromstart);
        if ((head->ptr) == NULL && size <= head->size && temp->size == 0) {
            printf("adding node at the end\n");
            head->ptr = head + sizeof(StorageBlock);
            StorageBlock nextBlock;
            
            nextBlock.size = head->size - size - sizeof(StorageBlock);
            
            
            head->size = size;
            
            nextBlock.ptr = NULL;
            new_StorageNode(memory, &nextBlock, head + head->size);
            return head->ptr;
        }
        if(temp != NULL && (head->ptr) == NULL && temp->size != 0 && head->size >= size){
            printf("filling in deallocated node\n");
            head->ptr = head+ sizeof(StorageBlock);
            return head->ptr;
        }
        head = (StorageBlock*)(memory->start_of_free_list + distfromstart);
    
    }
    
//    head->ptr = head + sizeof(StorageBlock);
//    StorageBlock nextBlock;
//
//    nextBlock.size = head->size - size - sizeof(StorageBlock);
//
//
//    head->size = size;
//
//    nextBlock.ptr = NULL;
//    new_StorageNode(memory, &nextBlock, head + head->size);
//
   
    
    
    
    
    
    
//    while (next->data.size <= size && next->data.ptr != NULL) {
//        nodenum++;
//        pointer += next->data.size + sizeof(Node);
//        printf("Size  =  %zu\n", next->data.size);
//        next = next->next;
//
//    }
//    if (next->next->data.ptr) {
//        next->data.ptr = pointer;
//        return next->data.ptr;
//    }

//    printf("Size  =  %zu -- Node # %d\n", next->data.size, nodenum);
//    next->data.ptr = pointer;
//    Node newNode;
//    newNode.data.size = next->data.size  -size;
//    printf("new node's size is = %zu\n", newNode.data.size);
//    newNode.next=NULL;
//    newNode.data.ptr = NULL;
//    new_StorageNode(memory, &newNode, next + size);
//    next->next = (Node*)(next+ size);
//    next->data.size = size;
//



    return head->ptr;
    
}


void memory_dealloc(OS_Memory* memory, void* ptr){
    
    StorageBlock* head =(StorageBlock*)memory->start_of_free_list;
    int distfromstart = 0;
    while (head->size != 0 && head != NULL)  {
        if(head->ptr != NULL){
        if (*(char*)head->ptr == *(char*)ptr) {
            head->ptr = NULL;
            printf("deallocated:\n");
            return;
        }
        }
        
        //printf("The size of the %d block is %zu\n", block,head->size);
        distfromstart+=sizeof(StorageBlock)+head->size;
        head = (StorageBlock*)(memory->start_of_free_list + distfromstart);
        if (distfromstart >= memory->size_of_stack){
            return;
        }
    }

    
}

void new_StorageNode(OS_Memory* memory, StorageBlock* sto, void* location){
    
    StorageBlock testBlock = *(StorageBlock*)sto;
    
    byte* databytes = calloc(((int)sizeof(StorageBlock)), sizeof(byte));
    byte* ptr_dat = (char*)sto;
    *ptr_dat = *(char*)sto;
    
    
    
    for(int i = 0;i < (int)sizeof(StorageBlock); i++){
    
        databytes[i] = (byte)ptr_dat[i];   //the data to be pushed in bytes
    }
    
    for(int i = 0;i < (int)sizeof(StorageBlock); i++){
        
        *(char*)location = (byte)ptr_dat[i];
      
        location ++;
    }
    location = location;
    free(databytes);
    
    printf("A storage node has been added. Location: %p , Size = %zu\n", location , testBlock.size);
}
