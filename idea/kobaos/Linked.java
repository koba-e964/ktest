package kobaos;

public abstract class Linked
{
	public abstract int size();
	public abstract byte[] toByteArray();
	public boolean equals(Linked t)
	{
		return this==t;
	}
}