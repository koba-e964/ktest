#ifndef UTIL_BILIST_OP_H_5d51e66d829ca40d
#define UTIL_BILIST_OP_H_5d51e66d829ca40d
#include "bilist.h"

BiList *bilist_end(BiList *target);


int bilist_add(BiList *target,BiList *inserted);



/**
	left <-> inserted_left <-> ... <-> inserted_right <-> right
*/
int bilist_insert(BiList *left,BiList *right,
		BiList *inserted_left,BiList *inserted_right);


BiList* bilist_forward(BiList *target,int nElem);

int bilist_is_loop(BiList *target);

int bilist_make_loop(BiList *target);
/**
	target‚Ì––”öi1ŒÂ‘Oj‚Éins‚ğ’Ç‰Á
*/
LoopList* looplist_add(LoopList* target,LoopList *ins);

LoopList *looplist_remove(LoopList *ll,LoopList* node);


#endif
