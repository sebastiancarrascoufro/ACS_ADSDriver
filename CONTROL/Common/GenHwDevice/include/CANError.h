#ifndef CANError_h
#define CANError_h
#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C.
#endif
/*******************************************************************************
 * ALMA - Atacama Large Millimeter Array
 * (c) Associated Universities Inc., 2009
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 */


#include <string>
#include <iostream>


namespace CAN
{

    // Meant only to be thrown
    struct Error
    {
        public:
        Error()
        {
        };

        Error(const std::string& msg):
            errorMsg(msg)
        {
        };

        std::string errorMsg;
    };

    inline std::ostream& operator<<(std::ostream& os, const CAN::Error& error)
    {
        os << error.errorMsg << "\n";
        return os;
    };

}
#endif
