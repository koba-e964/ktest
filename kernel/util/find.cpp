#include "./find.h"

int find_bit_in_array(const char* bits,int bit,int lenBits)
{
	int up29=lenBits>>3;//upper 29 bits
	int all=bit ? 0 : 0xff;
	lenBits-=up29<<3;
	int i;
	for(i=0;i<up29;i++)
	{
		int j;
		if((bits[i]&0xff)==all)
			continue;
		for(j=0;j<8;j++)
		{
			if( ((bits[i]>>j)&1) == bit)
			{
				return i*8+j;
			}
		}
	}
	bits+=up29<<3;
	for(i=0;i<lenBits;i++)
	{
		if( ((bits[0]>>i)&1) == bit)
			return (up29<<3)+i;
	}
	return -1;
}
