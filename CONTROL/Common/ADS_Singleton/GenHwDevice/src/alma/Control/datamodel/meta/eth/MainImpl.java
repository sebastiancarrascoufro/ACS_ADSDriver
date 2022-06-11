package alma.Control.datamodel.meta.eth;

import alma.Control.datamodel.meta.base.MainBase;
import alma.Control.datamodel.meta.base.Table;

public class MainImpl extends MainBase{
	public MainImpl(String[] row){
		super(row);
	}
	public String VendorClass() {
		return row[Table.getColNum(sheet, "Vendor Class")];
	}
}
