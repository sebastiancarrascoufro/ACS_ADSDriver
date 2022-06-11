package alma.Control.datamodel.meta.amb;

import org.openarchitectureware.core.constraint.DesignError;
import org.openarchitectureware.core.meta.core.ElementSet;

import alma.Control.datamodel.meta.base.ControlPoint;
import alma.Control.datamodel.meta.base.MonitorPoint;
import alma.Control.datamodel.meta.base.Note;
import alma.Control.datamodel.meta.base.SpreadsheetParser;
import alma.Control.datamodel.meta.base.SpreadsheetValidator;
import alma.Control.datamodel.meta.base.Table;
import alma.Control.datamodel.meta.base.Util;

public class DeviceModel extends alma.Control.datamodel.meta.base.DeviceModel {

	public DeviceModel(){
		super();
	}
	public boolean IsMonitorDBOnly() {
		return monitorDBOnly;
	}
	public boolean IsGenerateAlt() {
		return generateAlt;
	}
	public String Parent() {
		return ((MainImpl)main).Parent();
	}
	public String Cardinality() {
		return ((MainImpl)main).Cardinality();
	}
	public String NodeAddress() {
		return ((MainImpl)main).NodeAddress();
	}
	public String Channel() {
		return ((MainImpl)main).Channel();
	}
	public String BaseAddress() {
		return ((MainImpl)main).BaseAddress();
	}
	public boolean GenericMonitorPoints() {
		return ((MainImpl)main).GenericMonitorPoints();
	}

	public String CreateModel() {
		// Parse and validate the spreadsheet.
		int i;
		int mainIndex;
		int monitorIndex;
		int controlIndex;
		int archiveIndex;

		// Check if the spreadsheet file is an actual spreadsheet or a
		// "filter" file which is some kind of a filter for a real
		// spreadsheet but inherits from it.
		String xmlFile = spreadsheetDir + "/" + deviceName + "_spreadsheet.";
		String suffix = "filter";
		java.io.File fileExists = new java.io.File(xmlFile + suffix);
		if(fileExists.exists() == false)
		{
			// The blah.filter file does not exist, so assume that the file
			// is an XML file.
			suffix = "xml";
		}

		xmlFile += suffix;
		String xml = SpreadsheetParser.getSpreadsheet(spreadsheetDir, deviceName + "_spreadsheet." + suffix);
		String xsdFile = Util.getInstallDir() + "/config/schemas/" + busType.toLowerCase() + "/Workbook.xsd";

		SpreadsheetParser p = new SpreadsheetParser(xml);
		p.setXSD(xsdFile);
		spreadsheet = p.getWorksheets();
		SpreadsheetValidator v = new SpreadsheetValidator();
		if (!p.isReference() && !v.validate(xmlFile, xsdFile)) {
				String s = "Spreadsheet " + deviceName + "_spreadsheet." + suffix + " is not a valid spreadsheet.";
				throw new DesignError(s);
		}
		System.out.println("Spreadsheet " + Name() + " has been validated.");

		Table.initialize(spreadsheet);
		mainIndex = Table.getSheetNum("Hardware Device");
		monitorIndex = Table.getSheetNum("Monitor Point");
		controlIndex = Table.getSheetNum("Control Point");
		archiveIndex = Table.getSheetNum("Archive Property");

		if(spreadsheet[mainIndex][2][Table.getColNum(mainIndex,"Generic Monitor Points")].equals("yes")){
			String[][][] spreadsheetWithGenericPointsAdded;
			GenericMonitorPoints generic = new GenericMonitorPoints();
			spreadsheetWithGenericPointsAdded = generic.getDeviceWorksheetWithGenericPointsAdded(spreadsheet);
			spreadsheet=spreadsheetWithGenericPointsAdded;
		}

		// Get the Main
		main = new MainImpl(spreadsheet[mainIndex][2]);
		main.setMetaEnvironment(this.getMetaEnvironment());

		// Get the Notes
		notes = new ElementSet();
		for (i = 3; i < spreadsheet[mainIndex].length; ++i) {
			Note note = new Note();
			note.setMetaEnvironment(this.getMetaEnvironment());
			note.setNote(spreadsheet[mainIndex][i][DescriptionIndex]);
			notes.add(note);
		}

		// Get the monitor points
		MonitorImpl mparent = null;
		monitorPoints = new ElementSet();
		for (i = 2; i < spreadsheet[monitorIndex].length; ++i) {
			if(spreadsheet[monitorIndex][i].length == 0)
				break;
			MonitorImpl mp;
			String[] row = spreadsheet[monitorIndex][i];
			if(!spreadsheet[monitorIndex][i][1].startsWith(Table.DepChar)){
				mp = new MonitorImpl(row, null);
				mparent = mp;
			}
			else{
				mp = new MonitorImpl(row, mparent);
				mparent.addDependent(mp);
			}
			mp.setArchive(getArchive(mp.FullName()));
			mp.setMetaEnvironment(this.getMetaEnvironment());
			monitorPoints.add(mp);
		}

		// Define undefined dependent monitor points for sequence properties
		MonitorImpl[] list = (MonitorImpl[])monitorPoints.toArray(new MonitorImpl[0]);
		for (MonitorImpl mp : list) {
			if (mp.isWorldDataArray() && !mp.hasDependents())
				for (i=0; i < Integer.parseInt(mp.NumberItemsWorldData()); i++) {
					String[] row = {
					  mp.Assembly(),
					  "^" + mp.MPName() + "_" + Integer.toString(i),
					  mp.RCA(),
					  mp.RawDataType(),
					  mp.TeRelatedCell(),
                      mp.OnChange(),
					  mp.WorldDataType(),
					  mp.DataUnits(),
					  "none",
					  "none",
					  "none",
					  "none",
					  "none",
					  mp.ErrorCondition(),
					  mp.ErrorSeverity(),
					  mp.ErrorAction(),
					  mp.Mode(),
					  "yes",
					  "no",
					  mp.CanBeInvalid(),
					  mp.Description() + " (dependent monitor point)"
					};
					MonitorImpl dep = new MonitorImpl(row, mp);
					mp.addDependent(dep);

					dep.setArchive(getArchive(dep.FullName()));
					dep.setMetaEnvironment(this.getMetaEnvironment());
					monitorPoints.add(dep);
				}
		}

		// Get the control points
		ControlImpl cparent = null;
		controlPoints = new ElementSet();
		for (i = 2; i < spreadsheet[controlIndex].length; ++i) {
			if(spreadsheet[controlIndex][i].length == 0)
				break;
			ControlImpl cp;
			String[] row = spreadsheet[controlIndex][i];
			if(!spreadsheet[controlIndex][i][1].startsWith(Table.DepChar))
			{
				cp = new ControlImpl(row, null);
				cparent = cp;
			}
			else{
				cp = new ControlImpl(row, cparent);
				cparent.addDependent(cp);
			}
			cp.setArchive(getArchive(cp.FullName()));
			cp.setMetaEnvironment(this.getMetaEnvironment());
			controlPoints.add(cp);
		}

		//Get the Archive Properties
		archiveProperties = new ElementSet();
		for(i = 2; i < spreadsheet[archiveIndex].length; i++) {
			if(spreadsheet[archiveIndex][i].length == 0)
				break;
			ArchiveImpl ap;
			String[] row = spreadsheet[archiveIndex][i];
			ap = new ArchiveImpl(row);
                        MonitorPoint mp = getMonitorPoint(ap.RefersTo());
                        if (mp != null)
                                ap.setMP(mp);
                        ControlPoint cp = getControlPoint(ap.RefersTo());
                        if (cp != null)
                                ap.setCP(cp);
			archiveProperties.add(ap);
		}

		System.out.println("DeviceModel: Initialization complete.");
		return "";
	}
	public String TheEnd() {
		String dir = generatedDir + "/" + Assembly();
		Util.RemoveLinesFromFile(dir + "/doc", Assembly() + ".html", 1);
		Util.RemoveLinesFromFile(dir + "/idl", Assembly() + "Add.sql", 1);
		Util.RemoveLinesFromFile(dir + "/include", Assembly() + "HWSimBase.h", 1);
		Util.RemoveLinesFromFile(dir + "/src", Assembly() + "HWSimBase.cpp", 1);
		Util.RemoveLinesFromFile(dir + "/include", Assembly() + "CompSimBase.h", 1);
		Util.RemoveLinesFromFile(dir + "/src", Assembly() + "CompSimBase.cpp", 1);
		Util.RemoveLinesFromFile(dir + "/idl", Assembly() + "CompSimBase.idl", 1);
		Util.RemoveLinesFromFile(dir + "/test", "Test" + Assembly() + "HWSimImpl.cpp", 1);
		Util.RemoveLinesFromFile(dir + "/test", "Test" + Assembly() + "AmbDeviceInt.cpp", 1);
		if (!IsMonitorDBOnly()) {
			if (IsGenerateAlt()) {
				Util.RemoveLinesFromFile(dir + "/../ALT/", Assembly() + ".makefile", 1);
				Util.RemoveLinesFromFile(dir + "/../ALT/", Assembly() + "Impl.idl", 1);
				Util.RemoveLinesFromFile(dir + "/../ALT/", Assembly() + "Impl.h", 1);
				Util.RemoveLinesFromFile(dir + "/../ALT/", Assembly() + "Impl.cpp", 1);
			}
			super.TheEnd();
		} else 
			System.out.println("Code generation for " + deviceName + " done.");
		return "";
	}
	public String NodeAddressInDecimal() {
		String s = NodeAddress();
		if (s.equals("parm"))
			return "0";
		return HexToDec(s);
	}
	private String HexToDec(String s) {
		int n = -1;
		try {
			n = Integer.parseInt(s.substring(2),16);
		} catch (NumberFormatException err) {
			throw new DesignError("Invalid number (" + s + ")");
		}
		return Integer.toString(n);

	}
}
