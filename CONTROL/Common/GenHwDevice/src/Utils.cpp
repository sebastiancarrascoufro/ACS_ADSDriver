// ALMA - Atacama Large Millimeter Array
// (c) Associated Universities Inc., 2014
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA

#include "Utils.h"
#include <vector>
#include <iterator>
#include <ControlExceptions.h>

using std::vector;

ACS::pattern AMB::Converter< vector< bool >, ACS::pattern >
::value(const vector< bool >& val) {
    if (val.size() > 64) {
        ControlExceptions::IllegalParameterErrorExImpl ex(__FILE__, __LINE__,
            "ACS::pattern AMB::Converter< std::vector< bool >, "
            "ACS::pattern >::value(std::vector< bool >&)");
        ex.log();
        throw ex;
    }

    ACS::pattern x(0U);
    std::size_t i(0U);
    for (vector< bool >::const_iterator iter(val.begin());
        iter != val.end(); ++iter, ++i)
    {
        x |= static_cast<ACS::pattern>(*iter) << i;
    }

    return x;
}

