package alma.Control.datamodel.meta.eth;

import org.openarchitectureware.core.meta.core.Element;
import org.openarchitectureware.core.meta.core.ElementSet;
import org.openarchitectureware.core.meta.core.Model;
import org.openarchitectureware.core.constraint.DesignError;

import alma.Control.datamodel.meta.base.*;

public class DeviceModel extends alma.Control.datamodel.meta.base.DeviceModel {
	//private Main main;
	public DeviceModel(){
		super();
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
		spreadsheet = p.getWorksheets();
		SpreadsheetValidator v = new SpreadsheetValidator();
		if (!v.validate(xmlFile, xsdFile)) {
			String s = "Spreadsheet " + deviceName + "_spreadsheet." + suffix + " is not a valid spreadsheet.";
			throw new DesignError(s);
		}
		System.out.println("Spreadsheet " + Name() + " has been validated.");

		Table.initialize(spreadsheet);
		// Get the Main..
		mainIndex = Table.getSheetNum("Hardware Device");
		monitorIndex = Table.getSheetNum("Monitor Point");
		controlIndex = Table.getSheetNum("Control Point");
		archiveIndex = Table.getSheetNum("Archive Property");
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
			if(!spreadsheet[monitorIndex][i][0].startsWith(Table.DepChar)){
				mp = new MonitorImpl(row, null);
				mparent = mp;
			}
			else{
				mp = new MonitorImpl(row, mparent);
				mparent.addDependent(mp);
			}
			mp.setArchive(getArchive(mp.FullName()));
			mp.setAssemblyName(main.Assembly());
			mp.setMetaEnvironment(this.getMetaEnvironment());
			monitorPoints.add(mp);
		}
		// Get the control points
		ControlImpl cparent = null;
		controlPoints = new ElementSet();
		for (i = 2; i < spreadsheet[controlIndex].length; ++i) {
			if(spreadsheet[controlIndex][i].length == 0)
				break;
			ControlImpl cp;
			String[] row = spreadsheet[controlIndex][i];
			if(!spreadsheet[controlIndex][i][0].startsWith(Table.DepChar))
			{
				cp = new ControlImpl(row, null);
				cparent = cp;
			}
			else{
				cp = new ControlImpl(row, cparent);
				cparent.addDependent(cp);
			}
			cp.setArchive(getArchive(cp.FullName()));
			cp.setAssemblyName(main.Assembly());
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
			ap.setMetaEnvironment(this.getMetaEnvironment());
			archiveProperties.add(ap);
		}
		System.out.println("DeviceModel: Initialization complete.");
		return "";
	}
	public String VendorClass() {
		return ((MainImpl)main).VendorClass();
	}
}
