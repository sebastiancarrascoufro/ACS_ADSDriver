#!/bin/bash

##############################################################################
# ALMA - Atacama Large Millimiter Array
# (c) European Southern Observatory, 2002
# Copyright by ESO (in the framework of the ALMA collaboration),
# All rights reserved
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
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307  USA

# Documentation about the test goes here.
#
#

export CLASSPATH=$CLASSPATH:../lib/AddGenericMonitorPointTest.jar:../lib/junit-4.4.jar
declare TEST_CLASS=alma.Control.GenHwDevice.Test.AddGenericMonitorPointTest
declare TEST_LOG=/dev/stdout

if test $# -gt 1; then
  TEST_SUITE=$1
  if test $# -eq 2; then
    TEST_LOG=$2
  fi
fi

acsStartJava  -endorsed org.junit.runner.JUnitCore $TEST_CLASS &> "$TEST_LOG"

RESULT=$?
if [ "$RESULT" = "0" ]; then
    printf "OK\n"
else
    printf "ERROR\n"
fi

# __PBA__

