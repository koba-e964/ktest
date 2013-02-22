package kobaos;

/**
	struct RsrcEntry
	{
		int hash;
		int addr;
		int name;
	}
*/
public class RsrcEntry extends Linked
{
	public RsrcEntry(String name,Linked data,Linker lnk)
	{
		this.hash=Hash.namehash(name);
		this.ref=new Ref(data,lnk,true);
		this.lnk=lnk;
	}
	public int size()
	{
		return 12;
	}
	public byte[] toByteArray()
	{
		byte[] out=new byte[12];
		System.arraycopy(Util.toByteArray(hash),0,out,0,4);
		System.arraycopy(ref.toByteArray(),0,out,4,8);
		return out;
	}
	private int hash;
	private Ref ref;
	private Linker lnk;
}
