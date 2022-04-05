package alma.Control.datamodel.meta.base;

import org.openarchitectureware.core.meta.core.ElementSet;
import org.openarchitectureware.core.meta.core.Element;
import org.openarchitectureware.core.constraint.DesignError;

public abstract class MandCBase extends org.openarchitectureware.meta.uml.classifier.Class {

	private String spreadsheetName = "Not needed";
	private String sheetName;

	protected String[] row;
	protected String[] parentRow;
	protected int sheet;
	protected Element parent;
	protected Element archive;
	protected ElementSet dependents;

	public MandCBase(){
	}

	public MandCBase(String[] row, Element parent) {
		this.row = row;
		this.parent = parent;
		dependents = new ElementSet();
		sheet = (this instanceof MonitorPoint) ? Table.getSheetNum("Monitor Point") : Table.getSheetNum("Control Point");
		//sheetName = Table.tagNames[sheet];
	}
	public void addDependent(Element son){
		dependents.add(son);
	}
	public void setArchive(Element archive){
		this.archive = archive;
		if(archive != null){
			if(this instanceof MonitorPoint)
				((ArchiveProperty)archive).setMP(this);
			else
				((ArchiveProperty)archive).setCP(this);
		}
	}
	
	public boolean isDependent(){
		if (parent == null)
			return false;
		return true;
	}
	public Element getParent(){
		return parent;
	}
	public boolean hasDependents(){
		return !dependents.isEmpty();
	}
	public ElementSet getDependents(){
		return dependents;
	}
	public String FullName(){
		return row[Table.getColNum(sheet, "Name")];
	}
	public String Name(){
		if(isDependent())
			return row[Table.getColNum(sheet, "Name")].substring(1);
		else
			return row[Table.getColNum(sheet, "Name")];
	}
	public String getDependentName() {
		return Name();
	}
	public String getAltDependentName(){
		return Util.AltName(getDependentName());
	}
	public String getDependsOnName(){
		if(!isDependent())
			return "";
		return ((MandCBase)getParent()).Name();
	}
	public String getAltDependsOnName(){
		return Util.AltName(((MandCBase)getParent()).Name());
	}
	public String DataUnits(){
		return row[Table.getColNum(sheet, "Data Units")];
	}
	public String MinRange(){
		return row[Table.getColNum(sheet, "Min Range")];
	}
	public String MaxRange(){
		return row[Table.getColNum(sheet, "Max Range")];
	}
	public String Mode(){
		return row[Table.getColNum(sheet, "Mode")];
	}
	public boolean Implement(){
		return row[Table.getColNum(sheet, "Implement")].equals("yes");
	}
	public boolean External(){
		//return row[Table.getColNum(sheet, "External")];
		return row[Table.getColNum(sheet, "External")].equals("yes");
	}
	public String Description(){
		return row[Table.getColNum(sheet, "Description")];
	}
	public boolean isExternal(){
		return External();
	}
	
	public boolean isArchived(){
		// Always return true as we want to generate BACI properties for all monitor points.
		//if(archive == null)
		//	return false;
		return true;
	}
	public boolean isImplemented(){
		return Implement();
	}
	protected DesignError dErr(DesignError err, String cell) {
		return new DesignError (spreadsheetName + ":" + sheetName + ":" + cell + ": " + err.toString());
	}
	protected DesignError dErr(String err, String cell) {
		return new DesignError (spreadsheetName + ":" + sheetName + ":" + cell + ": " + err);
	}

	private String dropPrefix(String s) {
		if (s.startsWith("GET_") || s.startsWith("SET_"))
			return s.substring(4);
		return s;
	}
	public String PName() {
		if (Name().startsWith(Table.DepChar))
			return dropPrefix(Name().substring(1));
		return dropPrefix(Name());
	}
	public String AltPName() {
		if (Name().startsWith("SET_"))
			return "Cntl" + Util.AltName(PName());
		return Util.AltName(PName());
	}
	//Descriptions
	public String DescriptionAsString() {
		return Util.descriptionAsString(Description());
	}
	public String DescriptionFormatted() {
		return Util.formatDescriptionL2(Description());
	}
	public String DescriptionFormattedL4() {
		return Util.formatDescriptionL4(Description());
	}
	public String DescriptionFormattedL5() {
		return Util.formatDescriptionL5(Description());
	}
	abstract public boolean isMonitored();
}

