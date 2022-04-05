# ALMA - Atacama Large Millimeter Array
# (c) Associated Universities Inc., 2006 - 2007, 2009 - 2010, 2012 - 2014
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

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# Type-safe log code generation

ACSLOGTSDEF = ControlPointArchival



EXECUTABLES += getSimSerialNumber
# <brief description of getSimSerialNumber program>
getSimSerialNumber_OBJECTS += getSimSerialNumber
getSimSerialNumber_LIBS += SimulatedSerialNumber
getSimSerialNumber_LIBS += C++


LIBRARIES =  SimulatedSerialNumber MonitoringUtils
SimulatedSerialNumber_OBJECTS += SimulatedSerialNumber
SimulatedSerialNumber_LDFLAGS += -lstdc++
MonitoringUtils_OBJECTS += Utils
MonitoringUtils_LIBS += ControlExceptions

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES += CANError.h
INCLUDES += CANTypes.h
INCLUDES += TypeConversion.h
INCLUDES += Utils.h
INCLUDES += Utils.i
INCLUDES += SimulatedSerialNumber.h 


# Jarfiles and their directories
#
JARFILES += ControlGenHwDevice
ControlGenHwDevice_DIRS += alma


#
# java sources in Jarfile on/off
DEBUG = on


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

#
# TARGETS
# -------
all:	do_all
	jar ufe ../lib/ControlGenHwDevice.jar alma.buildtime.gen.LinuxGen
	@echo " . . . 'all' done" 

clean : clean_all 
	@echo " . . . clean done"

clean_dist : clean_all clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	test -e $(INSTALL_ROOT)/lib/ControlGenHwDevice || mkdir $(INSTALL_ROOT)/lib/ControlGenHwDevice
	cp --recursive ../lib ../config ../idl \
		$(INSTALL_ROOT)/lib/ControlGenHwDevice
	cp ../include/Device.Makefile $(INSTALL_ROOT)/include
	test -e $(INSTALL_ROOT)/lib/ControlGenHwDevice/_null || \
		mkdir $(INSTALL_ROOT)/lib/ControlGenHwDevice/_null
	chmod --recursive u+w,a-x,a+rX $(INSTALL_ROOT)/lib/ControlGenHwDevice \
		$(INSTALL_ROOT)/include/Device.Makefile
	@echo " . . . installation done"
