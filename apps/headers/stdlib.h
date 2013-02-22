#ifndef STDLIB_H_1021827b
#define STDLIB_H_1021827b
#include <stddef.h>
#include <sys/types.h>
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define RAND_MAX 0x7fff
EXT_C_BEG
long a64l(const char*);
void abort(void);
int abs(int);
int atexit(void (*)(void));
double atof(const char *);
int atoi(const char*);
long int atoll(const char*);

void *malloc(size_t );
void free(void *);

void srand(u32 seed);
int rand(void);
EXT_C_END
#endif
