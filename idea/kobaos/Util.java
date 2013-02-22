package kobaos;

public class Util
{
	public static byte[] toByteArray(int val)
	{
		byte[] out=new byte[4];
		for(int i=0;i<4;i++)
		{
			out[i]=(byte)val;
			val>>>=8;
		}
		return out;
	}
}
