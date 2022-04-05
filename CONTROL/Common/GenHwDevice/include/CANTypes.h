#ifndef CANTypes_h
#define CANTypes_h
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


namespace CAN
{
    typedef unsigned int id_t;
    typedef unsigned short datalen_t;
    typedef unsigned char byte_t;
}


namespace AMB
{
    typedef unsigned short node_t;
    typedef unsigned int rca_t;

    // Various interesting numbers
    enum
    {
        BUS_INIT = 0,
        MAX_AMB_NODE_NUMBER = 2031,
        BROADCAST_TOP = 0x0003FFFF
    };
}
#endif
