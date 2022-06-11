package alma.Control.datamodel.meta.amb;

import alma.Control.datamodel.meta.base.MainBase;
import alma.Control.datamodel.meta.base.Table;

public class MainImpl extends MainBase{
	public MainImpl(String[] row){
		super(row);
	}
	public String Parent() {
		return row[Table.getColNum(sheet, "Parent")];
	}
	public String Cardinality() {
		return row[Table.getColNum(sheet, "Cardinality")];
	}
	public String NodeAddress() {
		return row[Table.getColNum(sheet, "Node Address")];
	}
	public String Channel() {
		return row[Table.getColNum(sheet, "Channel")];
	}
	public String BaseAddress() {
		return row[Table.getColNum(sheet, "Base Address")];
	}
	public boolean GenericMonitorPoints() {
		return row[Table.getColNum(sheet, "Generic Monitor Points")].equals("yes");
	}
}
