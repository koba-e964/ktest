#ifndef UTIL_BILIST_HPP_00d63a61
#define UTIL_BILIST_HPP_00d63a61

#include "bilist.h"
namespace util
{
	class BiListCpp
	{
	public:
		BiListCpp(void);
		BiListCpp(BiList list);
		BiList *operator[](int i);
		bool isLoop(void)const;
		
	private:
		BiList cont;
	};
	/**
		T array[maxlen]の各要素を頂点とみなし、
		それを有向グラフで結ぶ。
		そのグラフはループをなす。
		例：[1]->[3]->[2]->[1]->...
		[1]:5
		[2]:9
		[3]:1
	*/
}
#endif
