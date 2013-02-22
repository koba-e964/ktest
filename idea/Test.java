import kobaos.*;
public class Test
{
	public static Linker test()
	{
		Linker lnk=new Linker();
		Bytes ba=new Bytes(8);
		RsrcEntry re=new RsrcEntry("test-window",ba,lnk);
		lnk.add(re);
		lnk.add(ba);
		return lnk;
	}
}