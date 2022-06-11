package alma.Control.datamodel.meta.base;

public class MainBase extends org.openarchitectureware.meta.uml.classifier.Class{
	protected String[] row;
	protected int sheet;
	public MainBase(String[] row){
		this.row = row;
		this.sheet = Table.getSheetNum("Hardware Device");
	}
	public String Assembly() {
		return row[Table.getColNum(sheet, "Assembly")];
	}
	public String Description() {
		return row[Table.getColNum(sheet, "Description")];
	}
	public String DescriptionAsString() {
		return Util.descriptionAsString(Description());
	}
	public String DeviceName() {
		return row[Table.getColNum(sheet, "Device Name")];
	}
	public String Extends() {
		return row[Table.getColNum(sheet, "Extends")];
	}
	public String ICD() {
		return row[Table.getColNum(sheet, "ICD")];
	}
	public String ICDDate() {
		return row[Table.getColNum(sheet, "ICD Date")];
	}
	public String ICDDateAsDatabaseDate() {
		return Util.toDatabaseDate(ICDDate());
	}
	public String ICDDateAsArrayTime() {
		return Util.toArrayTime(ICDDate());
	}
}
