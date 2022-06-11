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
 * File MonitorPoint.java
 */
package alma.Control.datamodel.meta.base;

import java.util.Iterator;

import org.openarchitectureware.core.meta.core.ElementSet;
import org.openarchitectureware.core.meta.core.Element;
import org.openarchitectureware.core.meta.core.Model;
import org.openarchitectureware.core.constraint.DesignError;

import alma.Control.datamodel.meta.amb.MonitorImpl;

/**
 * Description
 *
 * @version 1.0 Jul 13, 2005
 * @author Allen Farris
 */
public abstract class MonitorPoint extends MandCBase {

	public MonitorPoint(String[] row, Element parent) {
		super(row, parent);
	}
	public String Default(){
		return row[Table.getColNum(sheet, "Default")];
	}
	public String ErrorCondition(){
		return row[Table.getColNum(sheet, "Error Condition")];
	}
	public String ErrorSeverity(){
		return row[Table.getColNum(sheet, "Error Severity")];
	}
	public String ErrorAction(){
		return row[Table.getColNum(sheet, "Error Action")];
	}

	// The following items are from the Archive spreadsheet.
	public String APName(){
		//return ((ArchiveProperty)archive).Name();
		return PName();
	}
	public String RefersTo(){
		//return ((ArchiveProperty)archive).RefersTo();
		return PName();
	}
	public String Interval(){
		// Interval is always an integer, and default is 300 sec (5 minutes).
		double defaultValue = 300;
		if(archive != null) {
			if ( ((ArchiveProperty)archive).Interval().equals("none") )
				return Double.toString(defaultValue);
			double value = Double.valueOf(((ArchiveProperty)archive).Interval());
			if (value <= 0)
				value = defaultValue;
			return Double.toString(value);
		}
		else
			return Double.toString(defaultValue);
	}
	public String OldOnlyOnChange(){
		if(archive != null)
			return ((ArchiveProperty)archive).OldOnlyOnChange();
		else
			return "no";
	}
	public String DisplayUnits(){
		if(archive != null)
			return ((ArchiveProperty)archive).DisplayUnits();
		else
			return DataUnits();
	}
	public String GraphMin(){
		if(archive != null)
			return ((ArchiveProperty)archive).GraphMin();
		else
			return "none";
	}
	public String GraphMax(){
		if(archive != null)
			return ((ArchiveProperty)archive).GraphMax();
		else
			return "none";
	}
	public String Format(){
		if(archive != null)
			return ((ArchiveProperty)archive).Format();
		else
			return "none";
	}
	public String Title(){
		if(archive != null)
			return ((ArchiveProperty)archive).Title();
		else
			return Description();
	}

	public String MPName() {
		return PName();
	}
	public String AltMPName() {
		return AltPName();
	}
	
	@Override
	public boolean isMonitored(){
		if(isExternal() && isArchived() && !isPartOfPattern() && !isPartOfHomogeneous())
			return true;
		return false;
	}
	
	abstract public boolean isPartOfPattern();
	abstract public boolean isHomogeneous();
	abstract public boolean isPartOfHomogeneous();

}
