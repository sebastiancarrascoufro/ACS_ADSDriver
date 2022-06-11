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
//
// who      when     what
// pburgos  2009-07-22  created
//


/// Small snippet for getting the simulated serial number given its component
/// and assembly names.  Part of the Monitoring Archive FBT I effort for
/// getting unique serial number for Control Hardware Devices.


#include <iostream>
#include <iomanip>
#include <string>
#include <SimulatedSerialNumber.h>


int main(int argc, char* argv[])
{
    if(argc <= 2)
    {
        std::cout << "\n\nNot Enough Arguments!!!\n\n"
            ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n\n"
            "          Usage: "
            << argv[0]
            << " <ComponentName>  <AssemblyName>\n"
            "  \n"
            "          Example: getSimSerialNumber CONTROL/DA41/LO2BBpr0 LO2\n"
            "\n\n"
            ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n";
        return -1;
    }

    const std::string componentName(argv[1]);
    const std::string assemblyName(argv[2]);

    const uint64_t hashed_sn(
        AMB::Utils::getSimSerialNumber(componentName, assemblyName));

    std::cout << "ULL: "
        << hashed_sn
        << "\nHex: 0x"
        << std::hex
        << std::setw(8)
        << std::setfill('0')
        << hashed_sn
        << "\n";

    return 0;
}
