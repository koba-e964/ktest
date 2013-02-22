#include "bilist.hpp"

namespace util
{
	BiListCpp::BiListCpp(void){
		cont.data=0;
		cont.prev=BL_NULL;
		cont.next=BL_NULL;
	}
	BiListCpp::BiListCpp(BiList list)
	{
		cont=list;
	}
	BiList *BiListCpp::operator[](int i)
	{
		return bilist_forward(&cont,i);
	}
	bool BiListCpp::isLoop(void)const
	{
		return bilist_is_loop(const_cast<BiList*>(&cont))!=0;
	}
	
}
