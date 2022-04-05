package alma.Control.datamodel.meta.eth;

import alma.Control.datamodel.meta.base.Table;

public class MandCImpl {
	protected String[] row;
	protected int sheet;
	protected String assembly;
	public MandCImpl(String[] row, int sheet){
		this.row = row;
		this.sheet = sheet;
	}
	public void setAssemblyName(String assembly)
	{
		this.assembly = assembly;
	}
	public String Assembly()
	{
		return assembly;
	}
	public String Address(){
		return row[Table.getColNum(sheet, "Address")];
	}
    public String OnChange(){
        return row[Table.getColNum(sheet, "On Change")];
    }
	public String DataType(){
		String s = row[Table.getColNum(sheet, "Data Type")];
		return isDataArray() ? s.substring(0,s.indexOf("[")) : s;
	}
	public boolean isDataArray()
	{
		return row[Table.getColNum(sheet, "Data Type")].endsWith("]") ? true : false;
	}
	public boolean isOpenArray()
	{
		if(!isDataArray()) return false;
		String s = row[Table.getColNum(sheet, "Data Type")];
		return (s.indexOf("]") - s.indexOf("[") == 1) ? true : false;
	}
	public String getArrayLength()
	{
		if(!isDataArray()) return "1";
		String s = row[Table.getColNum(sheet, "Data Type")];
		return s.substring(s.indexOf("[")+1,s.indexOf("]"));
	}
	public String DataToCPPType()
	{
		return (String)Table.toCPP.get(DataType());
	}
	public String DataToIDLType()
	{
		return (String)Table.toIDL.get(DataType());
	}
	public String DataToIDLSeqType()
	{
		return (String)Table.toIDLSeq.get(DataType());
	}
	public String DataToCORBAType()
	{
		return (String)Table.toCORBA.get(DataType());
	}
	public String DataToCORBADevIOType()
	{
		return (String)Table.toCORBADevIO.get(DataType());
	}
	public String DataToCORBASeqType()
	{
		return (String)Table.toCORBASeq.get(DataType());
	}
	public String DataToBACIType()
	{
		return (String)Table.toBACI.get(DataType());
	}
	public String DataToDatabaseType()
	{
		return (String) Table.toDatabase.get(DataType());
	}
}
