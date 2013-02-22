#ifndef STDDEF_H_b536
#define STDDEF_H_b536
#ifdef __cplusplus
#define EXT_C_BEG extern "C"{
#define EXT_C_END }

#ifndef NULL
#define NULL 0
#endif

#else
#define EXT_C_BEG /**/
#define EXT_C_END /**/

#ifndef NULL
#define NULL ((void*)0)
#endif

#endif
typedef unsigned int size_t;
EXT_C_BEG
EXT_C_END


#endif

