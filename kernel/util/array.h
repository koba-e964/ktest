#ifndef UTIL_ARRAY_HPP_5cdedee6
#define UTIL_ARRAY_HPP_5cdedee6
#include "array_op.h"
#include "../layer.h"
namespace util
{
	/**
		An array whose maximum length is fixed.
	*/
	inline void error_msg(const char* str)
	{
		//DEBUG_PRINT(str);
	}
	template<class T,int maxlen>
	class LimArray
	{
	private:
		T elem_[maxlen];
		int cur_len_;
	public:
		LimArray(void):cur_len_(0)
		{
		}
		LimArray(T init):cur_len_(0)
		{
			for(int i=0;i<maxlen;i++)
				elem_[i]=init;
		}
		void ctor(T init,int initlen)
		{
			if(initlen < 0)cur_len_=0;
			if(initlen > maxlen)cur_len_ = maxlen;
			for(int i=0;i<maxlen;i++)
				elem_[i]=init;
		}
		int insert(const T* array,int length,int position)
		{
			int res=insert_array(&elem_,position,array,length,cur_len_,
				maxlen,sizeof(T));
			if(res >= 0)//success
			{
				cur_len_+=length;
			}
			return res;
		}
		int erase(int start,int end)
		{
			int lengthCut=erase_array(&elem_,start,end,cur_len_,sizeof(T));
			cur_len_-=lengthCut;
			return lengthCut;
		}
		inline const T& operator[] (int index)const
		{
			if(index<0 || index>=maxlen)
			{
				error_msg("ArrayIndexOutOfBounds:");
				return *(const T*)0;
			}
			return elem_[index];
		}
		inline T& operator[](int index)
		{
			return const_cast<T&>(const_cast<const LimArray<T,maxlen>& >(*this)[index]);
		}
		inline int size(void)const
		{	
			return cur_len_;
		}
		inline int capacity(void)const
		{
			return maxlen;
		}
	};
	template<class T,int N>
	class FixedArray{//array whose length is fixed
	private:
		T data[N];
	public:
		void init(T val)
		{
			for(int i=0;i<N;i++)
				data[i]=val;
		}
		inline const T& operator[](int index)const
		{
			if(index<0 || index>=N)
			{
				error_msg("error@FixedArray");
				return *(const T*)0;
			}
			return data[index];
		}
		inline T& operator[](int index)
		{
			return const_cast<T&>(const_cast<const FixedArray<T,N>& >(*this)[index]);
		}

	};
}

#endif
