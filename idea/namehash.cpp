int namehash(int len,const char *str)
{
	int sum=0;
	while(len>0)
	{
		sum*=0x61;
		sum+=*str&0xff;
		str++;
		len--;
	}
	return sum;
}
