#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C.
#endif
#ifndef SimulatedSerialNumber_h
#define SimulatedSerialNumber_h
//
// ALMA - Atacama Large Millimiter Array
// (c) Associated Universities Inc., 2010
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA

#include <string>
#include <stdint.h>


namespace AMB
{
    namespace Utils
    {
        // Implements DJB hashing, intended for serial numbers creation in
        // simulation.
        uint32_t DJBHash(const std::string& str);

        uint64_t getSimSerialNumber(
            const std::string& componentName,
            const std::string& assemblyName);

        uint64_t getSimSerialNumber(
            uint64_t hashedComponentName,
            const std::string& deviceType);
    };
}
#endif
