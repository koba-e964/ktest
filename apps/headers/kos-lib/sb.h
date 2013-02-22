#ifndef KOS_LIB_SB_H_a1fbb447
#define KOS_LIB_SB_H_a1fbb447
typedef struct StringBuilder
{
	char *buffer;
	size_t clen;
	size_t cmax;
}StringBuilder;
void sb_ctor(StringBuilder *sb,int isize);
void sb_dtor(StringBuilder *sb);
void sb_append_len(StringBuilder *sb,const char *str,size_t len);
void sb_append(StringBuilder *sb,const char *str);
#endif
