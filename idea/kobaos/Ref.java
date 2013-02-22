package kobaos;

public class Ref extends Linked
{
	public Ref(Linked p,Linker lnk,boolean withSize)
	{
		this.ptr=p;
		this.lnk=lnk;
		this.withSize=withSize;
	}
	public int size(){return this.withSize?8:4;}
	public byte[] toByteArray()
	{
		int offset=this.lnk.offsetOf(this.ptr);
		byte[] ofb=Util.toByteArray(offset);
		if(!this.withSize)return ofb;
		byte[] out=new byte[8];
		System.arraycopy(ofb,0,out,0,4);
		System.arraycopy(Util.toByteArray(this.ptr.size()),0,out,4,4);
		return out;
	}
	private Linked ptr;
	private Linker lnk;
	private boolean withSize;
}
