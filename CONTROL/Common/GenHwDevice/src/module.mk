GenHwDeviceLibFiles = \
	config/schemas/ArchiveBase.xsd \
	config/schemas/ControlBase.xsd \
	config/schemas/ExcelNamespace.xsd \
	config/schemas/MainBase.xsd \
	config/schemas/MandCBase.xsd \
	config/schemas/MonitorBase.xsd \
	config/schemas/OfficeNamespace.xsd \
	config/schemas/Util.xsd \
	config/schemas/amb/ArchiveImpl.xsd \
	config/schemas/amb/ControlImpl.xsd \
	config/schemas/amb/MainImpl.xsd \
	config/schemas/amb/MandCImpl.xsd \
	config/schemas/amb/MonitorImpl.xsd \
	config/schemas/amb/Workbook.xsd \
	config/schemas/eth/ArchiveImpl.xsd \
	config/schemas/eth/ControlImpl.xsd \
	config/schemas/eth/MainImpl.xsd \
	config/schemas/eth/MandCImpl.xsd \
	config/schemas/eth/MonitorImpl.xsd \
	config/schemas/eth/Workbook.xsd \
	config/templates/Root.xpt \
	config/templates/XMLAddDevice.xpt \
	config/templates/amb/ALT.xpt \
	config/templates/amb/BaseDotCPP.xpt \
	config/templates/amb/BaseDotH.xpt \
	config/templates/amb/BaseIDL.xpt \
	config/templates/amb/CCLBaseDotPY.xpt \
	config/templates/amb/CDBComponent.xpt \
	config/templates/amb/CDBInstance.xpt \
	config/templates/amb/CDBSchema.xpt \
	config/templates/amb/CVSIgnore.xpt \
	config/templates/amb/ControlPoints.xpt \
	config/templates/amb/DeviceDefDotPY.xpt \
	config/templates/amb/GUIControlPointPMDotJava.xpt \
	config/templates/amb/GUIIcdControlPointsDotJava.xpt \
	config/templates/amb/GUIIcdMonitorPointsDotJava.xpt \
	config/templates/amb/GUIMonitorPointPMDotJava.xpt \
	config/templates/amb/GUIPanelDotJava.xpt \
	config/templates/amb/HTML.xpt \
	config/templates/amb/HWSim/CompSimBaseDotCPP.xpt \
	config/templates/amb/HWSim/CompSimBaseDotH.xpt \
	config/templates/amb/HWSim/CompSimBaseIDL.xpt \
	config/templates/amb/HWSim/CompSimImplDotCPP.xpt \
	config/templates/amb/HWSim/CompSimImplDotH.xpt \
	config/templates/amb/HWSim/HWSimBaseDotCPP.xpt \
	config/templates/amb/HWSim/HWSimBaseDotH.xpt \
	config/templates/amb/HWSim/HWSimImplDotCPP.xpt \
	config/templates/amb/HWSim/HWSimImplDotH.xpt \
	config/templates/amb/HWSim/TestAmbDeviceInt.xpt \
	config/templates/amb/HWSim/TestHWSimImpl.xpt \
	config/templates/amb/MonitorPoints.xpt \
	config/templates/amb/SQLAddDevice.xpt \
	config/templates/amb/SQLFillDevice.xpt \
	config/templates/amb/SimBaseDotCPP.xpt \
	config/templates/amb/SimBaseDotH.xpt \
	config/templates/amb/XMLAddDevice.xpt \
	config/templates/eth/BaseDotCPP.xpt \
	config/templates/eth/BaseDotH.xpt \
	config/templates/eth/BaseIDL.xpt \
	config/templates/eth/CCLBaseDotPY.xpt \
	config/templates/eth/CDBComponent.xpt \
	config/templates/eth/CDBInstance.xpt \
	config/templates/eth/CDBSchema.xpt \
	config/templates/eth/CVSIgnore.xpt \
	config/templates/eth/SQLAddDevice.xpt \
	config/templates/eth/Sim/CompSimDotCPP.xpt \
	config/templates/eth/Sim/CompSimDotH.xpt \
	config/templates/eth/Sim/CompSimIDL.xpt \
	config/templates/eth/Sim/EthSimBaseDotCPP.xpt \
	config/templates/eth/Sim/EthSimBaseDotH.xpt \
	config/templates/eth/XMLAddDevice.xpt \
	config/workflow/log4j.properties \
	config/workflow/workflow.oaw \
	config/workflow/workflow.properties \
	idl/ControlPointArchival.xml \
	idl/GENERIC_spreadsheet.xml \
	lib/antlr.jar \
	lib/asdmIDLTypes.jar \
	lib/asdmRuntime.jar \
	lib/commons-cli-1.0.jar \
	lib/commons-lang-2.1.jar \
	lib/commons-logging.jar \
	lib/log4j-1.2.8.jar \
	lib/maciSchemaBindings.jar \
	lib/oaw411.jar \
	lib/oaw411beta.jar \
	lib/ojdbc14.jar

DEBUG = on

#ACSLOGTSDEF = ControlPointArchival

EXECUTABLES = getSimSerialNumber
getSimSerialNumber_OBJECTS = SimulatedSerialNumber getSimSerialNumber

LIBRARIES = MonitoringUtils SimulatedSerialNumber
MonitoringUtils_OBJECTS = Utils
MonitoringUtils_LIBS = ControlExceptions
SimulatedSerialNumber_OBJECTS = SimulatedSerialNumber

INCLUDES = \
	CANError.h \
	CANTypes.h \
	SimulatedSerialNumber.h \
	TypeConversion.h \
	Utils.h \
	Utils.i

JARFILES_L = ControlGenHwDevice
ControlGenHwDevice_DIRS = alma

INSTALL_FILES = \
	$(addprefix ../lib/ControlGenHwDevice/, $(GenHwDeviceLibFiles)) \
	../include/HardwareDevice.mk \
	../lib/ControlGenHwDevice/lib/ControlGenHwDevice.jar

# Standard rules
$(MODRULE)all: $(MODPATH) $(MODDEP)
	$(AT)echo " . . . $@ done"

$(MODRULE)clean: $(MODPATH) clean_$(MODDEP)
	$(AT)echo " . . . $@ done"

$(MODRULE)clean_dist: $(MODPATH) clean_dist_$(MODDEP)
	$(AT)echo " . . . $@ done"

$(MODRULE)install: $(MODPATH) install_$(MODDEP)
	$(AT)chmod -r u+w,a-x,a+rX $(INSTDIR)/lib/ControlGenHwDevice $(INSTDIR)/include/HardwareDevice.mk
	$(AT)echo " . . . $@ done"

$(MODPATH)/lib/ControlGenHwDevice: | $(MODPATH)/lib
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(MODPATH)/lib/ControlGenHwDevice/config: | $(MODPATH)/lib/ControlGenHwDevice
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(MODPATH)/lib/ControlGenHwDevice/config/schemas: | $(MODPATH)/lib/ControlGenHwDevice/config
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(MODPATH)/lib/ControlGenHwDevice/config/schemas/amb: | $(MODPATH)/lib/ControlGenHwDevice/config/schemas
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(MODPATH)/lib/ControlGenHwDevice/config/schemas/eth: | $(MODPATH)/lib/ControlGenHwDevice/config/schemas
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(MODPATH)/lib/ControlGenHwDevice/config/templates: | $(MODPATH)/lib/ControlGenHwDevice/config
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(MODPATH)/lib/ControlGenHwDevice/config/templates/amb: | $(MODPATH)/lib/ControlGenHwDevice/config/templates
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(MODPATH)/lib/ControlGenHwDevice/config/templates/amb/HWSim: | $(MODPATH)/lib/ControlGenHwDevice/config/templates/amb
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(MODPATH)/lib/ControlGenHwDevice/config/templates/eth: | $(MODPATH)/lib/ControlGenHwDevice/config/templates
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(MODPATH)/lib/ControlGenHwDevice/config/templates/eth/Sim: | $(MODPATH)/lib/ControlGenHwDevice/config/templates/eth
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(MODPATH)/lib/ControlGenHwDevice/config/workflow: | $(MODPATH)/lib/ControlGenHwDevice/config
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(MODPATH)/lib/ControlGenHwDevice/idl: | $(MODPATH)/lib/ControlGenHwDevice
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(MODPATH)/lib/ControlGenHwDevice/lib: | $(MODPATH)/lib/ControlGenHwDevice
	$(AT)$(if $(wildcard $<),,mkdir $@)

define DUMB_COPY
$(MODPATH)/lib/ControlGenHwDevice/$1: $(MODPATH)/$1 | $(abspath $(MODPATH)/lib/ControlGenHwDevice/$(dir $1))
	$(AT)cp $$< $$@

ControlGenHwDeviceFiles: $(INSTDIR)/lib/ControlGenHwDevice/$1
endef
$(foreach file, $(GenHwDeviceLibFiles), $(eval $(call DUMB_COPY,$(file))))

$(MODPATH)/lib/ControlGenHwDevice/lib/ControlGenHwDevice.jar: $(MODPATH)/lib/ControlGenHwDevice.jar | $(MODPATH)/lib/ControlGenHwDevice/lib
	$(AT)jar ufe $< alma.buildtime.gen.LinuxGen
	$(AT)cp $< $@

ControlGenHwDeviceFiles: $(INSTDIR)/lib/ControlGenHwDevice/lib/ControlGenHwDevice.jar

$(INSTDIR)/lib/ControlGenHwDevice: | $(INSTDIR)/lib
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(INSTDIR)/lib/ControlGenHwDevice/config: | $(INSTDIR)/lib/ControlGenHwDevice
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(INSTDIR)/lib/ControlGenHwDevice/config/schemas: | $(INSTDIR)/lib/ControlGenHwDevice/config
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(INSTDIR)/lib/ControlGenHwDevice/config/schemas/amb: | $(INSTDIR)/lib/ControlGenHwDevice/config/schemas
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(INSTDIR)/lib/ControlGenHwDevice/config/schemas/eth: | $(INSTDIR)/lib/ControlGenHwDevice/config/schemas
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(INSTDIR)/lib/ControlGenHwDevice/config/templates: | $(INSTDIR)/lib/ControlGenHwDevice/config
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(INSTDIR)/lib/ControlGenHwDevice/config/templates/amb: | $(INSTDIR)/lib/ControlGenHwDevice/config/templates
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(INSTDIR)/lib/ControlGenHwDevice/config/templates/amb/HWSim: | $(INSTDIR)/lib/ControlGenHwDevice/config/templates/amb
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(INSTDIR)/lib/ControlGenHwDevice/config/templates/eth: | $(INSTDIR)/lib/ControlGenHwDevice/config/templates
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(INSTDIR)/lib/ControlGenHwDevice/config/templates/eth/Sim: | $(INSTDIR)/lib/ControlGenHwDevice/config/templates/eth
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(INSTDIR)/lib/ControlGenHwDevice/config/workflow: | $(INSTDIR)/lib/ControlGenHwDevice/config
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(INSTDIR)/lib/ControlGenHwDevice/idl: | $(INSTDIR)/lib/ControlGenHwDevice
	$(AT)$(if $(wildcard $<),,mkdir $@)

$(INSTDIR)/lib/ControlGenHwDevice/lib: | $(INSTDIR)/lib/ControlGenHwDevice
	$(AT)$(if $(wildcard $<),,mkdir $@)
