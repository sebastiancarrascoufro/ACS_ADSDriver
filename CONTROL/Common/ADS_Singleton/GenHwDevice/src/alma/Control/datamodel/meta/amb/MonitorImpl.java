package alma.Control.datamodel.meta.amb;

import java.util.Iterator;

import alma.Control.datamodel.meta.base.MonitorPoint;
import alma.Control.datamodel.meta.base.Table;
import alma.Control.datamodel.meta.base.Util;

import org.openarchitectureware.core.meta.core.Element;

public class MonitorImpl extends MonitorPoint {
	MandCImpl mac;
	public MonitorImpl(String[] row, Element parent) {
		super(row, parent);
		mac = new MandCImpl(row, sheet, this);
	}
	public String CanBeInvalid(){
		return row[Table.getColNum(sheet, "Can Be Invalid")];
	}
   public boolean isFEStatus() {
	   return CanBeInvalid().equals("yes-feStatus");
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
    public String OnChange() {
        return mac.OnChange();
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
	public String RCACell(){
		return mac.RCACell();
	}
	public String NumberItemsWorldData(){
		return mac.NumberItemsWorldData();
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
	public String MinRange() {
		return Util.normalizeNumber(WorldDataType(), super.MinRange());
	}
	public String MaxRange() {
		return Util.normalizeNumber(WorldDataType(), super.MaxRange());
	}
	public String Default(){
		return Util.normalizeNumber(WorldDataType(), super.Default());
	}
	
	public boolean isPattern(){
		boolean ret = true;
		if( this.isDependent()) ret = false;
		if( this.getDependents().isEmpty() ) ret = false;
		for (Iterator<Element> iter = this.getDependents().iterator(); iter.hasNext(); ) {
			MonitorImpl var = (MonitorImpl) iter.next();
		    if( var.WorldDataType().compareTo("boolean") != 0 ) ret = false;
		}
		return ret;
	}
/*
	public boolean isArchived() {
		//FIXME Overriding method to exclude unsupported special case (see COMP-4995).
		if (WorldDataType().equals("uint"))
			return false;
		return true;
	}
*/
	@Override
	public boolean isPartOfPattern() {
		boolean ret = false;
		if( isDependent() ){
			MonitorImpl parent = ((MonitorImpl)this.getParent());
			if( parent.isPattern() )
				ret = true;
		}
		return ret;
	}
	@Override
	public boolean isHomogeneous() {
		boolean ret = true;
		String firsttype = null;
		if( hasDependents() ){
			for (Iterator<Element> iter = this.getDependents().iterator(); iter.hasNext(); ) {
				MonitorImpl var = (MonitorImpl) iter.next();
				if( firsttype == null){
					firsttype = new String(var.WorldDataType());
					continue;
				}
			    if( var.WorldDataType().compareTo(firsttype) != 0 ) ret = false;
			}
		}else{
			ret = false;
		}
		return ret;
	}
	@Override
	public boolean isPartOfHomogeneous() {
		boolean ret = false;
		if( isDependent() ){
			MonitorImpl parent = ((MonitorImpl)this.getParent());
			if( parent.isHomogeneous() )
				ret = true;
		}
		return ret;
	}


}
