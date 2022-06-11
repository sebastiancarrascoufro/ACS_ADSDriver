package alma.Control.datamodel.meta.amb;

import java.util.Iterator;

import org.openarchitectureware.core.constraint.DesignError;
import org.openarchitectureware.core.meta.core.ElementSet;
import org.openarchitectureware.core.meta.core.Element;

import alma.Control.datamodel.meta.base.Table;
import alma.Control.datamodel.meta.base.Util;
import alma.Control.datamodel.meta.base.MandCBase;

public class MandCImpl {
	protected String[] row;

	protected int sheet;

	protected MandCBase mcp;

	public MandCImpl(String[] row, int sheet, MandCBase mcp)
	{
		this.row = row;
		this.sheet = sheet;
		this.mcp = mcp;
	}

	public String Assembly()
	{
		return row[Table.getColNum(sheet, "Assembly")];
	}

	public String RCA()
	{
		return row[Table.getColNum(sheet, "RCA")];
	}

	public String RCACell()
	{
		return row[Table.getColNum(sheet, "RCA")];
	}

	public String RawDataType()
	{
		String s = row[Table.getColNum(sheet, "Raw Data Type")];
		if(!mcp.isDependent())
			return isRawDataArray() ? s.substring(0, s.indexOf("[")) : s;
		if(mcp instanceof MonitorImpl)
			return ((MonitorImpl) mcp.getParent()).RawDataType();
		if(mcp instanceof ControlImpl)
			return ((ControlImpl) mcp.getParent()).RawDataType();
		return "ERROR";
	}

	public String RawDataTypeCell()
	{
		return row[Table.getColNum(sheet, "Raw Data Type")];
	}

	public boolean TeRelated()
	{
		return false;
		// return row[Table.getColNum(sheet, "TE Related")].equals("yes");
	}

	public String TeRelatedCell()
	{
		return row[Table.getColNum(sheet, "TE Related")];
	}

    public String OnChange()
    {
        return row[Table.getColNum(sheet, "On Change")];
    }

	public String WorldDataType()
	{
		String s = row[Table.getColNum(sheet, "World Data Type")];
		return isWorldDataArray() ? s.substring(0, s.indexOf("[")) : s;
	}

	public String WorldDataTypeCell()
	{
		return row[Table.getColNum(sheet, "World Data Type")];
	}

	public String Scale()
	{
		String s = ScaleCell();
		if((s.equals("none") == true) || (s.equals("extended") == true)
				|| (s.equals(Table.CelsiusToKelvin) == true))
			s = "1.0";
		return s;
	}

	public String ScaleCell()
	{
		return Util.normalizeNumber(WorldDataType(), row[Table.getColNum(sheet,
				"Scale")]);
	}

	public String Offset()
	{
		String s = ScaleCell();
		if(s.equals(Table.CelsiusToKelvin))
			return "273.15";
		String o = OffsetCell();
		if((o.equals("0") == true) || (o.equals("none") == true)
				|| (o.equals("extended") == true))
			o = "0.0";
		return o;
	}

	public String OffsetCell()
	{
		return Util.normalizeNumber(WorldDataType(), row[Table.getColNum(sheet,
				"Offset")]);
	}

	public String RawDataToCPPType()
	{
		return (String) Table.rawToCPP.get(RawDataType());
	}

	public String WorldDataToCPPType()
	{
		return (String) Table.worldToCPP.get(WorldDataType());
	}

	public String WorldDataToCORBAType()
	{
		return (String) Table.worldToCORBA.get(WorldDataType());
	}

	public String WorldDataToIDLSeqType()
	{
		return (String) Table.worldToIDLSeq.get(WorldDataType());
	}

	public String WorldDataToIDLType()
	{
		return (String) Table.worldToIDL.get(WorldDataType());
	}

	public String WorldDataToCORBASeqType()
	{
		return (String) Table.worldToCORBASeq.get(WorldDataType());
	}

	public String WorldDataToCORBADevIOType()
	{
		return (String) Table.worldToCORBADevIO.get(WorldDataType());
	}

	public String WorldDataToJavaType()
	{
		return (String) Table.worldToJava.get(WorldDataType());
	}

	public String WorldDataToDatabaseType()
	{
		return (String) Table.worldToDatabase.get(WorldDataType());
	}

	public String WorldDataToBACIType()
	{
		return (String) Table.worldToBACI.get(WorldDataType());
	}

	public boolean isConversion()
	{
		if(ScaleCell().equals("none") && OffsetCell().equals("none"))
		{
			return false;
		}
		else if(ScaleCell().equals("1.0") && OffsetCell().equals("0.0"))
		{
			return false;
		}
		else if(ScaleCell().equals("1.0") && OffsetCell().equals("none"))
		{
			return false;
		}
		else if(ScaleCell().equals("none") && OffsetCell().equals("0.0"))
		{
			return false;
		}

		return true;
	}

	public boolean isSpecialConversion()
	{
		return ScaleCell().equals("extended");
	}

	public boolean isWorldDataArray()
	{
		String s = row[Table.getColNum(sheet, "World Data Type")];
		return s.endsWith("]") ? true : false;
	}

	public boolean isRawDataArray()
	{
		String s = row[Table.getColNum(sheet, "Raw Data Type")];
		if(!mcp.isDependent())
			return s.endsWith("]") ? true : false;
		if(s.startsWith("&lt;") && s.substring(1).indexOf("&lt;") != -1)
			return true;
		// if(mcp instanceof MonitorImpl)
		// return ((MonitorImpl)mcp.getParent()).isRawDataArray();
		// if(mcp instanceof ControlImpl)
		// return ((ControlImpl)mcp.getParent()).isRawDataArray();
		return false;
	}

	public String NumberItemsRawData()
	{
		String s = row[Table.getColNum(sheet, "Raw Data Type")];
		return Util.NumberOfItems(s);
	}

	public String TotalBytesRawData()
	{
		String s = row[Table.getColNum(sheet, "Raw Data Type")];
		return Util.RawDataTypeTotalBytes(s);
	}

	public String NumberRawDataTypeBytes()
	{
		return Util.NumberRawDataTypeBytes(RawDataType());
	}

	public String NumberItemsWorldData()
	{
		String s = row[Table.getColNum(sheet, "World Data Type")];
		return Util.NumberOfItems(s);
	}

	// //////////////////// Logical Conditions

	public boolean isDependentGroupBit()
	{
		if(!mcp.isDependent())
			return false;
		if(isDependentBit() && RawDataTypeCell().indexOf("-") != -1)
			return true;
		return false;
	}

	public boolean isDependentBit()
	{
		if(!mcp.isDependent())
			return false;
		if(RawDataTypeCell().startsWith("&lt;"))
			return true;
		return false;
	}

	public boolean isDependentElement()
	{
		if(!mcp.isDependent())
			return false;
		if(RawDataTypeCell().startsWith("["))
			return true;
		return false;
	}

	public boolean isByteSwapped()
	{
		return ((String) Table.rawToByteSwapped.get(RawDataType()))
				.equals("true");
	}

	public boolean isDependentBitElement()
	{
		if(!mcp.isDependent())
			return false;
		if(RawDataTypeCell().startsWith("&lt;"))
			return true;
		return false;
	}

	public boolean isDependentArrayElement()
	{
		if(!mcp.isDependent())
			return false;
		if(RawDataTypeCell().startsWith("["))
			return true;
		return false;
	}
	
	public String GetDimension()
	{
		String s = RawDataTypeCell();
		if(isDependentArrayElement())
		{
			int n = s.indexOf("]");
			return s.substring(1, n);
		}
		if(!isRawDataArray())
			return "0";
		if(isDependentBitElement())
		{
			int n = s.indexOf("&gt;");
			return s.substring(4, n);
		}
		return "0";
	}

	public boolean isSingleBit()
	{
		String s = RawDataTypeCell();
		if(!s.startsWith("&lt;"))// &lt;0&gt;
			return false;
		if(!s.endsWith("&gt;"))
			return false;
		if(s.indexOf("-") == -1)
			return true;
		return false;
	}

	public String getBit()
	{
		String s = RawDataTypeCell();
		int n = s.indexOf("&gt;");
		return s.substring(4, n);
	}

	private static String[] mask = {"0x01", "0x02", "0x04", "0x08", "0x10",
			"0x20", "0x40", "0x80"};

	public String getMask()
	{
		if(!isSingleBit())
			return "";
		String s = null;
		if(isRawDataArray())
		{
			s = RawDataTypeCell().substring(4);
			int n = s.indexOf("&lt;");
			int m = s.substring(n + 4).indexOf("&gt;");
			s = s.substring(n + 4, m + n + 4);
		}
		else
		{
			s = RawDataTypeCell().substring(4, RawDataTypeCell().length() - 4);
		}
		int n = Integer.parseInt(s);
		return mask[n];
	}

	public String getFirstBit()
	{
		String s = RawDataTypeCell();
		if(!s.startsWith("&lt;") || !s.endsWith("&gt;"))
			return "";
		if(isSingleBit())
			return getBit();
		int n = s.indexOf("-");
		if(n == -1)
			return "";
		return s.substring(4, n);
	}

	public String getLastBit()
	{
		String s = RawDataTypeCell();
		if(!s.startsWith("&lt;") || !s.endsWith("&gt;"))
			return "";
		if(isSingleBit())
			return getBit();
		int n = s.indexOf("-");
		if(n == -1)
			return "";
		return s.substring(n + 1, s.length() - 4);
	}

	public String getGroupStart()
	{
		String s = RawDataTypeCell();
		if(isRawDataArray())
		{
			s = s.substring(4);
			int n = s.indexOf("&lt;");
			int m = s.substring(n + 4).indexOf("&gt;");
			if(n == -1 || m == -1)
				return "";
			s = s.substring(n + 4, m + n + 4);
		}
		else
		{
			s = s.substring(4, s.length() - 4);
		}
		// "s" is of type "i-j"
		int n = s.indexOf("-");
		if(n == -1)
			return "";
		return s.substring(0, n);
	}

	public String getGroupEnd()
	{
		String s = RawDataTypeCell();
		int n = s.indexOf("-");
		if(n == -1)
			return "";
		return s.substring(n + 1, s.length() - 4);
	}

	public String getGroupMask()
	{
		String firstBit = getGroupStart();
		String lastBit = getGroupEnd();
		int n = Integer.parseInt(firstBit);
		int m = Integer.parseInt(lastBit);
		int intMaskSize = m - n;
		int intMask = 0;
		for(int i = 0; i <= intMaskSize; i++)
		{
			intMask += Math.pow(2, i);
		}
		String mask = "0x".concat(Integer.toHexString(intMask).toUpperCase());
		return mask;
	}

	public boolean isRawSubArray()
	{
		String s = RawDataTypeCell();
		if((s.indexOf("[") != -1) && (s.indexOf("-") != -1)
				&& (s.indexOf("]") != -1))
			return true;
		return false;
	}

	public String RawSubArrayStartIndex()
	{
		String s = RawDataTypeCell();
		return String.valueOf(findRawSubArrayIndex(s, true));
	}

	public String RawSubArrayEndIndex()
	{
		String s = RawDataTypeCell();
		return String.valueOf(findRawSubArrayIndex(s, false));
	}

	public int findRawSubArrayIndex(String s, boolean startOrEnd)
	{
		int end = s.indexOf("]");
		if(end == -1)
			throw new DesignError(
					"In Raw Datatype column of monitor spreadsheet, subarray notation does not contain \"]\" ("
							+ s + ")");
		int start = s.indexOf("[") + 1;
		if(start < 1)
			throw new DesignError(
					"In Raw Datatype column of monitor spreadsheet, subarray notation does not start with \"[\" ("
							+ s + ")");

		String subString = s.substring(start, end);
		String interpretStrings[] = subString.split("-");
		if(interpretStrings.length != 2)
			throw new DesignError(
					"In Raw Datatype column of monitor spreadsheet, subarray notation does contain a range ("
							+ s + ")");
		int index = -1;
		try
		{
			if(startOrEnd == true)
				index = Integer.parseInt(interpretStrings[0]);
			else
				index = Integer.parseInt(interpretStrings[1]);
		}
		catch(NumberFormatException err)
		{
			throw new DesignError(
					"In Raw Datatype column of monitor spreadsheet, subarray notation does not contain an integer as index ("
							+ s
							+ ", "
							+ interpretStrings[0]
							+ interpretStrings[1] + ")");
		}
		return index;
	}

	public String WorldDataToDatabaseTypeUpper1()
	{
		String s = WorldDataToDatabaseType();
		return Character.toUpperCase(s.charAt(0)) + s.substring(1);
	}

	public String WorldDataTypeUpper1()
	{
		String s = WorldDataType();
		return Character.toUpperCase(s.charAt(0)) + s.substring(1);
	}
}
