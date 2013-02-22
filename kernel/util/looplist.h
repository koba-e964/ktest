#ifndef UTIL_LOOPLIST_H_a22c6ab4
#define UTIL_LOOPLIST_H_a22c6ab4

#include "../layer.h"
namespace util
{
	template<class T,int maxlen>
	class LoopListCpp
	{
	private:
		int fail;
	public:
		struct LLCNode{
			T data;
			struct LLCNode *next,*prev;
		};
		LLCNode array[maxlen];
		LoopListCpp(void)
		{
			ctor();
		}
		void ctor(void)
		{
			fail=0;
			cur_=0;
			for(int i=0;i<maxlen;i++)
			{
				array[i].data=T();
				array[i].next=0;
				array[i].prev=0;
			}
		}
		//––”ö‚É‰Á‚¦‚éB
		void add(int index)
		{
			if(index<0)
			{
				fail=-2;
				return;
			}
			if(index >= maxlen)
			{
				fail=-3;
				return ;
			}
			if(cur_ == (LLCNode*)0)
			{
				cur_=array+index;
				cur_->next=cur_;
				cur_->prev=cur_;
				return;
			}
			//check if array[index] is used
			if(array[index].next !=0)
			{
				fail=-4;
				return;
			}
			LLCNode *target=array+index;
			LLCNode *last=cur_->prev;
			target->next=cur_;
			last->next=target;
			target->prev=last;
			cur_->prev=target;
		}
		int loopLength(void)const
		{
			if(cur_==(LLCNode*)0)
				return 0;
			LLCNode *first=this->cur_;
			LLCNode *current=this->cur_;
			int count=0;
			do{
				current=current->next;
				count++;
			}while(first != current);
			return count;
		}
		inline void operator++(void)
		{
			if(cur_ != 0)
				cur_=cur_->next;
		}
		inline void operator++(int)
		{
			if(cur_ != 0)
				cur_=cur_->next;
		}
		inline void operator--(void)
		{
			if(cur_ != 0)
				cur_=cur_->prev;
		}
		inline void operator--(int)
		{
			if(cur_ != 0)
				cur_=cur_->prev;
		}
		void erase(int index)
		{
			if(index<0)
			{
				fail=-1;
				return;
			}
			if(index>=maxlen)
			{
				fail=-5;
				return;
			}
			LLCNode *target=array+index;
			if(target->next == 0)//array[index] is not used
				return ;
			LLCNode *next=target->next,*prev=target->prev;
			next->prev=prev;
			prev->next=next;
			target->next=0;
			target->prev=0;
			if(cur_ == target)
			{
				if(cur_ ==next)
					cur_ = 0;//if listLength()==1,cur_ becomes 0.
				cur_ =next;
			}
		}
		inline void erase(void)
		{
			return erase(cur_ - array);
		}
		inline int currentIndex(void)const
		{
			if(cur_ == 0)
				return -1;
			return cur_-array;
		}
		inline T operator*(void)const{return cur_->data;}
		inline T& data(int index){
			if((unsigned int)index >= (unsigned int)maxlen)
			{
				//DEBUG_PRINT("LoopListCpp::data(int) : error!\n");
				return *(T*)0;
			}
			return this->array[index].data;
		}
	private:
		LLCNode *cur_;
	};
}

#endif
