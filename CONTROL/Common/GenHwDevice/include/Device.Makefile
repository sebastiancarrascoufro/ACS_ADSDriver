#******************************************************************************
# ALMA - Atacama Large Millimiter Array
# (c) Associated Universities Inc., 2004 - 2010 
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



#############################################
#############################################
## Do NOT touch anything beyond this line! ##
#############################################
#############################################

# Do not create any static (.a) libraries. They are not needed and just consume disk space.
MAKE_NOSTATIC=true

#
# Error System Definition (optional)
# ----------------------------------
ifneq ($(wildcard ../idl/$(MODULE_NAME)Exceptions.xml), )
ACSERRDEF += $(notdir $(basename $(wildcard ../idl/$(MODULE_NAME)Exceptions.xml)))
endif


# 
# IDL Files and flags
# 
IDL_FILES += $(MODULE_NAME)Base
ifneq ($(wildcard ../idl/$(MODULE_NAME).idl), )
IDL_FILES += $(MODULE_NAME)
$(MODULE_NAME)_IDLprereq += ../idl/$(MODULE_NAME)Base.idl
endif
ifneq ($(DEVICE_TYPE),ETH)
IDL_FILES += $(MODULE_NAME)CompSimBase
IDL_FILES += $(MODULE_NAME)CompAdsBase
endif

$(MODULE_NAME)BaseStubs_LIBS += HardwareDeviceStubs
$(MODULE_NAME)BaseStubs_LIBS += ControlCommonExceptions
$(MODULE_NAME)BaseStubs_LIBS += ControlExceptions
ifeq ($(DEVICE_TYPE),ETH)
$(MODULE_NAME)BaseStubs_LIBS += EthernetDeviceExceptions
endif
$(MODULE_NAME)BaseStubs_LIBS += baciStubs
ifneq ($(ACSERRDEF),)
$(MODULE_NAME)BaseStubs_LIBS += $(ACSERRDEF)
endif

ifneq ($(wildcard ../idl/$(MODULE_NAME).idl), )
$(MODULE_NAME)Stubs_LIBS += $(MODULE_NAME)BaseStubs
endif

ifneq ($(wildcard ../idl/$(MODULE_NAME)Exceptions.xml), )
$(MODULE_NAME)Stubs_LIBS += $(ACSERRDEF)
endif

ifneq ($(DEVICE_TYPE),ETH)
$(MODULE_NAME)CompSimBaseStubs_LIBS += $(MODULE_NAME)Stubs
$(MODULE_NAME)CompAdsBaseStubs_LIBS += $(MODULE_NAME)Stubs
endif


#
# Libraries (public and local)
# ----------------------------
LIBRARIES += $(MODULE_NAME)Impl
ifneq ($(DEVICE_TYPE),ETH)
LIBRARIES += $(MODULE_NAME)CompSimImpl
LIBRARIES += $(MODULE_NAME)CompAdsImpl
else
LIBRARIES += $(MODULE_NAME)SimImpl
LIBRARIES += $(VENDOR_CLASS)
LIBRARIES += $(VENDOR_CLASS)Sim
endif

# Implementation classes
$(MODULE_NAME)Impl_OBJECTS += $(MODULE_NAME)Impl $(MODULE_NAME)Base
$(MODULE_NAME)Impl_LIBS += $(MODULE_NAME)Stubs $(MODULE_NAME)BaseStubs
ifneq ($(ACSERRDEF),)
$(MODULE_NAME)Impl_LIBS += $(ACSERRDEF)
endif
$(MODULE_NAME)Impl_LIBS += ControlDeviceExceptions
$(MODULE_NAME)Impl_LIBS += acsThread
$(MODULE_NAME)Impl_LIBS += TETimeUtil
$(MODULE_NAME)Impl_LIBS += logging
$(MODULE_NAME)Impl_LIBS += RepeatGuard
$(MODULE_NAME)Impl_LIBS += monitorHelper
$(MODULE_NAME)Impl_LIBS += m 
$(MODULE_NAME)Impl_LIBS += pthread
ifneq ($(DEVICE_TYPE),ETH)
$(MODULE_NAME)Impl_LIBS += ambDeviceImpl
else
$(MODULE_NAME)Impl_LIBS += EthernetDevice
$(MODULE_NAME)Impl_LIBS += $(VENDOR_CLASS)
endif

ifneq ($(DEVICE_TYPE),ETH)
$(MODULE_NAME)CompSimImpl_OBJECTS += $(MODULE_NAME)CompSimImpl $(MODULE_NAME)CompSimBase
$(MODULE_NAME)CompSimImpl_OBJECTS += $(MODULE_NAME)HWSimImpl $(MODULE_NAME)HWSimBase
$(MODULE_NAME)CompSimImpl_LIBS += SimulatedSerialNumber
$(MODULE_NAME)CompSimImpl_LIBS += ambsimcommon
$(MODULE_NAME)CompSimImpl_LIBS += $(MODULE_NAME)Impl
$(MODULE_NAME)CompSimImpl_LIBS += $(MODULE_NAME)CompSimBaseStubs
$(MODULE_NAME)CompSimImpl_LIBS += pthread




$(MODULE_NAME)CompAdsImpl_OBJECTS += $(MODULE_NAME)CompAdsImpl $(MODULE_NAME)CompAdsBase
$(MODULE_NAME)CompAdsImpl_LIBS += SimulatedSerialNumber
$(MODULE_NAME)CompAdsImpl_LIBS += adsDevIO 
$(MODULE_NAME)CompAdsImpl_LIBS += $(MODULE_NAME)Impl
$(MODULE_NAME)CompAdsImpl_LIBS += $(MODULE_NAME)CompAdsBaseStubs
$(MODULE_NAME)CompAdsImpl_LIBS += pthread

else
$(MODULE_NAME)SimImpl_OBJECTS += $(MODULE_NAME)SimImpl
$(MODULE_NAME)SimImpl_LIBS += $(MODULE_NAME)Impl $(VENDOR_CLASS)Sim
$(MODULE_NAME)SimImpl_LIBS += SimulatedSerialNumber
endif


# Generated classes
ifneq ($(DEVICE_TYPE),ETH)
else
$(VENDOR_CLASS)_OBJECTS += $(VENDOR_CLASS)
$(VENDOR_CLASS)_LIBS += logging
$(VENDOR_CLASS)_LIBS += pthread
$(VENDOR_CLASS)_LDFLAGS += -lstdc++

$(VENDOR_CLASS)Sim_OBJECTS += $(VENDOR_CLASS)Sim
$(VENDOR_CLASS)Sim_LIBS += $(VENDOR_CLASS)
$(VENDOR_CLASS)Sim_LIBS += logging
$(VENDOR_CLASS)Sim_LIBS += pthread
$(VENDOR_CLASS)Sim_LDFLAGS += -lstdc++
endif


#
# Python stuff
# ----------------------------
PY_PACKAGES += CCL


#
# Configuration Database Files
# ----------------------------
CDB_SCHEMAS += $(MODULE_NAME)Base
#Automatically include $(MODULE_NAME).xsd if it exists
ifneq ($(wildcard ../config/CDB/schemas/$(MODULE_NAME).xsd), )
CDB_SCHEMAS += $(MODULE_NAME)
endif


#
# Jarfiles and their directories
# ------------------------------
$(MODULE_NAME)_JLIBS += $(MODULE_NAME)Base
ifeq ($(CreateGUI),YES)
JARFILES += $(MODULE_NAME)ExecPlugin
$(MODULE_NAME)ExecPlugin_DIRS += alma/Control/device/gui/$(MODULE_NAME)/
$(MODULE_NAME)ExecPlugin_JLIBS += $(MODULE_NAME)
endif


#
# Java sources in jarfile on
# --------------------------
DEBUG = on


# Other files which have to be installed
# --------------------------------------
INSTALL_FILES += ../config/TMCDB$(MODULE_NAME)Add.xml


#
# list of all possible C-sources (used to create automatic dependencies)
# ------------------------------
CSOURCENAMES = \
    $(foreach exe, $(EXECUTABLES) $(EXECUTABLES_L), $($(exe)_OBJECTS)) \
    $(foreach rtos, $(RTAI_MODULES) , $($(rtos)_OBJECTS)) \
    $(foreach lib, $(LIBRARIES) $(LIBRARIES_L), $($(lib)_OBJECTS))

#
#>>>>> END OF standard rules

#
# INCLUDE STANDARDS
# -----------------

MAKEDIRTMP := $(shell searchFile include/acsMakefile)
ifneq ($(MAKEDIRTMP),\#error\#)
   MAKEDIR := $(MAKEDIRTMP)/include
   include $(MAKEDIR)/acsMakefile
endif

# Location of Control's code generation framework.
# ------------------------------------------------
GenHwDir := /lib/ControlGenHwDevice
GenHwLocation := $(shell searchFile $(GenHwDir)/lib/ControlGenHwDevice.jar)
ifeq ($(GenHwLocation), \#error\#)
   GenHwLocation := $(INSTALL_ROOT)/lib
endif
empty:=
space:= $(empty) $(empty)
GEN_CLASSPATH = $(GenHwLocation)$(GenHwDir):
GEN_CLASSPATH += $(GenHwLocation)$(GenHwDir)/lib/ControlGenHwDevice.jar:
GEN_CLASSPATH += $(GenHwLocation)$(GenHwDir)/config/templates:
GEN_CLASSPATH += $(GenHwLocation)$(GenHwDir)/config/workflow:
GEN_CLASSPATH += $(GenHwLocation)$(GenHwDir)/lib/antlr.jar:
GEN_CLASSPATH += $(GenHwLocation)$(GenHwDir)/lib/asdmIDLTypes.jar:
GEN_CLASSPATH += $(GenHwLocation)$(GenHwDir)/lib/asdmRuntime.jar:
GEN_CLASSPATH += $(GenHwLocation)$(GenHwDir)/lib/commons-cli-1.0.jar:
GEN_CLASSPATH += $(GenHwLocation)$(GenHwDir)/lib/commons-lang-2.1.jar:
GEN_CLASSPATH += $(GenHwLocation)$(GenHwDir)/lib/commons-logging.jar:
GEN_CLASSPATH += $(GenHwLocation)$(GenHwDir)/lib/log4j-1.2.8.jar:
GEN_CLASSPATH += $(GenHwLocation)$(GenHwDir)/lib/maciSchemaBindings.jar:
GEN_CLASSPATH += $(GenHwLocation)$(GenHwDir)/lib/oaw411.jar:
GEN_CLASSPATH += $(GenHwLocation)$(GenHwDir)/lib/oaw411beta.jar:
GEN_CLASSPATH += $(GenHwLocation)$(GenHwDir)/lib/ojdbc14.jar


# List of generated files and directories.
#GEN_LIST += ../bin
GEN_LIST += ../doc/$(MODULE_NAME).html
GEN_LIST += ../doc/$(MODULE_NAME).xml
GEN_LIST += ../doc/$(MODULE_NAME)Component.xml
GEN_LIST += ../doc/$(MODULE_NAME)ControlPoints.html
GEN_LIST += ../doc/$(MODULE_NAME)MonitorPoints.html
#GEN_LIST += ../lib/python
#GEN_LIST += ../rtai
GEN_LIST += ../config/TMCDB$(MODULE_NAME)Add.xml
GEN_LIST += ../config/CDB/schemas/$(MODULE_NAME)Base.xsd
GEN_LIST += ../idl/$(MODULE_NAME)Base.idl
GEN_LIST += ../idl/$(MODULE_NAME)CompSimBase.idl
GEN_LIST += ../idl/$(MODULE_NAME)CompAdsBase.idl
GEN_LIST += ../idl/$(MODULE_NAME)Add.sql
GEN_LIST += ../include/$(MODULE_NAME)Base.h
ifneq ($(DEVICE_TYPE),ETH)
GEN_LIST += ../include/$(MODULE_NAME)CompSimBase.h
GEN_LIST += ../include/$(MODULE_NAME)CompAdsBase.h
ifeq ($(DeleteCompSimImpl),YES)
GEN_LIST += ../include/$(MODULE_NAME)CompSimImpl.h
GEN_LIST += ../include/$(MODULE_NAME)CompAdsImpl.h
endif
GEN_LIST += ../include/$(MODULE_NAME)HWSimBase.h 
ifeq ($(DeleteHwSimImpl),YES)
GEN_LIST += ../include/$(MODULE_NAME)HWSimImpl.h
endif
endif
GEN_LIST += $(MODULE_NAME)Base.cpp
ifneq ($(DEVICE_TYPE),ETH)
GEN_LIST += $(MODULE_NAME)CompSimBase.cpp
GEN_LIST += $(MODULE_NAME)CompAdsBase.cpp
ifeq ($(DeleteCompSimImpl),YES)
GEN_LIST += $(MODULE_NAME)CompSimImpl.cpp
GEN_LIST += $(MODULE_NAME)CompAdsImpl.cpp
endif
GEN_LIST += $(MODULE_NAME)HWSimBase.cpp
ifeq ($(DeleteHwSimImpl),YES)
GEN_LIST += $(MODULE_NAME)HWSimImpl.cpp
endif
endif
#GUIS
#USER_GUI_FILES_LIST += $(foreach file,$(USER_GUI_FILES),$(shell find ./alma/ -path .*$(file)*))
#GUI_FILES = $(shell find ./alma/ -type f)
#GUI_LIST = $(filter-out $(USER_GUI_FILES_LIST),$(GUI_FILES))
#GEN_LIST += $(GUI_LIST)

GEN_LIST += alma/Control/device/gui/$(MODULE_NAME)/IcdControlPoints.java
GEN_LIST += alma/Control/device/gui/$(MODULE_NAME)/IcdMonitorPoints.java
GEN_LIST += alma/Control/device/gui/$(MODULE_NAME)/presentationModels/ControlPointPM.java
GEN_LIST += alma/Control/device/gui/$(MODULE_NAME)/presentationModels/MonitorPointPM.java

#python
GEN_LIST += $(MODULE_NAME)Def.py
GEN_LIST += CCL/__init__.py
GEN_LIST += CCL/$(MODULE_NAME)Base.py

ifneq ($(DEVICE_TYPE),ETH)
ifneq ($(DeleteTestHwSimImpl),NO)
GEN_LIST += ../test/Test$(MODULE_NAME)HWSimImpl.cpp
endif
ifneq ($(DeleteTestAmbDeviceInt),NO)
GEN_LIST += ../test/Test$(MODULE_NAME)AmbDeviceInt.cpp
endif
endif

GEN_TEMPLATES += Root.xpt
GEN_TEMPLATES += XMLAddDevice.xpt
GEN_TEMPLATES += amb/BaseDotH.xpt
GEN_TEMPLATES += amb/ALT.xpt
GEN_TEMPLATES += amb/HWSim/CompSimBaseDotCPP.xpt
GEN_TEMPLATES += amb/HWSim/CompSimBaseDotH.xpt
GEN_TEMPLATES += amb/HWSim/CompSimBaseIDL.xpt
GEN_TEMPLATES += amb/HWSim/CompSimImplDotCPP.xpt
GEN_TEMPLATES += amb/HWSim/CompSimImplDotH.xpt
GEN_TEMPLATES += amb/HWSim/HWSimBaseDotCPP.xpt
GEN_TEMPLATES += amb/HWSim/HWSimBaseDotH.xpt
GEN_TEMPLATES += amb/HWSim/HWSimImplDotCPP.xpt
GEN_TEMPLATES += amb/HWSim/HWSimImplDotH.xpt
GEN_TEMPLATES += amb/HWSim/TestAmbDeviceInt.xpt
GEN_TEMPLATES += amb/HWSim/TestHWSimImpl.xpt
GEN_TEMPLATES += amb/ADS/CompAdsBaseDotCPP.xpt
GEN_TEMPLATES += amb/ADS/CompAdsBaseDotH.xpt
GEN_TEMPLATES += amb/ADS/CompAdsBaseIDL.xpt
GEN_TEMPLATES += amb/ADS/CompAdsImplDotCPP.xpt
GEN_TEMPLATES += amb/ADS/CompAdsImplDotH.xpt
GEN_TEMPLATES += amb/BaseDotCPP.xpt
GEN_TEMPLATES += amb/BaseIDL.xpt
GEN_TEMPLATES += amb/CCLBaseDotPY.xpt
GEN_TEMPLATES += amb/CDBComponent.xpt
GEN_TEMPLATES += amb/CDBInstance.xpt
GEN_TEMPLATES += amb/CDBSchema.xpt
GEN_TEMPLATES += amb/CVSIgnore.xpt
GEN_TEMPLATES += amb/ControlPoints.xpt
GEN_TEMPLATES += amb/DeviceDefDotPY.xpt
GEN_TEMPLATES += amb/GUIControlPointPMDotJava.xpt
GEN_TEMPLATES += amb/GUIIcdControlPointsDotJava.xpt
GEN_TEMPLATES += amb/GUIIcdMonitorPointsDotJava.xpt
GEN_TEMPLATES += amb/GUIMonitorPointPMDotJava.xpt
GEN_TEMPLATES += amb/GUIPanelDotJava.xpt
GEN_TEMPLATES += amb/HTML.xpt
GEN_TEMPLATES += amb/InterfaceBaseDotCPP.xpt
GEN_TEMPLATES += amb/InterfaceBaseDotH.xpt
GEN_TEMPLATES += amb/MonitorPoints.xpt
GEN_TEMPLATES += amb/SQLAddDevice.xpt
GEN_TEMPLATES += amb/SQLFillDevice.xpt
GEN_TEMPLATES += amb/SimBaseDotCPP.xpt
GEN_TEMPLATES += amb/SimBaseDotH.xpt
GEN_TEMPLATES += amb/XMLAddDevice.xpt
GEN_TEMPLATES += eth/Sim/CompSimDotCPP.xpt
GEN_TEMPLATES += eth/Sim/CompSimDotH.xpt
GEN_TEMPLATES += eth/Sim/CompSimIDL.xpt
GEN_TEMPLATES += eth/Sim/EthSimBaseDotCPP.xpt
GEN_TEMPLATES += eth/Sim/EthSimBaseDotH.xpt
GEN_TEMPLATES += eth/BaseDotCPP.xpt
GEN_TEMPLATES += eth/BaseDotH.xpt
GEN_TEMPLATES += eth/BaseIDL.xpt
GEN_TEMPLATES += eth/CCLBaseDotPY.xpt
GEN_TEMPLATES += eth/CDBComponent.xpt
GEN_TEMPLATES += eth/CDBInstance.xpt
GEN_TEMPLATES += eth/CDBSchema.xpt
GEN_TEMPLATES += eth/CVSIgnore.xpt
GEN_TEMPLATES += eth/SQLAddDevice.xpt
GEN_TEMPLATES_DEPEND := $(foreach GEN,$(GEN_TEMPLATES),$(GenHwLocation)/$(GenHwDir)/config/templates/$(GEN))

#Devices don't require to set this except for.. guess which, yes the FrontEnd (ColdCarts)
ifeq ($(SPREADSHEET_ICD), )
    SPREADSHEET_ICD:=../idl/$(MODULE_NAME)_spreadsheet.xml
endif
#
# TARGETS
# -------

#This is required by the new Makefile, otherwise the dependency file could be incomplete
#When runung -j > 1
do_all: ../object/generation.done


#The new makefile require that we specifically state this. It should not relly matter
#Due to the dependency do_all: ../object/generation.done, but we try to keep it standard.
.DEFAULT: all
all:  do_all
	@echo " . . . all done"

.PHONY: compile
compile: do_all
	@echo " . . . compile done"

.PHONY: gen
gen: ../object/generation.done 
	java -ea -classpath $(subst $(space),,$(GEN_CLASSPATH)) alma.buildtime.gen.LinuxGen $(MODULE_NAME) $(DEVICE_TYPE)

#The code generator generates all file in one shot. So we need an intermidiate file so the generator runs ONLY ONCE
#independent of the -j parameter. This also guarantee that if the generator fails we will not jump into a file
#generation loop.
$(GEN_LIST):../object/generation.done

../object/generation.done: $(SPREADSHEET_ICD) $(GEN_TEMPLATES_DEPEND $(GenHwLocation)/$(GenHwDir)/lib/ControlGenHwDevice.jar
	@echo " . . . Code generation for $(MODULE_NAME)"
	java -ea -classpath $(subst $(space),,$(GEN_CLASSPATH)) alma.buildtime.gen.LinuxGen $(MODULE_NAME) $(DEVICE_TYPE)
	@echo " . . . Code generation done"
	@mkdir -p ../object
	@touch ../object/generation.done

clean_gen:
	$(RM) $(GEN_LIST)
	$(shell find ./alma/ -type d -empty -exec rmdir --parents --ignore-fail-on-non-empty {} +)
	$(RM) ../object/generation.done

clean: clean_gen clean_all
	@echo " . . . clean done"

clean_dist: clean_gen clean_all clean_dist_all
	@echo " . . . clean_dist done"

man: do_man 
	@echo " . . . man page(s) done"

install: install_all
	@echo " . . . installation done"
