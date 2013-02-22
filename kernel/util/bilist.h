#ifndef UTIL_BILIST_H_08dd1ecd
#define UTIL_BILIST_H_08dd1ecd
/**
	Bidirectional-linked List
*/
typedef struct BiList{
	int data;
	struct BiList* prev;
	struct BiList* next;
}BiList;
#define BL_NULL ((BiList*)0)

typedef BiList LoopList;

#include "./bilist_op.h"

#endif
