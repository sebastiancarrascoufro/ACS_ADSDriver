package alma.Control.datamodel.meta.amb;

import alma.Control.datamodel.meta.base.ArchiveProperty;
import alma.Control.datamodel.meta.base.Table;

public class ArchiveImpl extends ArchiveProperty{
	public ArchiveImpl(String[] row){
		super(row);
	}
	public String Assembly(){
		return row[Table.getColNum(sheet, "Assembly")];
	}
   public boolean isIntervalTE() {
       String s = IntervalFull();
       if (s.startsWith("te/"))
           return true;
       return false;
   }
	public String Interval(){
		if(isIntervalTE())
			return IntervalFull().substring(3);
		return IntervalFull();
	}
}
