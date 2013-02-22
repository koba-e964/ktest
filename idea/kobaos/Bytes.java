package kobaos;

public class Bytes extends Linked
{
	public Bytes(int size)
	{
		dat=new byte[size];
	}
	public int size()
	{
		return dat.length;
	}
	public byte[] toByteArray()
	{
		byte[] out=new byte[dat.length];
		System.arraycopy(this.dat,0,out,0,out.length);
		return out;
	}
	private byte[] dat;
}