/******************************************************
 * Copyright Grégory Mounié 2018                      *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <assert.h>
#include "mem.h"
#include "mem_internals.h"

void *
emalloc_small(unsigned long size)
{
    /* ecrire votre code ici */
    if (arena.chunkpool == NULL){
        unsigned long taille = mem_realloc_small(); //la taille en octet du bloc dont l’adresse est maintenant dans arena.chunkpool
        int nChunks = taille/CHUNKSIZE ;

        void **p = arena.chunkpool;
        for (int i = 0; i < nChunks; i++){
            *p = (void *)(p + CHUNKSIZE/8); //on divise par 8 parce que le type void **ptr vaut 8 octets
            p += CHUNKSIZE/8 ;

        }
        p -=CHUNKSIZE/8;
        *p = NULL;
    }
        void *p = arena.chunkpool;
        arena.chunkpool = *(void **) p;
    return (void *)mark_memarea_and_get_user_ptr(p, CHUNKSIZE, SMALL_KIND);
}

void efree_small(Alloc a) {
    /* ecrire votre code ici */
    
     *(void **)a.ptr = arena.chunkpool;
    arena.chunkpool = a.ptr;
    }
