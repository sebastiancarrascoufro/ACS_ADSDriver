package alma.Control.datamodel.meta.amb;

import alma.Control.datamodel.meta.base.ControlPoint;
import alma.Control.datamodel.meta.base.Table;
import alma.Control.datamodel.meta.base.Util;

import org.openarchitectureware.core.meta.core.Element;
import org.openarchitectureware.core.constraint.DesignError;

public class ControlImpl extends ControlPoint {
	MandCImpl mac;
	public ControlImpl(String[] row, Element parent) {
		super(row, parent);
		mac = new MandCImpl(row,sheet,this);
	}
	public String Data(){
		return row[Table.getColNum(sheet, "Data")];
	}
	public String Value(){
		return row[Table.getColNum(sheet, "Value")];
	}
	public String Returns(){
		return row[Table.getColNum(sheet, "Returns")];
	}
	public String Parameter(){
		return row[Table.getColNum(sheet, "Parameter")];
	}

	public String Assembly(){
		return mac.Assembly();
	}
	public String RCA(){
		return mac.RCA();
	}
	public String RawDataType(){
		return mac.RawDataType();
	}
	public boolean TeRelated(){
		return mac.TeRelated();
	}
	public String WorldDataType(){
		return mac.WorldDataType();
	}
	public String Scale(){
		return mac.Scale();
	}
	public String Offset(){
		return mac.Offset();
	}
	public String RawDataToCPPType()
	{
		return mac.RawDataToCPPType();
	}
	public String WorldDataToCPPType()
	{
		return mac.WorldDataToCPPType();
	}
	public String WorldDataToCORBAType()
	{
		return mac.WorldDataToCORBAType();
	}
	public String WorldDataToIDLSeqType()
	{
		return mac.WorldDataToIDLSeqType();
	}
	public String WorldDataToIDLType()
	{
		return mac.WorldDataToIDLType();
	}
	public String WorldDataToCORBASeqType()
	{
		return mac.WorldDataToCORBASeqType();
	}
	public String WorldDataToCORBADevIOType()
	{
		return mac.WorldDataToCORBADevIOType();
	}
	public String WorldDataToJavaType()
	{
		return mac.WorldDataToJavaType();
	}
	public String WorldDataToDatabaseType()
	{
		return mac.WorldDataToDatabaseType();
	}
	public String WorldDataToBACIType()
	{
		return mac.WorldDataToBACIType();
	}
	public boolean isRawDataArray()
	{
		return mac.isRawDataArray();
	}
	public boolean isWorldDataArray()
	{
		return mac.isWorldDataArray();
	}
	public String MinRange() {
		return Util.normalizeNumber(WorldDataType(), super.MinRange());
	}
	public String MaxRange() {
		return Util.normalizeNumber(WorldDataType(), super.MaxRange());
	}

	public String idlReturns()
	{
		return (String)Table.worldToIDL.get(Returns());
	}
	public String corbaReturns()
	{
		return (String)Table.worldToCORBA.get(Returns());
	}
	public String idlDeclaration() {
		String s = idlSignature();
		return s;
	}
	public String idlSignature() {
		String s = "SET_" + CPName();
		if (!Parameter().equals("default")) {
			if (Parameter().equals("void"))
				s += "()";
			else
				s += "(" + idlParameters() + ")";
			if (External()) {
				s +=  " raises(ControlExceptions::CAMBErrorEx, ControlExceptions::INACTErrorEx)";
			}
			return s;
			// This gets the non-default case out of the way.
		}
		// This is the "default" case.
		boolean isTeRelated = TeRelated();
		boolean isExternal = External();
		boolean isArray = isWorldDataArray();
		if (isExternal) {
			if (isArray) {
				s += "(in " + WorldDataToIDLSeqType() + " world";
			} else {
				s += "(in " + WorldDataToIDLType() + " world";
			}
		}
		if (isTeRelated) {
			s += ", in ACS::Time requestTime)";
		} else {
			s += ")";
		}
		if (isExternal) {
			s += " raises(ControlExceptions::CAMBErrorEx, ControlExceptions::INACTErrorEx)";
		}
		return s;
	}
    private String idlParameters() {
        String s = Parameter();
        int pos = 0;
        int n = s.indexOf(' ');
        if (n == -1)
            throw new DesignError("Invalid syntax in Parameter field: (" + s + ")");
        String word = s.substring(pos,n);
        String out = "";
        boolean isArray = isWorldDataArray();
        if (isArray)
        {
            out += "in " + (String)Table.worldToIDLSeq.get(word) + " ";
        }
        else
            out = "in " + (String)Table.worldToIDL.get(word) + " ";
        pos = n + 1;
        while (true) {
            n = s.indexOf(',',pos);
            if (n == -1)
                break;
            word = s.substring(pos,n);
            out += word + ", ";
            pos = n + 1;
            while (true) {
                if (s.charAt(pos) == ' ')
                    pos++;
                else
                    break;
            }
            n = s.indexOf(' ',pos);
            if (n == -1)
                throw new DesignError("Invalid syntax in Parameter field: (" + s + ")");
            word = s.substring(pos,n);
            if (isArray)
            {
                String type = (String)Table.worldToIDL.get(word);
                type = type.replace("unsigned long", "uLong");
                out += "in " + type;
            }
            else
                out = "in " + (String)Table.worldToIDL.get(word) + " ";
            pos = n + 1;
        }
        word = s.substring(pos);
        out += word;
        return out;
    }
    public String corbaDeclaration() {
        String s = "";
        if (!Parameter().equals("default")) {
            if (Parameter().equals("void"))
                s += "()";
            else
                s += "(" + corbaParameters() + ")";
            return s;
            // This gets the non-default case out of the way.
        }
        // This is the "default" case.
        boolean isTeRelated = TeRelated();
        boolean isArray = isWorldDataArray();
        if(isArray == true)
        {
            s += "(const " + WorldDataToCORBASeqType() + "& world";
        }
        else
        {
            String type = WorldDataToCORBAType();
            if(type.equalsIgnoreCase("CORBA::String") == true)
            {
                type = "const " + type + "_var&";
            }

            s += "(" + type + " world";
        }
        if (isTeRelated)
        {
            s += ", const acstime::Epoch& requestTime)";
        }
        else
        {
            s += ")";
        }
        return s;
    }
    private String corbaParameters() {
        String s = Parameter();
         int pos = 0;
        int n = s.indexOf(' ');
        if (n == -1)
            throw new DesignError("Invalid syntax in Parameter field: (" + s + ")");
        String word = s.substring(pos,n);
        String out = (String)Table.worldToCORBA.get(word) + " ";
        pos = n + 1;
        while (true) {
            n = s.indexOf(',',pos);
            if (n == -1)
                break;
            word = s.substring(pos,n);
            out += word + ", ";
            pos = n + 1;
            while (true) {
                if (s.charAt(pos) == ' ')
                    pos++;
                else
                    break;
            }
            n = s.indexOf(' ',pos);
            if (n == -1)
                throw new DesignError("Invalid syntax in Parameter field: (" + s + ")");
            word = s.substring(pos,n);
            out += (String)Table.worldToCORBA.get(word) + " ";
            pos = n + 1;
        }
        word = s.substring(pos);
        out += word;
        return out;
    }
    public String cppReturns() {
        return (String)Table.worldToCPP.get(Returns());
    }
    public String cppDeclaration() {
        String s = "";
        if (!Parameter().equals("default")) {
            if (Parameter().equals("void"))
                s += "()";
            else
                s += "(" + cppParameters() + ")";
            return s;
            // This gets the non-default case out of the way.
        }
        // This is the "default" case.
        boolean isTeRelated = TeRelated();
        boolean isArray = isWorldDataArray();
        if (isArray) {
            s += "(const std::vector< " + WorldDataToCPPType() + " >& world";
        } else {
            s += "(const " + WorldDataToCPPType() + " world";
        }
        if (isTeRelated) {
            s += ", const acstime::Epoch& requestTime)";
        } else {
            s += ")";
        }
        return s;
    }
    private String cppParameters() {
        String s = Parameter();
        int pos = 0;
        int n = s.indexOf(' ');
        if (n == -1)
            throw new DesignError("Invalid syntax in Parameter field: (" + s + ")");
        String word = s.substring(pos,n);
        String out = (String)Table.worldToCPP.get(word) + " ";
        pos = n + 1;
        while (true) {
            n = s.indexOf(',',pos);
            if (n == -1)
                break;
            word = s.substring(pos,n);
            out += word + ", ";
            pos = n + 1;
            while (true) {
                if (s.charAt(pos) == ' ')
                    pos++;
                else
                    break;
            }
            n = s.indexOf(' ',pos);
            if (n == -1)
                throw new DesignError("Invalid syntax in Parameter field: (" + s + ")");
            word = s.substring(pos,n);
            out += (String)Table.worldToCPP.get(word) + " ";
            pos = n + 1;
        }
        word = s.substring(pos);
        out += word;
        return out;
    }
    public String argList() {
        if (!Parameter().equals("default")) {
            if (Parameter().equals("void"))
                return "";
            else
                return argWords();
            // This gets the non-default case out of the way.
        }
        // This is the "default" case.
        String s = "world";
        boolean isTeRelated = TeRelated();
        if (isTeRelated) {
            s += ", requestTime";
        }
        return s;
    }
    private String argWords() {
        String s = Parameter();
        int pos = 0;
        int n = s.indexOf(' ');
        if (n == -1)
            throw new DesignError("Invalid syntax in Parameter field: (" + s + ")");
        String word = s.substring(pos,n);
        String out = "";
        pos = n + 1;
        while (true) {
            n = s.indexOf(',',pos);
            if (n == -1)
                break;
            word = s.substring(pos,n);
            out += word + ", ";
            pos = n + 1;
            while (true) {
                if (s.charAt(pos) == ' ')
                    pos++;
                else
                    break;
            }
            n = s.indexOf(' ',pos);
            if (n == -1)
                throw new DesignError("Invalid syntax in Parameter field: (" + s + ")");
            word = s.substring(pos,n);
            pos = n + 1;
        }
        word = s.substring(pos);
        out += word;
        return out;
    }

	public boolean isConversion()
	{
		return mac.isConversion();
	}
	public boolean isSpecialConversion()
	{
		return mac.isSpecialConversion();
	}
	public String NumberItemsRawData()
	{
		return mac.NumberItemsRawData();
	}
	public String TotalBytesRawData()
	{
		return mac.TotalBytesRawData();
	}
	public String NumberRawDataTypeBytes()
	{
		return mac.NumberRawDataTypeBytes();
	}
	public String NumberItemsWorldData()
	{
		return mac.NumberItemsWorldData();
	}
	public String RCACell(){
		return mac.RCACell();
	}
	public String TeRelatedCell(){
		return mac.TeRelatedCell();
	}
	public boolean isDependentGroupBit(){
		return mac.isDependentGroupBit();
	}
	public boolean isDependentBit(){
		return mac.isDependentBit();
	}
	public boolean isDependentElement(){
		return mac.isDependentElement();
	}
	public boolean isByteSwapped(){
		return mac.isByteSwapped();
	}
	public boolean isDependentBitElement(){
		return mac.isDependentBitElement();
	}
	public boolean isDependentArrayElement(){
		return mac.isDependentArrayElement();
	}
	public String GetDimension(){
		return mac.GetDimension();
	}
	public boolean isSingleBit(){
		return mac.isSingleBit();
	}
	public String getBit(){
		return mac.getBit();
	}
	public String getMask(){
		return mac.getMask();
	}
	public String getFirstBit(){
		return mac.getFirstBit();
	}
	public String getLastBit(){
		return mac.getLastBit();
	}
	public String getGroupMask() {
		return mac.getGroupMask();
	}
	public String getGroupEnd() {
		return mac.getGroupEnd();
	}
	public String getGroupStart() {
		return mac.getGroupStart();
	}
	public String RawSubArrayEndIndex(){
		return mac.RawSubArrayEndIndex();
	}
	public String RawSubArrayStartIndex(){
		return mac.RawSubArrayStartIndex();
	}
	public boolean isRawSubArray(){
		return mac.isRawSubArray();
	}
	public String WorldDataToDatabaseTypeUpper1() {
		return mac.WorldDataToDatabaseTypeUpper1();
	}
	public String WorldDataTypeUpper1() {
		return mac.WorldDataTypeUpper1();
	}
}
