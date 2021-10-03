#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* exported functions are:
 * void *allocate(unsigned long n, unsigned a);
 * void deallocate(unsigned a);
 * void *newarray(unsigned long m, unsigned long n, unsigned a);
 */

/* an Arena is a linked list of large blocks or memory */
struct block {
        struct block *next;
        char *limit;
        char *avail;
};

static struct block first[] = { {NULL}, {NULL}, {NULL} };
static struct block *arena[] = { &first[0], &first[1], &first[2] };

/* roundup value is normally passed as a union of common C types 
 * 'sizeof(union align)', which means it's a power of two.
 */
#define roundup(n, round_up_value) \
        (n + (round_up_value-1)) & ~(round_up_value-1)

union align {
        long l;
        char *p;
        double d;
        int (*f)(void);
};

union header {
        struct block b;
        union align a; /* just to make sure we start at a proper alignment
                        * _after_ jumping over a struct block. 
                        */
};

static struct block *freeblocks;

/* most allocations are trivial.
 *
 * 1) round the request size up to the proper alignment boundary
 * 2) increment the 'avail' pointer by the rounded size
 * 3) return the start of the new memory area allocated (the previous value)
 */
void *allocate(unsigned long size, unsigned a)
{
        struct block *ap;
        void *ptr;

        /*nacuna??? no checking for out-of-bounds arena index */
        ap = arena[a];

        size = roundup(size, sizeof(union align));
        while ((ap->avail + size) > ap->limit) {
                /* ok so why do we assign ap->next immediately to the first 
                 * 'freeblock' ?
                 *
                 * the arena[x] pointer is also a list tail pointer - pointing to 
                 * the "latest" block to be appended to the list.
                 *
                 * see figure 2.1 Arena representation
                 */
                if ((ap->next = freeblocks) != NULL) {
                        freeblocks = freeblocks->next;
                        ap = ap->next;
                } else {
                        /* if we have exhausted all the freeblocks...
                         * then we need to call on the system now to give us 
                         * a new block that will be big enough to satisfy 
                         * the rounded up request 'size'.
                         */

                        unsigned m = sizeof(union header) + size + 1*1024;
                        ap->next = malloc(m);
                        ap = ap->next;
                        if (ap == NULL) {
                                fprintf(stderr, "insufficient memory\n");
                                exit(-1);
                        }
                        ap->limit = (char *)ap + m;
                }

                /* pointer arithmetic. we simple convert the 'ap' to a 
                 * header pointer so we can add 1 and assign the avail to the
                 * first free space in the block. */
                ap->avail = (char *)((union header *)ap + 1);
                ap->next = NULL;
                arena[a] = ap;
        }

        /* when we reach this point, we have an 'ap' that has enough space 
         * to allow our requested 'size' */
        ptr = ap->avail;
        ap->avail = ap->avail + size;

        return ptr;
        
}

/* deallocations:
 * an arena is a deallocated by adding its blocks to the free-blocks list and 
 * reinitializing it to point to the appropriate one-element list that holds 
 * a zero-length block.`
 *
 * the idea here is to prepend __all__ the linked blocks found in the arena[i]
 * to the freeblocks list.
 *
 * yes. free-ing an arena frees the _ENTIRE_ arena.
 *
 */
void deallocate(unsigned a)
{
        /* link the last block of arena[i] to the head of freeblocks */
        arena[a]->next = freeblocks;

        /* make freeblocks now point to the first block of arena[i] */
        freeblocks = first[a].next;

        /* detach the arena block list now from the sentinel structure first[a] */
        first[a].next = NULL;

        /* reinitialize the arena like it was from the beginning,
         * pointing to the sentinel block structure. */
        arena[a] = &first[a];
        return;
}

int main(void)
{
        char *test = allocate(128, 0);
        test = allocate(128*1024, 0);
        deallocate(0);
        test = allocate(128, 0);
        deallocate(0);
        return 0;
}
