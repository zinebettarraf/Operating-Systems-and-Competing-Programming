/******************************************************
 * Copyright Grégory Mounié 2018                      *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <sys/mman.h>
#include <assert.h>
#include <stdint.h>
#include "mem.h"
#include "mem_internals.h"

unsigned long knuth_mmix_one_round(unsigned long in)
{
    return in * 6364136223846793005UL % 1442695040888963407UL;
}

void *mark_memarea_and_get_user_ptr(void *ptr, unsigned long size, MemKind k)
{
    /* ecrire votre code ici */    /* the given size is in bytes */
 
    unsigned long magicNbr = knuth_mmix_one_round((unsigned long)ptr) ;
    // modifying the last two bits to mark the type of allocation
    magicNbr=magicNbr & ~ (0b11UL);  // set the two last bits to zero
    magicNbr=magicNbr | k ; // fusion with the memKind

    unsigned long* pointer_8o= (unsigned long*)ptr; 
    char* pointer_1o= (char*)ptr;   

    *(pointer_8o)=size;
    *(pointer_8o+1)=magicNbr;
    *(unsigned long*)(pointer_1o+size-8)=size;
    *(unsigned long*)(pointer_1o+size-16)=magicNbr;

    return ((unsigned long*)ptr)+2;

}

Alloc
mark_check_and_get_alloc(void *ptr)
{
    /* ecrire votre code ici */
    unsigned long* pointer_8o= (unsigned long*)ptr; 
    char* pointer_1o= (char*)ptr;
    
    unsigned long magicNbr = *(pointer_8o - 1) ;
    MemKind kind= magicNbr &  0b11UL ;
    unsigned long size = *(pointer_8o - 2) ;
    void* starting_adress=(void *)(pointer_8o - 2);
    
    // assert size 
    unsigned long sizeEnd = *(unsigned long*)(pointer_1o + size -8-16);
    assert(sizeEnd == size);

    // assert magic number 
    unsigned long magicNbrEnd = *(unsigned long*)(pointer_1o + size -16-16);
    assert(magicNbrEnd == magicNbr);
    
    // is the magicNbr corect
    unsigned long expectedMagicNbr= (knuth_mmix_one_round((unsigned long)starting_adress)  & ~ (0b11UL) )| kind ; 
    assert(magicNbr==expectedMagicNbr);

    Alloc a = {starting_adress,kind,size};
    return a;
}


unsigned long
mem_realloc_small() {
    assert(arena.chunkpool == 0);
    unsigned long size = (FIRST_ALLOC_SMALL << arena.small_next_exponant);
    arena.chunkpool = mmap(0,
			   size,
			   PROT_READ | PROT_WRITE | PROT_EXEC,
			   MAP_PRIVATE | MAP_ANONYMOUS,
			   -1,
			   0);
    if (arena.chunkpool == MAP_FAILED)
	handle_fatalError("small realloc");
    arena.small_next_exponant++;
    return size;
}

unsigned long
mem_realloc_medium() {
    uint32_t indice = FIRST_ALLOC_MEDIUM_EXPOSANT + arena.medium_next_exponant;
    assert(arena.TZL[indice] == 0);
    unsigned long size = (FIRST_ALLOC_MEDIUM << arena.medium_next_exponant);
    assert( size == (1 << indice));
    arena.TZL[indice] = mmap(0,
			     size*2, // twice the size to allign
			     PROT_READ | PROT_WRITE | PROT_EXEC,
			     MAP_PRIVATE | MAP_ANONYMOUS,
			     -1,
			     0);
    if (arena.TZL[indice] == MAP_FAILED)
	handle_fatalError("medium realloc");
    // align allocation to a multiple of the size
    // for buddy algo
    arena.TZL[indice] += (size - (((intptr_t)arena.TZL[indice]) % size));
    arena.medium_next_exponant++;
    return size; // lie on allocation size, but never free
}


// used for test in buddy algo
unsigned int
nb_TZL_entries() {
    int nb = 0;
    
    for(int i=0; i < TZL_SIZE; i++)
	if ( arena.TZL[i] )
	    nb ++;

    return nb;
} 


