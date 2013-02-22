#ifndef KOS_LIB_ASSERT_H_5e5ac539
#define KOS_LIB_ASSERT_H_5e5ac539
/**
if val==0 then abort().
*/
#define assert(val) do{if(!(val))abort();}while(0)
#endif
