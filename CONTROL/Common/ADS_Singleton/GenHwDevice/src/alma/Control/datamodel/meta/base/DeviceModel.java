package alma.Control.datamodel.meta.base;

import org.openarchitectureware.core.meta.core.Element;
import org.openarchitectureware.core.meta.core.ElementSet;
import org.openarchitectureware.core.meta.core.Model;
import org.openarchitectureware.core.constraint.DesignError;

import java.io.File;
import java.util.Iterator;
import java.util.Map;

public abstract class DeviceModel extends Model {
	// The following designates sheets in the spreadsheet.
	private static final String NOTES = "Notes";

	// The following designate columns in the first sheet of the spreadsheet.
	protected static int DescriptionIndex = 1;

	protected String[][][] spreadsheet;
	protected MainBase main;
	protected ElementSet notes;
	protected ElementSet monitorPoints;
	protected ElementSet controlPoints;
	protected ElementSet archiveProperties;

	protected String deviceName;
	protected String busType;
	protected String deviceDir;
	protected String codegenDir;
	protected String spreadsheetDir;
	protected String generatedDir;
	protected boolean monitorDBOnly;
	protected boolean generateAlt;

	public abstract String CreateModel();

	public DeviceModel () {
		// Set the basic information from the properties.
		this.setDeviceName(System.getProperty("deviceName"));
		this.setDeviceDir(System.getProperty("deviceDir"));
		this.setCodegenDir(System.getProperty("codegenDir"));
		this.setSpreadsheetDir(System.getProperty("spreadsheetDir"));
		this.setGeneratedDir(System.getProperty("generatedDir"));
	}
	public String getCodegenDir() {
		return codegenDir;
	}
	public void setCodegenDir(String codegenDir) {
		this.codegenDir = codegenDir;
	}
	public String getDeviceDir() {
		return deviceDir;
	}
	public void setDeviceDir(String deviceDir) {
		this.deviceDir = deviceDir;
	}
	public String getDeviceName() {
		return deviceName;
	}
	public void setDeviceName(String deviceName) {
		this.deviceName = deviceName;
	}
	public void setBusType(String busType){
		this.busType = busType;
	}
	public String getGeneratedDir() {
		return generatedDir;
	}
	public void setGeneratedDir(String generatedDir) {
		this.generatedDir = generatedDir;
	}
	public String getSpreadsheetDir() {
		return spreadsheetDir;
	}
	public void setSpreadsheetDir(String spreadsheetDir) {
		this.spreadsheetDir = spreadsheetDir;
	}
	public String CheckConstraints() {
		return "";
	}
	protected void initializeModelDependencies() {
	}
	//Only there because of generalization.
	public void setGenerateAlt(boolean generateAlt) {
		this.generateAlt = generateAlt;
	}
	public void setMonitorDBOnly(boolean monitorDBOnly) {
		this.monitorDBOnly = monitorDBOnly;
	}
	///////////
	public String[][][] getSpreadsheet() {
		return spreadsheet;
	}
	public ElementSet Notes() {
		return notes;
	}
	public ElementSet MonitorPoint() {
		return monitorPoints;
	}
	public ElementSet ControlPoint() {
		return controlPoints;
	}
	public ElementSet ArchiveProperty() {
		return archiveProperties;
	}
	public String Assembly(){
		return main.Assembly();
	}
	public String Extends(){
		return main.Extends();
	}
	public String DeviceName() {
		return main.DeviceName();
	}
	public String Description() {
		return main.Description();
	}
	public String DescriptionAsString() {
		return main.DescriptionAsString();
	}
	public String ICD(){
		return main.ICD();
	}
	public String ICDDate(){
		return main.ICDDate();
	}
	public String ICDDateAsDatabaseDate() {
		return main.ICDDateAsDatabaseDate();
	}
	public String ICDDateAsArrayTime() {
		return main.ICDDateAsArrayTime();
	}
	public String DirPath() {
		return Assembly();
	}
	public String TheEnd() {
                String dir = generatedDir + "/" + Assembly();
                Util.RemoveLinesFromFile(dir + "/src", Assembly() + "Base.cpp", 1);
                Util.RemoveLinesFromFile(dir + "/include", Assembly() + "Base.h", 1);
                Util.RemoveLinesFromFile(dir + "/idl", Assembly() + "Base.idl", 1);
                Util.RemoveLinesFromFile(dir + "/idl", Assembly() + "Add.sql", 1);
                Util.RemoveLinesFromFile(dir + "/doc", Assembly() + ".xml", 1);
                Util.RemoveLinesFromFile(dir + "/doc", Assembly() + "Component.xml", 1);
                Util.RemoveLinesFromFile(dir + "/config/CDB/schemas", Assembly() + "Base.xsd", 1);
                Util.RemoveLinesFromFile(dir + "/config", "TMCDB" + Assembly() + "Add.xml", 1);
                Util.RemoveLinesFromFile(dir + "/src/CCL", Assembly() + "Base.py", 1);
                Util.RemoveLinesFromFile(dir + "/src/CCL", "__init__.py", 1);
                System.out.println("Code generation for " + deviceName + " done.");
                return "";
    }

    public boolean checkIfFileExists(String fileName)
    {
        java.io.File f = new java.io.File(fileName);

        if(f.exists() == true)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    public boolean checkIfFileExists(String directory, String fileName)
    {
        if(directory.isEmpty() == false)
        {
            return checkIfFileExists(directory + "/" + fileName);
        }
        else
        {
            return checkIfFileExists(fileName);
        }
    }

    public boolean checkIfDeviceFileExists(String directory,
            String localFileName)
    {
        String fileName = getDeviceDir() + "/" + directory + "/" + Assembly()
                + localFileName;
        return checkIfFileExists(fileName);
    }

    public boolean checkIfTestFileExists(String directory, String localFileName)
    {
        String fileName = getDeviceDir() + "/" + directory + "/Test"
                + Assembly() + localFileName;
        return checkIfFileExists(fileName);
    }

    public boolean checkIfCvsignoreFileExists(String directory)
    {
        String fileName = getDeviceDir() + "/" + directory + "/.cvsignore";
        return checkIfFileExists(fileName);
    }

    protected Element getArchive(String name)
    {
        // Get the Archive Properties
        int archiveIndex = Table.getSheetNum("Archive Property");
        for(int i = 2; i < spreadsheet[archiveIndex].length; i++)
        {
            if(spreadsheet[archiveIndex][i].length == 0)
                break;
            ArchiveProperty ap;
            String[] row = spreadsheet[archiveIndex][i];
            ap = new ArchiveProperty(row);
            if(name.equals(ap.RefersTo()))
                return ap;
        }
        return null;
    }

    protected MonitorPoint getMonitorPoint(String fullName)
    {
        for(int i = 0; i < monitorPoints.size(); i++)
        {
            MonitorPoint mp = (MonitorPoint) monitorPoints.get(i);
            if(mp.FullName().equals(fullName))
                return mp;
        }
        return null;
    }

    protected ControlPoint getControlPoint(String fullName) {
        for (int i = 0; i < controlPoints.size(); i++) {
            ControlPoint cp = (ControlPoint) controlPoints.get(i);
            if (cp.FullName().equals(fullName))
                return cp;
        }
        return null;
    }
}
