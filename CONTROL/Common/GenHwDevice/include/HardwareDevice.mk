#******************************************************************************
# ALMA - Atacama Large Millimiter Array
# (c) Associated Universities Inc., 2019
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
#******************************************************************************

$(MODULE_NAME)_MODULE_NAME := $(MODULE_NAME)
$(MODULE_NAME)_DEVICE_TYPE := $(DEVICE_TYPE)

# Error System Definition (optional)
ifneq ($(wildcard $(MODPATH)/idl/$(MODULE_NAME)Exceptions.xml), )
ACSERRDEF += $(MODULE_NAME)Exceptions
endif

# IDL Files and flags
IDL_FILES += $(MODULE_NAME)Base
$(MODULE_NAME)Base_IDLprereq += $(call findDep,HardwareDevice.idl,idl,idl,0)
$(MODULE_NAME)Base_idl_mods += Control
ifneq ($(wildcard $(MODPATH)/idl/$(MODULE_NAME).idl), )
IDL_FILES += $(MODULE_NAME)
$(MODULE_NAME)_idl_mods += Control
$(MODULE_NAME)_JARS += $(MODULE_NAME)Base
endif
ifneq ($(DEVICE_TYPE),ETH)
IDL_FILES += $(MODULE_NAME)CompSimBase
$(MODULE_NAME)CompSimBase_idl_mods += Control
$(MODULE_NAME)CompSimBase_JARS += $(MODULE_NAME)
endif

$(MODULE_NAME)BaseStubs_LIBS += \
	ControlCommonExceptions \
	ControlExceptions \
	HardwareDeviceStubs
ifeq ($(DEVICE_TYPE),ETH)
$(MODULE_NAME)BaseStubs_LIBS += EthernetDeviceExceptions
endif
$(MODULE_NAME)BaseStubs_LIBS += baciStubs
ifneq ($(ACSERRDEF),)
$(MODULE_NAME)BaseStubs_LIBS += $(ACSERRDEF)
endif

ifneq ($(wildcard $(MODPATH)/idl/$(MODULE_NAME).idl), )
$(MODULE_NAME)Stubs_LIBS += $(MODULE_NAME)BaseStubs
endif

ifneq ($(wildcard $(MODPATH)/idl/$(MODULE_NAME)Exceptions.xml), )
$(MODULE_NAME)Stubs_LIBS += $(MODULE_NAME)Exceptions
endif

ifneq ($(DEVICE_TYPE),ETH)
$(MODULE_NAME)CompSimBaseStubs_LIBS += $(MODULE_NAME)Stubs
endif

# Libraries (public and local)
LIBRARIES += $(MODULE_NAME)Impl
ifneq ($(DEVICE_TYPE),ETH)
LIBRARIES += $(MODULE_NAME)CompSimImpl
else
LIBRARIES += $(MODULE_NAME)SimImpl $(VENDOR_CLASS) $(VENDOR_CLASS)Sim
endif

# Implementation classes
$(MODULE_NAME)Impl_OBJECTS += $(MODULE_NAME)Impl $(MODULE_NAME)Base
$(MODULE_NAME)Impl_LIBS += \
	$(MODULE_NAME)BaseStubs \
	$(MODULE_NAME)Stubs \
	ControlDeviceExceptions \
	RepeatGuard \
	TETimeUtil \
	acsThread \
	logging \
	m \
	monitorHelper \
	pthread
ifneq ($(ACSERRDEF),)
$(MODULE_NAME)Impl_LIBS += $(MODULE_NAME)Exceptions
endif
ifneq ($(DEVICE_TYPE),ETH)
$(MODULE_NAME)Impl_LIBS += ambDeviceImpl
else
$(MODULE_NAME)Impl_LIBS += $(VENDOR_CLASS) EthernetDevice
endif

ifneq ($(DEVICE_TYPE),ETH)
$(MODULE_NAME)CompSimImpl_OBJECTS += \
	$(MODULE_NAME)CompSimBase \
	$(MODULE_NAME)CompSimImpl \
	$(MODULE_NAME)HWSimBase \
	$(MODULE_NAME)HWSimImpl
$(MODULE_NAME)CompSimImpl_LIBS += \
	$(MODULE_NAME)CompSimBaseStubs \
	$(MODULE_NAME)Impl \
	SimulatedSerialNumber \
	ambsimcommon \
	pthread
else
$(MODULE_NAME)SimImpl_OBJECTS += $(MODULE_NAME)SimImpl
$(MODULE_NAME)SimImpl_LIBS += \
	$(MODULE_NAME)Impl \
	$(VENDOR_CLASS)Sim \
	SimulatedSerialNumber
endif

# Generated classes
ifneq ($(DEVICE_TYPE),ETH)
else
$(VENDOR_CLASS)_OBJECTS += $(VENDOR_CLASS)
$(VENDOR_CLASS)_LIBS += logging pthread
$(VENDOR_CLASS)_LDFLAGS += -lstdc++

$(VENDOR_CLASS)Sim_OBJECTS += $(VENDOR_CLASS)Sim
$(VENDOR_CLASS)Sim_LIBS += $(VENDOR_CLASS) logging pthread
$(VENDOR_CLASS)Sim_LDFLAGS += -lstdc++
endif

# Python stuff
PY_PACKAGES += CCL

# Configuration Database Files
CDB_SCHEMAS += $(MODULE_NAME)Base
#Automatically include $(MODULE_NAME).xsd if it exists
ifneq ($(wildcard $(MODPATH)/config/CDB/schemas/$(MODULE_NAME).xsd), )
CDB_SCHEMAS += $(MODULE_NAME)
endif

# Jarfiles and their directories
$(MODULE_NAME)_JLIBS += $(MODULE_NAME)Base
ifeq ($(CreateGUI),YES)
JARFILES += $(MODULE_NAME)ExecPlugin
$(MODULE_NAME)ExecPlugin_DIRS += alma/Control/device/gui/$(MODULE_NAME)/
$(MODULE_NAME)ExecPlugin_JLIBS += $(MODULE_NAME)
endif

# Java sources in jarfile on
DEBUG = on

# Other files which have to be installed
INSTALL_FILES += ../config/TMCDB$(MODULE_NAME)Add.xml

# Location of Control's code generation framework.
GenHwDir := lib/ControlGenHwDevice
GenHwLocation := $(shell searchFile $(GenHwDir)/lib/ControlGenHwDevice.jar)/$(GenHwDir)
GEN_CLASSPATH := \
$(GenHwLocation)/config/templates:\
$(GenHwLocation)/config/workflow:\
$(GenHwLocation)/lib/ControlGenHwDevice.jar:\
$(GenHwLocation)/lib/antlr.jar:\
$(GenHwLocation)/lib/asdmIDLTypes.jar:\
$(GenHwLocation)/lib/asdmRuntime.jar:\
$(GenHwLocation)/lib/commons-cli-1.0.jar:\
$(GenHwLocation)/lib/commons-lang-2.1.jar:\
$(GenHwLocation)/lib/commons-logging.jar:\
$(GenHwLocation)/lib/log4j-1.2.8.jar:\
$(GenHwLocation)/lib/maciSchemaBindings.jar:\
$(GenHwLocation)/lib/oaw411.jar:\
$(GenHwLocation)/lib/oaw411beta.jar:\
$(GenHwLocation)/lib/ojdbc14.jar:\
$(GenHwLocation)

# List of generated files and directories.
GEN_LIST := \
	$(MODPATH)/include/$(MODULE_NAME)Base.h \
	$(MODPATH)/src/$(MODULE_NAME)Base.cpp \
	$(MODPATH)/idl/$(MODULE_NAME)Base.idl \
	$(MODPATH)/config/CDB/schemas/$(MODULE_NAME)Base.xsd \
	$(MODPATH)/config/TMCDB$(MODULE_NAME)Add.xml \
	$(MODPATH)/doc/$(MODULE_NAME).html \
	$(MODPATH)/doc/$(MODULE_NAME).xml \
	$(MODPATH)/doc/$(MODULE_NAME)Component.xml \
	$(MODPATH)/doc/$(MODULE_NAME)ControlPoints.html \
	$(MODPATH)/doc/$(MODULE_NAME)MonitorPoints.html \
	$(MODPATH)/idl/$(MODULE_NAME)Add.sql \
	$(MODPATH)/src/CCL/$(MODULE_NAME)Base.py \
	$(MODPATH)/src/CCL/__init__.py
ifneq ($(DEVICE_TYPE),ETH)
GEN_LIST += $(MODPATH)/idl/$(MODULE_NAME)CompSimBase.idl
GEN_LIST += $(MODPATH)/include/$(MODULE_NAME)CompSimBase.h
ifeq ($(DeleteCompSimImpl),YES)
GEN_LIST += $(MODPATH)/include/$(MODULE_NAME)CompSimImpl.h
endif
GEN_LIST += $(MODPATH)/include/$(MODULE_NAME)HWSimBase.h
ifeq ($(DeleteHwSimImpl),YES)
GEN_LIST += $(MODPATH)/include/$(MODULE_NAME)HWSimImpl.h
endif
endif
ifneq ($(DEVICE_TYPE),ETH)
GEN_LIST += $(MODPATH)/src/$(MODULE_NAME)CompSimBase.cpp
ifeq ($(DeleteCompSimImpl),YES)
GEN_LIST += $(MODPATH)/src/$(MODULE_NAME)CompSimImpl.cpp
endif
GEN_LIST += $(MODPATH)/src/$(MODULE_NAME)HWSimBase.cpp
ifeq ($(DeleteHwSimImpl),YES)
GEN_LIST += $(MODPATH)/src/$(MODULE_NAME)HWSimImpl.cpp
endif
endif
ifeq ($(DEVICE_TYPE),AMB)
GEN_LIST += \
	$(MODPATH)/src/$(MODULE_NAME)Def.py \
	$(MODPATH)/src/alma/Control/device/gui/$(MODULE_NAME)/IcdControlPoints.java \
	$(MODPATH)/src/alma/Control/device/gui/$(MODULE_NAME)/IcdMonitorPoints.java \
	$(MODPATH)/src/alma/Control/device/gui/$(MODULE_NAME)/presentationModels/ControlPointPM.java \
	$(MODPATH)/src/alma/Control/device/gui/$(MODULE_NAME)/presentationModels/MonitorPointPM.java
endif
ifneq ($(DEVICE_TYPE),ETH)
ifneq ($(DeleteTestHwSimImpl),NO)
GEN_LIST += $(MODPATH)/test/Test$(MODULE_NAME)HWSimImpl.cpp
endif
ifneq ($(DeleteTestAmbDeviceInt),NO)
GEN_LIST += $(MODPATH)/test/Test$(MODULE_NAME)AmbDeviceInt.cpp
endif
endif

# Most devices don't require this, except for the FrontEnd (ColdCarts)
ifeq ($(SPREADSHEET_ICD), )
SPREADSHEET_ICD := $(MODPATH)/idl/$(MODULE_NAME)_spreadsheet.xml
endif

# Needed to ensure generation completes before compilation starts
ifneq ($(wildcard $(MODPATH)/idl/$(MODULE_NAME).idl), )
$(MODPATH)/object/$(MODULE_NAME).id: $(MODPATH)/idl/$(MODULE_NAME)Base.idl
endif

$(MODPATH)/src/$(MODULE_NAME)Impl.cpp: $(MODPATH)/include/$(MODULE_NAME)Impl.h
$(MODPATH)/include/$(MODULE_NAME)Impl.h: $(MODPATH)/object/$(MODULE_NAME)BaseS.h $(MODPATH)/include/$(MODULE_NAME)Base.h

ifeq ($(DEVICE_TYPE),ETH)
$(MODPATH)/src/$(MODULE_NAME)SimImpl.cpp: $(MODPATH)/include/$(MODULE_NAME)SimImpl.h
$(MODPATH)/include/$(MODULE_NAME)SimImpl.h: $(MODPATH)/include/$(MODULE_NAME)Impl.h
else
$(MODPATH)/src/$(MODULE_NAME)Base.cpp: $(MODPATH)/include/$(MODULE_NAME)Base.h
$(MODPATH)/include/$(MODULE_NAME)Base.h: $(MODPATH)/object/$(MODULE_NAME)BaseS.h
$(MODPATH)/src/$(MODULE_NAME)CompSimBase.cpp: $(MODPATH)/include/$(MODULE_NAME)CompSimBase.h
$(MODPATH)/src/$(MODULE_NAME)CompSimImpl.cpp: $(MODPATH)/include/$(MODULE_NAME)CompSimImpl.h
$(MODPATH)/include/$(MODULE_NAME)CompSimImpl.h: $(MODPATH)/include/$(MODULE_NAME)CompSimBase.h
$(MODPATH)/include/$(MODULE_NAME)CompSimBase.h: $(MODPATH)/include/$(MODULE_NAME)Impl.h $(MODPATH)/object/$(MODULE_NAME)CompSimBaseS.h
$(MODPATH)/src/$(MODULE_NAME)HWSimImpl.cpp: $(MODPATH)/include/$(MODULE_NAME)HWSimBase.h
endif

$(MODPATH)/object/$(MODULE_NAME)C.cpp: $(MODPATH)/object/$(MODULE_NAME)C.h
$(MODPATH)/object/$(MODULE_NAME)C.h: $(MODPATH)/object/$(MODULE_NAME)BaseC.h
$(MODPATH)/object/$(MODULE_NAME)BaseC.h: $(call findDep,HardwareDeviceC.h,include,object,0) $(call findDep,ControlExceptionsC.h,include,object,0)

ControlGenHwDeviceFiles:
	$(AT)

$(GEN_LIST): $(MODPATH)/object/generation.done
$(MODPATH)/object/generation.done: $(SPREADSHEET_ICD) $(GenHwLocation)/lib/ControlGenHwDevice.jar | $(MODPATH)/object
