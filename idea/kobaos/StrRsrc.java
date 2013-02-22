package kobaos;

public class StrRsrc extends Linked
{
	public StrRsrc(String str)
	{
		this.content=str.getBytes();
	}
	@Override
	public int size()
	{
		return content.length+1;//'\0'
	}
	@Override
	public byte[] toByteArray()
	{
		byte[] out=new byte[content.length+1];
		System.arraycopy(this.content,0,out,0,content.length);
		out[content.length]=0;
		return out;
	}
	private byte[] content;
}