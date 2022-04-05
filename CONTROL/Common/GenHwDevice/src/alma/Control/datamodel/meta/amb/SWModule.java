/*
 * ALMA - Atacama Large Millimeter Array
 * (c) European Southern Observatory, 2002
 * (c) Associated Universities Inc., 2002
 * Copyright by ESO (in the framework of the ALMA collaboration),
 * Copyright by AUI (in the framework of the ALMA collaboration),
 * All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307  USA
 *
 * File SWModule.java
 */
package alma.Control.datamodel.meta.amb;

import org.openarchitectureware.core.meta.core.Element;
import org.openarchitectureware.core.meta.core.ElementSet;
import org.openarchitectureware.core.meta.core.Model;
import org.openarchitectureware.core.constraint.DesignError;

import alma.Control.datamodel.meta.base.Table;
import alma.Control.datamodel.meta.base.Util;
/**
 * Description
 *
 * @version 1.0 Jul 13, 2005
 * @author Allen Farris
 */
public class SWModule extends org.openarchitectureware.meta.uml.classifier.Class{
	private String AssemblyName = null;
	private String dirPath;
	private String mainAssembly;
	private boolean oneSwModule;
	protected String[] row;
	protected String[] parentRow;
	int sheet;
	private ElementSet monitorPoints;
	private ElementSet controlPoints;
	private ElementSet archiveProperties;
	private ElementSet simMonitorPoints;
	private ElementSet simControlPoints;
	public SWModule(String[] row, boolean oneSwModule, String mainAssembly) {
		this.row = row;
		this.sheet = Table.getSheetNum("Hardware Device");
		this.oneSwModule = oneSwModule;
		int assemblyIndex = Table.getColNum(sheet, "Assembly");
		this.AssemblyName = row[assemblyIndex];
		this.mainAssembly = mainAssembly;
		if(oneSwModule){
			setDirPath(row[assemblyIndex]);
		}
		else{
			setDirPath(mainAssembly + "/" + row[assemblyIndex]);
		}
		monitorPoints = new ElementSet();
		controlPoints = new ElementSet();
		archiveProperties = new ElementSet();
		simMonitorPoints = new ElementSet();
		simControlPoints = new ElementSet();
	}
	public void addMP(Element mp){
		monitorPoints.add(mp);
		if (!(((MonitorImpl)mp).RCA().equals("none") ||
				((MonitorImpl)mp).RCA().equals("0x00000") ||
				((MonitorImpl)mp).RCA().equals("0x30000") ||
				((MonitorImpl)mp).RCA().equals("0x30001") ||
				((MonitorImpl)mp).RCA().equals("0x30002") ||
				((MonitorImpl)mp).RCA().equals("0x30003") ||
				((MonitorImpl)mp).RCA().equals("0x30004") ||
				((MonitorImpl)mp).RCA().equals("0x30005")
		)){
			for(int i=0;i< simMonitorPoints.size();i++)
				if(((MonitorImpl)mp).RCA().equals(((MonitorImpl)simMonitorPoints.get(i)).RCA()))
					return;
			simMonitorPoints.add(mp);
		}
	}
	public void addCP(Element cp){
		controlPoints.add(cp);
		if (!(((ControlImpl)cp).RCA().equals("none") ||
				((ControlImpl)cp).RCA().equals("0x31000") ||
				((ControlImpl)cp).RCA().equals("0x31001")
		)){
			for(int i=0;i< simControlPoints.size();i++)
				if(((ControlImpl)cp).RCA().equals(((ControlImpl)simControlPoints.get(i)).RCA()))
					return;
			simControlPoints.add(cp);
		}
	}
	public void addAP(Element ap){
	    archiveProperties.add(ap);
	}
	public void setAssemblyName(String s) {
		this.AssemblyName = s;
	}
	public void setDirPath(String path) {
		dirPath = path;
	}
	public String Initialize() {
//		String[][][] sheet = model.getSpreadsheet();
//		int i, j;
//		monitorPoints = new ElementSet();
//		for (i=2; i < sheet[SP.mont].length; ++i) {
//			if (sheet[SP.mont][i][SP.montAssembly].equals(AssemblyName)) {
//				MonitorPoint mp = new MonitorPoint(model);
//				mp.setMetaEnvironment(this.getMetaEnvironment());
//				mp.setRowIndex(i,monitorPoints,controlPoints);
//				monitorPoints.add(mp);
//			}
//		}
//		controlPoints = new ElementSet();
//		for (i = 2; i < sheet[SP.cont].length; ++i) {
//			if (sheet[SP.cont][i][SP.contAssembly].equals(AssemblyName)) {
//				ControlPoint cp = new ControlPoint(model);
//				cp.setMetaEnvironment(this.getMetaEnvironment());
//				cp.setRowIndex(i,monitorPoints,controlPoints);
//				controlPoints.add(cp);
//			}
//		}
//		archiveProperties = new ElementSet();
//		for (i = 2; i < sheet[SP.arch].length; ++i) {
//			if (sheet[SP.arch][i][SP.archAssembly].equals(AssemblyName)) {
//				ArchiveProperty ap = new ArchiveProperty(model);
//				ap.setMetaEnvironment(this.getMetaEnvironment());
//				ap.setRowIndex(i,monitorPoints,controlPoints);
//				archiveProperties.add(ap);
//			}
//		}
//		simMonitorPoints = new ElementSet();
//		for (i=2; i < sheet[SP.mont].length; ++i) {
//			if (sheet[SP.mont][i][SP.montAssembly].equals(AssemblyName)) {
//				//Exlude generic monitor points and without RCA
//				if (sheet[SP.mont][i][SP.montRCA].equals("none") ||
//					sheet[SP.mont][i][SP.montRCA].equals("0x00000") ||
//					sheet[SP.mont][i][SP.montRCA].equals("0x30000") ||
//					sheet[SP.mont][i][SP.montRCA].equals("0x30001") ||
//					sheet[SP.mont][i][SP.montRCA].equals("0x30002") ||
//					sheet[SP.mont][i][SP.montRCA].equals("0x30003") ||
//					sheet[SP.mont][i][SP.montRCA].equals("0x30004") ||
//					sheet[SP.mont][i][SP.montRCA].equals("0x30005")
//				)
//					continue;
//				//Search for repeated monitor points in the sheet
//				boolean repeated = false;
//				for (j=2; j < i; ++j) {
//					if (sheet[SP.mont][i][SP.montRCA].equals(sheet[SP.mont][j][SP.montRCA])) {
//						repeated = true;
//						break;
//					}
//				}
//				if (repeated)
//					continue;
//				//If it is not repeated, add it
//				MonitorPoint mp = new MonitorPoint(model);
//				mp.setMetaEnvironment(this.getMetaEnvironment());
//				mp.setRowIndex(i,monitorPoints,controlPoints);
//				simMonitorPoints.add(mp);
//			}
//		}
//		simControlPoints = new ElementSet();
//		for (i = 2; i < sheet[SP.cont].length; ++i) {
//			if (sheet[SP.cont][i][SP.contAssembly].equals(AssemblyName)) {
//				//Exlude generic control points and without RCA
//				if (sheet[SP.cont][i][SP.contRCA].equals("none") ||
//					sheet[SP.cont][i][SP.contRCA].equals("0x31000") ||
//					sheet[SP.cont][i][SP.contRCA].equals("0x31001")
//				)
//					continue;
//				//Search for repeated monitor points in the sheet
//				boolean repeated = false;
//				for (j=2; j < i; ++j) {
//					if (sheet[SP.cont][i][SP.contRCA].equals(sheet[SP.cont][j][SP.contRCA])) {
//						repeated = true;
//						break;
//					}
//				}
//				if (repeated)
//					continue;
//				//If it is not repeated, add it
//				ControlPoint cp = new ControlPoint(model);
//				cp.setMetaEnvironment(this.getMetaEnvironment());
//				cp.setRowIndex(i,monitorPoints,controlPoints);
//				simControlPoints.add(cp);
//			}
//		}
		return "";
	}
	
	public String DirPath() {
		return dirPath;
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
	public ElementSet SimMonitorPoint() {
		return simMonitorPoints;
	}
	public ElementSet SimControlPoint() {
		return simControlPoints;
	}
	public String Assembly() {
		return row[Table.getColNum(sheet, "Assembly")];
	}
	public String Description() {
		return row[Table.getColNum(sheet, "Description")];
	}
	public String DescriptionL1() {
		return Util.formatDescriptionL1(Description());
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
	public String Parent() {
		return row[Table.getColNum(sheet, "Parent")];
	}
	public String Cardinality() {
		return row[Table.getColNum(sheet, "Cardinality")];
	}
	public boolean GenericMonitorPoints() {
		return row[Table.getColNum(sheet, "Generic Monitor Points")].equals("yes");
	}
	public String NodeAddress() {
		return row[Table.getColNum(sheet, "Node Address")];
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
	public String NodeAddressInDecimal() {
		String s = NodeAddress();
		if (s.equals("parm"))
		    return "0";
		//if (!s.startsWith("0x"))
		//    throw new DesignError("Invalid number (" + s + ")");
		return HexToDec(s);
	}
	public String Channel() {
		return row[Table.getColNum(sheet, "Channel")];
	}
	public String BaseAddress() {
		return row[Table.getColNum(sheet, "Base Address")];
	}
	public String SimSerialNumber() {
		String hex = Integer.toHexString(Assembly().hashCode()).toLowerCase();
		hex = "0000000000000000" + hex;
		int len = hex.length();
		hex = "0x" + hex.substring(len - 16, len);
		return hex;
	}
}
