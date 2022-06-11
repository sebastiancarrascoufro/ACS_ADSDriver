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
 * File HwDeviceWorkflowInitializer.java
 */
package alma.Control.datamodel;

import org.openarchitectureware.core.meta.MetaEnvironment;
import org.openarchitectureware.workflow.WfCHelper;
import org.openarchitectureware.workflow.WorkflowContext;
import org.openarchitectureware.workflow.issues.Issues;
import org.openarchitectureware.workflow.monitor.ProgressMonitor;
import org.openarchitectureware.workflow.oawclassic.util.WorkflowComponentWithME;

import alma.Control.datamodel.meta.base.Util;

public class HwDeviceWorkflowInitializer extends WorkflowComponentWithME {
	private String deviceDir;
	private String spreadsheetDir;
	private String generatedDir;
	private boolean monitorDBOnly;
	private boolean generateAlt;
	private String modelSlot;
	
	public void checkConfiguration(Issues issues) {
		super.checkConfiguration(issues);
		if (!WfCHelper.isParamSet(modelSlot)) 
			issues.addError(this, "model slot not specified");
	}

	public void invoke(WorkflowContext ctx, ProgressMonitor monitor, Issues issues) {
		System.out.println("HwDeviceWorkflowInitializer.invoke started.");
		MetaEnvironment me = getMetaEnv(ctx,issues);
		alma.Control.datamodel.meta.base.DeviceModel t = null;
		if(System.getProperty("DEVICE_TYPE").equals("AMB")){
			t = new alma.Control.datamodel.meta.amb.DeviceModel();
		}
		else if(System.getProperty("DEVICE_TYPE").equals("ETH")){
			t = new alma.Control.datamodel.meta.eth.DeviceModel();
		}
		else {
			System.out.println("The device type "+ System.getProperty("DEVICE_TYPE") +" is not supported.");
			return;
		}

		t.setMetaEnvironment(me);
		me.addElement(t);
		t.setName("DeviceModel");
		t.setCodegenDir(Util.getInstallDir());
		t.setDeviceDir(deviceDir);
		t.setDeviceName(System.getProperty("DEVICE_NAME"));
		t.setGeneratedDir(generatedDir);
		t.setSpreadsheetDir(spreadsheetDir);
		t.setMonitorDBOnly(monitorDBOnly);
		t.setGenerateAlt(generateAlt);
		t.setBusType(System.getProperty("DEVICE_TYPE"));
		t.CreateModel(); 
		ctx.set(modelSlot,t);
		System.out.println("HwDeviceWorkflowInitializer completed.");
	}

	public void setDeviceDir(String deviceDir) {
		System.out.println("HwDeviceWorkflowInitializer.setDeviceDir setDeviceDir = " + deviceDir);
		this.deviceDir = deviceDir;
	}
	public void setGeneratedDir(String generatedDir) {
		System.out.println("HwDeviceWorkflowInitializer.setDeviceDir setGeneratedDir = " + generatedDir);
		this.generatedDir = generatedDir;
	}
	public void setSpreadsheetDir(String spreadsheetDir) {
		System.out.println("HwDeviceWorkflowInitializer.setDeviceDir setSpreadsheetDir = " + spreadsheetDir);
		this.spreadsheetDir = spreadsheetDir;
	}	
	public void setModelSlot(String s) {
		System.out.println("HwDeviceWorkflowInitializer.setModelSlot = " + s);
		this.modelSlot = s;
	}
	public void setMonitorDBOnly(String s) {
		System.out.println("HwDeviceWorkflowInitializer.setMonitorDBOnly = " + s);
		this.monitorDBOnly = s.equals("true") ? true : false;
	}
	public void setGenerateAlt(String s) {
		System.out.println("HwDeviceWorkflowInitializer.setGenerateAlt = " + s);
		this.generateAlt = s.equals("true") ? true : false;;
	}
}
