package alma.Control.datamodel.meta.eth;

import alma.Control.datamodel.meta.base.MonitorPoint;

import org.openarchitectureware.core.meta.core.Element;

public class MonitorImpl extends MonitorPoint
{
	MandCImpl mac;

	public MonitorImpl(String[] row, Element parent)
	{
		super(row, parent);
		mac = new MandCImpl(row, sheet);
	}

	public void setAssemblyName(String assembly)
	{
		mac.setAssemblyName(assembly);
	}

	public String Assembly()
	{
		return mac.Assembly();
	}

    public String OnChange()
    {
        return mac.OnChange();
    }

	public String Address()
	{
		return mac.Address();
	}

	public boolean isEnumAddress()
	{
		try
		{
			Integer.decode(mac.Address());
		}
		catch(Exception e)
		{
			return true;
		}

		return false;
	}

	public String DataType()
	{
		return mac.DataType();
	}

	public boolean isDataArray()
	{
		return mac.isDataArray();
	}

	public boolean isOpenArray()
	{
		return mac.isOpenArray();
	}

	public String getArrayLength()
	{
		return mac.getArrayLength();
	}

	public String DataToIDLType()
	{
		return mac.DataToIDLType();
	}

	public String DataToIDLSeqType()
	{
		return mac.DataToIDLSeqType();
	}

	public String DataToCPPType()
	{
		return mac.DataToCPPType();
	}

	public String DataToCORBAType()
	{
		return mac.DataToCORBAType();
	}

	public String DataToCORBADevIOType()
	{
		return mac.DataToCORBADevIOType();
	}

	public String DataToCORBASeqType()
	{
		return mac.DataToCORBASeqType();
	}

	public String DataToBACIType()
	{
		return mac.DataToBACIType();
	}

	public boolean isArchived(){
		//FIXME Overriding method to exclude unsupported special case (see COMP-4996).
		if (DataType().equals("uint"))
			return false;
		//FIXME Exclude special case COMP-4996
		if ( DataType().equals("bool") || DataType().equals("boolean")
				|| DataType().equals("string")
				|| (DataType().equals("double") && isDataArray())
				|| (DataType().equals("int") && isDataArray()) )
			return false;
		return true;
	}

	public String DataToDatabaseType()
	{
		return mac.DataToDatabaseType();
	}

	@Override
	public boolean isPartOfPattern() {
		return false;
	}

	@Override
	public boolean isHomogeneous() {
		// TODO Auto-generated method stub
		return true;
	}

	@Override
	public boolean isPartOfHomogeneous() {
		// TODO Auto-generated method stub
		return false;
	}

}
