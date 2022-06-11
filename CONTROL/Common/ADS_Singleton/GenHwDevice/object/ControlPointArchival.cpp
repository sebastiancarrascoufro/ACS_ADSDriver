/*******************************************************************************
* ALMA - Atacama Large Millimiter Array
* Copyright (c) ESO - European Southern Observatory, 2011
* (in the framework of the ALMA collaboration).
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
* "@(#) $Id: LTS2Cpp.xslt,v 1.9 2012/03/06 19:16:56 tstaig Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* nbarriga  2007-01-30  created
*/

#include "vltPort.h"

#include "ControlPointArchival.h"

#include <acsutilTimeStamp.h>
#include <sstream>

using namespace ControlPointArchival;

ControlPointUsed::ControlPointUsed(string file, unsigned long line, string routine){
                        init(file, line, routine);
                }
    void ControlPointUsed::init(string file, unsigned long line, string routine){
	this->priority=Logging::ace2acsPriority(ACE_Log_Priority(LM_DEBUG));
	this->file=file;
	this->line=line;
	this->routine=routine;
	this->name="ControlPointUsed";
	this->audience="System Engineers";
	this->shortDescription="A control point has been invoked.";

}

ControlPointUsed::ControlPointUsed(string file, unsigned long line, string routine, string array, string antenna){
        init(file, line, routine);
        this->array=array;
        this->antenna=antenna;
}        
ControlPointUsed::~ControlPointUsed(){

}
void ControlPointUsed::setArray(string array){
        this->array=array;
}
void ControlPointUsed::setAntenna(string antenna){
        this->antenna=antenna;
}

string ControlPointUsed::getArray(){
        return array;
}
string ControlPointUsed::getAntenna(){
        return antenna;
}
void ControlPointUsed::log(){
        Logging::BaseLog::LogRecord lr;
        lr.priority=this->priority;
        lr.message=this->shortDescription;
        lr.file=this->file;
        lr.line=this->line;
        lr.method=this->routine;
        lr.timeStamp=getTimeStamp();
        LoggingProxy::AddData("logName",this->name.c_str());
        //LoggingProxy::AddData("audience",this->audience.c_str());
        LoggingProxy::audience(this->audience.c_str());
        if(this->array.length()!=0)LoggingProxy::array(this->array.c_str());
        if(this->antenna.length()!=0)LoggingProxy::antenna(this->antenna.c_str());
        for(unsigned int i=0;i<members.length();i++){
                LoggingProxy::AddData(members[i].name.in(),members[i].value.in());
        }
        LoggingProxy::Flags(LM_SOURCE_INFO | LM_RUNTIME_CONTEXT);
        getLogger()->log(lr);
		LoggingProxy::audience(NULL);

}

void ControlPointUsed::setComponent(string value){
	ACSLoggingLog::NameValue nv;
	nv.name=CORBA::string_dup("Component");
	nv.value=CORBA::string_dup(value.c_str());
	members.length(members.length()+1);
	members[members.length()-1]=nv;

}

void ControlPointUsed::setControlPoint(string value){
	ACSLoggingLog::NameValue nv;
	nv.name=CORBA::string_dup("ControlPoint");
	nv.value=CORBA::string_dup(value.c_str());
	members.length(members.length()+1);
	members[members.length()-1]=nv;

}

void ControlPointUsed::setNewValue(string value){
	ACSLoggingLog::NameValue nv;
	nv.name=CORBA::string_dup("NewValue");
	nv.value=CORBA::string_dup(value.c_str());
	members.length(members.length()+1);
	members[members.length()-1]=nv;

}



/*___oOo___*/
