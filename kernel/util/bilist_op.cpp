#include "./bilist_op.h"
BiList *bilist_end(BiList *target)
{
	//check if `target` is an infinite loop
	BiList *current=target;
	while(1)
	{
		if(current->next==(BiList*)0)
		{
			return current;
		}
		current=current->next;
		if(current==target)
		{
			//an infinite loop was detected.
			return (BiList*)0;//nullptr
		}
	}

}
int bilist_add(BiList *target,BiList *inserted)
{
	BiList *orig_next=target->next;
	BiList *eoi=bilist_end(inserted);
	if(eoi==(BiList*)0)
	{
		return -1;//failure
	}
	target->next=inserted;
	inserted->prev=target;

	eoi->next=orig_next;
	if(orig_next != (BiList*)0 )
		orig_next->prev=eoi;

	return 0;//success
}

int bilist_insert(BiList *left,BiList *right,
		BiList *inserted_left,BiList *inserted_right)
{
	if(left!=BL_NULL)
	{
		left->next=inserted_left;
	}
	inserted_left->prev=left;

	inserted_right->next=right;
	if(right!=BL_NULL)
	{
		right->prev=inserted_right;
	}
	return 0;
}

BiList* bilist_forward(BiList *target,int index)
{
	BiList *cur=target;
	if(target==BL_NULL)
		return BL_NULL;
	if(index>0)
	{
		for(;index>0;index--)
		{
			cur=cur->next;
			if(cur==BL_NULL)
				return BL_NULL;
		}
		return cur;
	}
	if(index<0)
	{
		for(;index<0;index++)
		{
			cur=cur->prev;
			if(cur==BL_NULL)
				return BL_NULL;
		}
		return cur;
	}
	//index==0
	return target;
}

int bilist_is_loop(BiList *target)
{
	return bilist_end(target)==BL_NULL;
}
int bilist_make_loop(BiList *target)
{
	BiList *eol=bilist_end(target);
	if(eol==BL_NULL)
		return -1;//target is already a loop
	eol->next=target;
	target->prev=eol;
	return 0;//success
}

LoopList *looplist_add(LoopList *target,LoopList *ins)
{
	if(target==BL_NULL)
	{
		ins->prev=ins;
		ins->next=ins;
		return ins;
	}
	bilist_insert(target->prev,target,ins,ins);
	return target;
}
LoopList *looplist_remove(LoopList *ll,LoopList *node)
{
	LoopList* next=node->next;
	if(next==node)
	{
		return BL_NULL;
	}
	LoopList *prev=node->prev;
	prev->next=next;
	next->prev=prev;
	if(ll==node)
		return next;
	return ll;
}
