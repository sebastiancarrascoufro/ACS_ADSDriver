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

#include <SimulatedSerialNumber.h>


unsigned int AMB::Utils::DJBHash(const std::string& str)
{
   unsigned int hash(5381U);

   for(std::size_t i(0U); i < str.size(); ++i)
   {
      hash = ((hash << 5U) + hash) + str[i];
   }

   return hash;
}

uint64_t AMB::Utils::getSimSerialNumber(
    const std::string& componentName, const std::string& assemblyName)
{
    const uint64_t hashed_upper_sn((
        static_cast< uint64_t >(
            AMB::Utils::DJBHash(componentName))) << 32ULL);
    const uint64_t hashed_lower_sn(static_cast< uint64_t >(
        AMB::Utils::DJBHash(assemblyName)));
    const uint64_t hashed_sn(hashed_upper_sn  | hashed_lower_sn);

    return hashed_sn;
}

uint64_t AMB::Utils::getSimSerialNumber(
    uint64_t hashedComponentName,
    const std::string& deviceType)
{
    const uint64_t hashed_upper_sn(hashedComponentName << 32ULL);
    const uint64_t hashed_lower_sn(static_cast< uint64_t >(
        AMB::Utils::DJBHash(deviceType)));
    const uint64_t hashed_sn(hashed_upper_sn  | hashed_lower_sn);

    return hashed_sn;
}
