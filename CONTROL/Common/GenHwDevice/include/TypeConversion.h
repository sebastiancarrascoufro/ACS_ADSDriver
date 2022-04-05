#ifndef TypeConversion_h
#define TypeConversion_h

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C.
#endif

//*****************************************************************************
// ALMA - Atacama Large Millimeter Array
// (c) Associated Universities Inc., 2009, 2017
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
//*****************************************************************************

// STL
#include <algorithm>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>
// Linux
#include <endian.h>
// ACS
#include <CANError.h>
#include <CANTypes.h>
#include <TETimeUtil.h>

namespace AMB {
// This class provides all necessary tools to convert from normal values to
// CAN bus vectors, and vice versa. C++ templates are used to support all
// allowed data types (see ../test/TestTypeConversion.cpp).
// WARNING: Not tested with string types!

namespace TypeConversion {

namespace detail {

template <typename Type>
static void pack(std::vector<CAN::byte_t>&, const Type&,
                 std::size_t, std::size_t);
template <typename Type>
static Type unpack(const std::vector<CAN::byte_t>&,
                   std::size_t, std::size_t);

} // namespace detail

// Convert a vector of Types to a CAN::byte_t vector, specifying the
// required number of bytes for each CAN::byte_t.
template<typename Type>
void valueToData(std::vector<CAN::byte_t> &data,
                 const std::vector<Type> &values,
                 std::size_t nbytes) {
    if(values.size() * nbytes > 8) {
        throw CAN::Error("CAN data cannot contain more than 8 bytes.");
    }

    data.clear();
    data.reserve(values.size() * nbytes);
    for(std::size_t i = 0; i < values.size(); ++i) {
        detail::pack(data, values[i], i*nbytes, nbytes);
    }
}

// Convert a vector of Types to a CAN::byte_t vector. The number of
// bytes of the Type data type is assumed for each CAN::byte_t.
template<typename Type>
void valueToData(std::vector<CAN::byte_t> &data,
                 const std::vector<Type> &value) {
    valueToData(data, value, sizeof(Type));
}

// Convert a Type value to a CAN::byte_t vector, specifying the
// required number of bytes for each CAN::byte_t.
template<typename Type>
void valueToData(std::vector<CAN::byte_t> &data,
                 const Type &value,
                 std::size_t nbytes) {
    const std::vector<Type> vvalue(1, value);
    valueToData(data, vvalue, nbytes);
}

// Convert a Type value to a CAN::byte_t vector. The number of bytes of
// the Type data type is assumed for each CAN::byte_t.
template<typename Type>
void valueToData(std::vector<CAN::byte_t> &data,
                 const Type &value) {
    valueToData(data, value, sizeof(Type));
}

// Convert a CAN::byte_t vector to a vector of Type by using a
// particular number of bytes.
template<typename Type>
void dataToValue(const std::vector<CAN::byte_t> &data,
                 std::vector<Type> &values,
                 std::size_t nbytes) {
    if(nbytes > sizeof(Type)) {
        std::ostringstream out;
        out << "Converted CAN data does not fit into output data type. "
            << "sizeof(" << typeid(Type).name() << ") = " << sizeof(Type)
            << ", number of bytes to convert = " << nbytes << ".";
        throw CAN::Error(out.str().c_str());
    } else if((data.size() % nbytes) != 0) {
        std::ostringstream out;
        out << "Not all CAN data can be converted because slices are "
            << "not compatible with the target data type. Length of the "
            << "raw data vector = " << data.size() << ", number of raw "
            << "data bytes per target data type = " << nbytes << ".";
        throw CAN::Error(out.str().c_str());
    } else if(data.empty()) {
        std::ostringstream out;
        out << "CAN data vector is empty.";
        throw CAN::Error(out.str().c_str());
    }

    values.resize(data.size() / nbytes);
    for(std::size_t i = 0; i < values.size(); ++i) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        values[i] =
#else
        values[values.size()-i-1] =
#endif
            detail::unpack<Type>(data, i*nbytes, nbytes);
    }
}

// Convert a CAN::byte_t vector to a vector of Types.
template<typename Type>
void dataToValue(const std::vector<CAN::byte_t> &data,
                 std::vector<Type> &values) {
    if((data.size() % sizeof(Type)) != 0) {
        throw CAN::Error("CAN data is not compatible with data type.");
    }

    dataToValue(data, values, sizeof(Type));
}

// Convert a CAN::byte_t vector to a Type value.
template<typename Type>
void dataToValue(const std::vector<CAN::byte_t> &data,
                 Type &value) {
    std::vector<Type> vvalue;
    dataToValue(data, vvalue);
    value = vvalue[0];
}

// Convert a CAN::byte_t vector to a Type value by using a particular
// number of bytes.
template<typename Type>
void dataToValue(const std::vector<CAN::byte_t> &data,
                 Type &value,
                 std::size_t nbytes) {
    std::vector<Type> vvalue;
    dataToValue(data, vvalue, nbytes);
    value = vvalue[0];
}

namespace detail {

template <typename Type>
union Conversion {
    CAN::byte_t bytes[sizeof(Type)];
    Type value;
};

template <typename Type>
inline void pack(std::vector<CAN::byte_t> &data,
                 const Type &value,
                 std::size_t first,
                 std::size_t nbytes) {
    Conversion<Type> conv;
    conv.value = value;
    // Copy the least significant bytes
#if __BYTE_ORDER == __LITTLE_ENDIAN
    std::reverse_copy
#else
    std::copy
#endif
        (&conv.bytes[0],
         &conv.bytes[nbytes],
         std::back_inserter(data));
}

template <typename Type>
inline Type unpack(const std::vector<CAN::byte_t> &data,
                   std::size_t first,
                   std::size_t nbytes) {
    Conversion<Type> conv;
    conv.value = 0;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    std::reverse_copy(&data[first], &data[first+nbytes], &conv.bytes[0]);
#else
    std::copy_backward(&data[first], &data[first+nbytes], &conv.bytes[sizeof(Type)]);
#endif
    return conv.value;
}

} // namespace detail

} // namespace TypeConversion

} // namespace AMB

#endif//TypeConversion_h
