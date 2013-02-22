#include <stdlib.h>
#define TEST 0

#if TEST
typedef unsigned int u32;
#else
#define ksrand srand
#define krand rand
#endif

static u32 state[4];


void ksrand(u32 seed)
{
	int i;
	for(i=0;i<4;i++)
	{
		state[i]=seed&0xcafebabeul;
		seed*=0x875;//4Z+1
		seed+=1;//2Z+1
	}
	if(state[3]==0)
		state[3]=0xcafebabeul;
}

/**
P(x)=x^127+x^31+x^3+x+1->0x8000000b
2^127-1 is a prime
Mersenne Twisterの劣化版
*/
int krand(void)
{
	int i;
	u32 highest=0;
	u32 msb=state[3]&0x40000000ul;
	msb>>=30;
	msb=-msb;/*0x40000000->-1=0xffffffff,0->0*/
	/*shift*/
	for(i=0;i<4;i++)
	{
		int tmp=state[i]&0x80000000ul;
		state[i]<<=1;
		state[i]|=highest>>31;
		highest=tmp;
	}
	state[0]^=msb&0x8000000b;
	highest=state[0]^(state[0]>>3);//highestの使い回し,よい子は真似をしないように
	highest^=highest<<5;
	highest^=highest>>11;
	highest^=highest<<17;
	return (int)((highest^(highest>>16))&0x7fff);
}


#if TEST
#include <math.h>
#include <stdio.h>
void printstate(void)
{
	int i;
	for(i=0;i<4;i++)
	{
		printf("state[%d]=%08x\n",i,state[i]);
	}
}
int main(void)
{
	int randbox[0x8000]={0};
	int i=0;
	ksrand(718);
	for(i=0;i<0x20000000;i++)
	{
		int res=krand();
		randbox[res]++;
	}
	for(i=0;i<0x8000;i++)
	{
		if(abs(randbox[i]-0x4000)>=128*3.487)/*randbox[i] \sim N(2^{14},2^{14}(1-2^{-15})),P(|Z|\leq 3.487)=1-2^{-11}*/
		{
			printf("out of 3.487:%04x,number:%x\n",i,randbox[i]);//16個くらい出るものと予想される
		}
	}
	printstate();
	return 0;
}

#endif
