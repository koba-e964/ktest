#include <stdio.h>

int main(int ,char** argv)
{
	char *name="haribote.img";
	FILE *fp=fopen(name,"rb");
	int sum=0;
	int cnt=0;
	while(1)
	{
		char ch;
		if(fread(&ch,1,1,fp) != 1)
			break;
		if(cnt == 0x0)
			printf("ch[%x]=%02x\n",cnt,ch&0xff);
		sum += (ch & 0xff)*cnt;
		cnt++;
	}
	printf("sum=%08x\ncnt=%x\n",sum,cnt);
	return 0;
}