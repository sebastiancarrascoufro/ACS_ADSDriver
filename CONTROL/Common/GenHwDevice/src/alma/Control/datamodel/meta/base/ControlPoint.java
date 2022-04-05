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
 * File ControlPoint.java
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
public class ControlPoint extends MandCBase {

	public ControlPoint(String[] row, Element parent) {
		super(row, parent);
	}
	public String CPName() {
		return PName();
	}
	public String AltCPName() {
		return AltPName();
	}
	public boolean ArchiveOnUse(){
		return row[Table.getColNum(sheet, "Archive On Use")].equals("yes");
	}
	@Override
	public boolean isMonitored() {
		return false;
	}
}
