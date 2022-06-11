#ifndef _ControlPointArchival_H_
#define _ControlPointArchival_H_
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
* "@(#) $Id: LTS2H.xslt,v 1.7 2012/03/06 19:16:56 tstaig Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* nbarriga  2007-01-30  created
*/

/************************************************************************
 *
 *----------------------------------------------------------------------
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <string>
#include <logging_idlS.h>
#include <loggingACSLogger.h>
#include <logging.h>
#include <TypeSafeLog.h>

using namespace std;

namespace ControlPointArchival{

class ControlPointUsed:public Logging::TypeSafeLog{
	private:
                Logging::BaseLog::Priority priority;
                string file;
                unsigned long line;
                string routine;
                string name;
                string audience;
                string array;
                string antenna;
                string shortDescription;
                ACSLoggingLog::NameValueSeq members;
	protected:

	public:
		ControlPointUsed(string file,
                        unsigned long line,
                        string routine);
		ControlPointUsed(string file,
                        unsigned long line,
                        string routine,
                        string array,
                        string antenna);
                void init(string file, unsigned long line, string routine);
                virtual ~ControlPointUsed();
		void log();
                void setArray(string array);
                void setAntenna(string antenna);
                string getArray();
                string getAntenna();
		void setComponent(string value);
		void setControlPoint(string value);
		void setNewValue(string value);

};


}

#endif
