# ALMA - Atacama Large Millimeter Array
# (c) Associated Universities Inc., 2005 - 2010, 2014
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
# C/C++ programs (public and local)
# -----------------------------


# EXECUTABLES_L = cppTestingServer
EXECUTABLES_L = ADSTimeAtNextTE




ADSTimeAtNextTE_OBJECTS = ADSTimeAtNextTE
ADSTimeAtNextTE_LIBS =  adsDevIO logging  
ADSTimeAtNextTE_LIBS += TETimeUtil ControlExceptions
# Necessary to make semaphores work on RHEL5.3
ADSTimeAtNextTE_LIBS += pthread








#
# Includes (.h) files (public and local)
# ---------------------------------
# INCLUDES_L = cppTestingServer.h 

#
# Libraries (public and local)
# ----------------------------
#LIBRARIES_L = ambDevIOTest
#LIBRARIES_L += TestControlDevice

#ambDevIOTest_OBJECTS = ambDevIOTestImpl 
#ambDevIOTest_LIBS = ambDevIOTestStubs
#ambDevIOTest_LIBS += oldAmbDeviceImpl

#TestControlDevice_OBJECTS = TestControlDeviceImpl
#TestControlDevice_LIBS = controlDevice

#
# Scripts (public and local)
# ----------------------------
#SCRIPTS_L = ambDevIOTest.py

#
# Python stuff (public and local)
# ----------------------------
#PY_SCRIPTS_L = ambDevIOTest

#
# On-Line Database Files
# ----------------------
#CDB_SCHEMAS = AmbDevIOTest

#
# IDL FILES
# ----------------------
#IDL_FILES = ambDevIOTest


#
# list of all possible C-sources (used to create automatic dependencies)
# ------------------------------
CSOURCENAMES = \
	$(foreach exe, $(EXECUTABLES) $(EXECUTABLES_L), $($(exe)_OBJECTS)) \
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
all: do_all
	@echo " . . . 'all' done"

clean: clean_all
	$(RM) *~ tatlogs sed.scan tmp .TestList.sed .testSession .purify
	@echo " . . . clean done"

clean_dist: clean clean_dist_all
	@echo " . . . clean_dist done"

man: do_man
	@echo " . . . man page(s) done"

install: install_all
	@echo " . . . installation done"
