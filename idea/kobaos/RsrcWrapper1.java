package kobaos;

/**
	あるhashを持つ名前が一つしかなかったときに配置される。
	struct RsrcWrapper1
	{
		char *name;
		int  type;
		char data[1];
	};//size>=8
*/
public class RsrcWrapper1 extends Linked
{
	public RsrcWrapper1(StrRsrc name,int type,Linked target,Linker lnk)
	{
		this.nameref=new Ref(name,lnk,false);
		this.type=type;
		this.target=target;
		this.lnk=lnk;
	}
	@Override
	public int size()
	{
		return this.nameref.size()+4+this.target.size();
	}
	@Override
	public byte[] toByteArray()
	{
		int size=this.size();
		byte[] out=new byte[size];
		int ns=this.nameref.size();
		System.arraycopy(this.nameref.toByteArray(),0,out,0,ns);
		System.arraycopy(Util.toByteArray(this.type),0,out,ns,4);
		System.arraycopy(this.target.toByteArray(),0,out,ns+4,this.target.size());
		return out;
	}
	private Linked target;
	private Ref nameref;
	private int type;
	private Linker lnk;
}
