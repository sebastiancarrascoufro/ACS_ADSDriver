package alma.Control.datamodel.meta.eth;

import alma.Control.datamodel.meta.base.ControlPoint;
import alma.Control.datamodel.meta.base.Table;

import org.openarchitectureware.core.meta.core.Element;

public class ControlImpl extends ControlPoint
{
	MandCImpl mac;

	public ControlImpl(String[] row, Element parent)
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
	public String DataToDatabaseType()
	{
		return mac.DataToDatabaseType();
	}
}
