#ifndef __C_H__
#define __C_H__
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define NEW(p, a)       ((p) = allocate(sizeof *(p), (a)))


#define PERM 0  /* arena 0 is the permanent storage */

void *allocate(unsigned long size, unsigned a);
void deallocate(unsigned a);


char *stringn(char *str, int len);
char *stringd(int n);
char *string(char *str);

#endif /* __C_H__ */


