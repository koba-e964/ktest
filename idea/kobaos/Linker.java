package kobaos;

import java.util.*;

public class Linker extends Linked
{
	public Linker()
	{
		objs=new ArrayList<Linked>(100);
	}
	public void add(Linked l)
	{
		objs.add(l);
	}
	public int offsetOf(Linked obj)
	{
		int index=objs.indexOf(obj);
		if(index==-1)return -1;
		int place=0;
		for(int i=0;i<index;i++)
		{
			place+=objs.get(i).size();
		}
		return place;
	}
	@Override
	public byte[] toByteArray()
	{
		try
		{
			int size=this.size();
			byte[] out=new byte[size];
			int pos=0;
			for(Linked obj:this.objs)
			{
				byte[] dat=obj.toByteArray();
				System.arraycopy(dat,0,out,pos,dat.length);
				pos+=dat.length;
			}
			return out;
		}
		catch(NullPointerException npx){return null;}
	}
	@Override
	public int size()
	{
		try
		{
			int size=0;
			for(Linked obj:this.objs)
			{
				size+=obj.size();
			}
			return size;
		}
		catch(NullPointerException npx){return -1;}
	}
	private List<Linked> objs;
}