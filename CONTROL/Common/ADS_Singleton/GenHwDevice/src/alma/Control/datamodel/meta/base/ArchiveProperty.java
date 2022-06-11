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
 * File ArchiveProperty.java
 */
package alma.Control.datamodel.meta.base;

import org.openarchitectureware.core.meta.core.ElementSet;
import org.openarchitectureware.core.meta.core.Element;
import org.openarchitectureware.core.meta.core.Model;
import org.openarchitectureware.core.constraint.DesignError;

/**
 * Description
 *
 * @version 1.0 Jul 13, 2005
 * @author Allen Farris
 */
public class ArchiveProperty extends org.openarchitectureware.meta.uml.classifier.Class {
	protected String[] row;
	protected int sheet;
	protected Element mp = null;
	protected Element cp = null;
	public ArchiveProperty(){
	}
	public ArchiveProperty(String[] row) {
		this.row = row;
		this.sheet = Table.getSheetNum("Archive Property");
	}
	public void setMP(Element mp){
		this.mp = mp;
	}
	public void setCP(Element cp){
		this.cp = cp;
	}
	public boolean isRefersTo() {
		return mp != null || cp != null;
	}
	public boolean isMonitorPoint() {
		return mp != null;
	}
	public Element getMonitorPoint() {
		return mp;
	}
	public boolean isControlPoint() {
		return cp != null;
	}
	public Element getControlPoint() {
		return cp;
	}
	public String Name(){
		return row[Table.getColNum(sheet, "Name")];
	}
	public String APName(){
	    String name = Name();
	    if (name.startsWith(Table.DepChar))
	        name = name.substring(1);
		return name;
	}
	public String RefersTo(){
		return row[Table.getColNum(sheet, "Refers To")];
	}
	public String IntervalFull(){
		return row[Table.getColNum(sheet, "Interval (secs)")];
	}
	public String Interval(){
		return row[Table.getColNum(sheet, "Interval (secs)")];
	}
	public String OldOnlyOnChange(){
		return row[Table.getColNum(sheet, "Only On Change")];
	}
	public String DisplayUnits(){
		return row[Table.getColNum(sheet, "Display Units")];
	}
	public String GraphMin(){
		return row[Table.getColNum(sheet, "Graph Min")];
	}
	public String GraphMax(){
		return row[Table.getColNum(sheet, "Graph Max")];
	}
	public String Format(){
		return row[Table.getColNum(sheet, "Format")];
	}
	public boolean OnStartup(){
		return row[Table.getColNum(sheet, "On Startup")].equals("yes");
	}
	public String Title(){
		return row[Table.getColNum(sheet, "Title")];
	}
}
