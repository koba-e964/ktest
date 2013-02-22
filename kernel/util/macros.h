#ifndef UTIL_MACROS_H
#define UTIL_MACROS_H

#define make_max(a,b)\
	do{\
		if((a)<(b))\
			a=b;\
	}while(0)
#define make_min(a,b) do{if((a)>(b))a=b;}while(0)
#define _in /**/
#define _out /**/
#define _inout /**/

#define LSB_ALIGN(x,pow) (((x)+(pow-1))& -(pow))

#define arraylen(arr) (sizeof(arr)/sizeof((arr)[0]))

//extern "C"

#ifdef CPP_MODE
#define EXTERN_C_BEGIN extern "C"{
#define EXTERN_C_END }
namespace util
{
	namespace macros
	{
		template<class T,int N>
		inline int sizeArray(T (&arr)[N])
		{
			return N;
		}
		
		template<class I>
		inline I min(I a, I b)
		{
			return a<b?a:b;
		}
		template<class I>
		inline I max(I a,I b)
		{
			return b<a?a:b;
		}
		template<class T,T min,T sup>
		inline bool inRange(T val)
		{
			return min<=val && val<sup;
		}
	}
}
#else
#define EXTERN_C_BEGIN
#define EXTERN_C_END 
#endif

#endif
