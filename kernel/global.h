#ifndef GLOBAL_H_DEF_85dd0233
#define GLOBAL_H_DEF_85dd0233
/**
	global.c‚É‚ÄA‘åˆæ•Ï”‚ÌÀ‘Ì‚ğ’è‹`‚·B
*/
#ifdef GLOBAL_DEF
#define global 
#define GLOBAL_INIT(init_val) =(init_val)

#else
#define global extern
#define GLOBAL_INIT(init_val) 
#endif


#endif
