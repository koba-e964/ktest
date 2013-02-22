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
		T array[maxlen]�̊e�v�f�𒸓_�Ƃ݂Ȃ��A
		�����L���O���t�Ō��ԁB
		���̃O���t�̓��[�v���Ȃ��B
		��F[1]->[3]->[2]->[1]->...
		[1]:5
		[2]:9
		[3]:1
	*/
}
#endif
