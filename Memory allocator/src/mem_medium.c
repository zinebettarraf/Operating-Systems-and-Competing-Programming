/******************************************************
 * Copyright Grégory Mounié 2018                      *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <stdint.h>
#include <assert.h>
#include "mem.h"
#include "mem_internals.h"


#include <sys/mman.h>
#include <assert.h>
#include <stdint.h>
#include "mem.h"
#include "mem_internals.h"



unsigned int puiss2(unsigned long size) {

    unsigned int p=0;
    size = size -1; // allocation start in 0
    while(size) {  // get the largest bit
	p++;
	size >>= 1;
    }
    if (size > (1 << p))
	p++;
    return p;
}

unsigned long **buddy(unsigned int index){
    if(index >= FIRST_ALLOC_MEDIUM_EXPOSANT + arena.medium_next_exponant)
        mem_realloc_medium();          
    unsigned long **ad = (unsigned long **)arena.TZL[index];
    if(ad != NULL){
        arena.TZL[index]  = * ad;
        return ad; 
    }
    unsigned long **ptr = (unsigned long **)buddy(index +1);
    unsigned long **tmp = (unsigned long **)((long unsigned int )ptr^(1UL<<index));
    *tmp =0 ;
    arena.TZL[index] = tmp;
    return ptr;
}
void *
emalloc_medium(unsigned long size)
{
    assert(size < LARGEALLOC);
    assert(size > SMALLALLOC);

        /* ecrire votre code ici */
    size += 32; //c'est le size du marquage
    unsigned int index  = puiss2(size);
    unsigned long *ptr = (unsigned long *)buddy(index);
    return  mark_memarea_and_get_user_ptr(ptr,size,MEDIUM_KIND);
}

void efree_medium(Alloc a) {
    /* ecrire votre code ici */
/*     unsigned int index = puiss2(a.size);
    unsigned long buddy_adress = (unsigned long)a.ptr ^ a.size;
    if ((unsigned long) arena.TZL[index] == buddy_adress){  
        arena.TZL[index]  = NULL ; 
        arena.TZL[index+1] =  mark_memarea_and_get_user_ptr(a.ptr,2*a.size,MEDIUM_KIND);
    }
    else{
        arena.TZL[index] = a.ptr ;
    } */
}