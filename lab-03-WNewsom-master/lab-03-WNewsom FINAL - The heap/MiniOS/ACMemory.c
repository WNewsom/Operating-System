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
 * This will allocate memory of the wanted size to the heap.
 */

void* memory_alloc(OS_Memory* memory, size_t size){
    StorageBlock* head = (StorageBlock*)memory->start_of_free_list;
    *head = * (StorageBlock*)memory->start_of_free_list;
    StorageBlock* lasblk = NULL;
    /*
     * This will walk us through the free list.
     */
    while (head) {
        if (size <= head->size) {

            /*
             * If this is the last term in the list, then it will create a new storage node with the size remaining
             */
            if(memory->start_of_free_list == head && head->ptr == NULL){

            
            StorageBlock nextBlock;
            
            nextBlock.size = head->size - size - sizeof(StorageBlock);
            
            
            head->size = size;
            
            nextBlock.ptr = head->ptr;
            new_StorageNode(memory, &nextBlock, head + head->size);
            
            head->ptr = (StorageBlock*)(head+head->size);
   
            
                memory->start_of_free_list = head + head->size;
                return head + sizeof(StorageBlock);
            }
            /*
             * If the node isnt the last free node, then all we have to do is take it off of the free list.
             */
            if(head >= (StorageBlock*)memory->start_of_free_list && head->ptr!= NULL){
                printf("given an already created node\n");
                if(lasblk == NULL){
                    memory->start_of_free_list = head->ptr;
                    return head + sizeof(StorageBlock);
                }
                lasblk->ptr = head->ptr;
            return head + sizeof(StorageBlock);
            }
        }
        lasblk = head;
        head = (StorageBlock*)head->ptr;
        
    }
    return 0;
    
}

/*
 * This will dealocate a certain pointer in the linked list, adding it to the free list, then allowing it to be over-written.
 */
void memory_dealloc(OS_Memory* memory, void* ptr){
    
    StorageBlock* head =(StorageBlock*)memory->start_of_heap;
    StorageBlock* prev =NULL;
    printf("%p \n", ptr);
    /*
     * Walking down the whole heap untill we find a pointer that is the same as the one given as a parameter.
     */
    while (head)  {
        if (ptr == head+sizeof(StorageBlock)) {
            StorageBlock* freest =(StorageBlock*)memory->start_of_free_list;
            /*
             * If this is a node before the address of the freelist, then it set that node to be the start of the free list.
             */
            if (prev == NULL && freest > head) {
                head->ptr = freest;
                memory->start_of_free_list = head;
                printf("deallocated:\n");
                return;
            }
            
            /*
             * If the node isnt the start, wee need to find the freelist node before the node so we can add it to the list.
             */
            while (freest < head) {
                freest = freest->ptr;
            }
            /*
             * If the node is already on the free list, then there is nothing to dealocate
             */
            if(freest == head){
                printf("DEALOCATION FAILED\n");
                return;
            }
            head->ptr = prev->ptr;
            prev->ptr = head;
            printf("deallocated:\n");
            return;
        }
        if (head->size <= 0){
            printf("DEALOCATION FAILED\n");
            return;
        }
        prev = head;
        head = (StorageBlock*)head + head->size;
    }
    printf("DEALOCATION FAILED");
}

/*
 * The storage node will hold a pointer to the next free node, and a size of the allocated area. This method will push a storage node in bytes to the location in memory
 */
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
