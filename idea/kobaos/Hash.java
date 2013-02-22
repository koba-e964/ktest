package kobaos;

public class Hash
{
	static int namehash(String str)
	{
		int sum=0;
		for(int i=0,l=str.length();i<l;++i)
		{
			sum*=0x61;
			sum+=str.charAt(i);
		}
		return sum;
	}
	
}
