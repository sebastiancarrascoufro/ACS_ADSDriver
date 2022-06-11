#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C.
#endif
/*******************************************************************************
 * ALMA - Atacama Large Millimiter Array
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
 *
 * who      when     what
 * tjuerges  Apr 8, 2009  created
 */


#include <vector>
#include <iterator>
#include <string>
#include <cmath>
#include <ControlExceptions.h>
#include <Guard_T.h>
#include <Mutex.h>
#include <TETimeUtil.h>


template< typename InputType, typename OutputType >
OutputType AMB::Converter< InputType, OutputType >::value(const InputType& val)
{
    return OutputType(val);
}

template< typename InputType, typename OutputType >
OutputType AMB::Converter< std::vector< InputType >, OutputType >::value(
    const std::vector< InputType >& val)
{
    OutputType x(val.size());
    /// TODO
    /// Thomas, Apr 17, 2009
    /// Is it necessary to set the length?
    ///
    x.length(val.size());
    std::size_t i(0U);
    for(typename std::vector< InputType >::const_iterator iter(val.begin());
        iter != val.end(); ++iter, ++i)
    {
        x[i] = (*iter);
    }

    return x;
}

template< typename InputType >
ACS::pattern AMB::Converter< InputType, ACS::pattern >::value(
    const InputType& val)
{
    return ACS::pattern(static_cast< ACS::pattern >(val));
}

template< typename InputType >
ACS::pattern AMB::Converter< std::vector< InputType >, ACS::pattern >::value(
    const std::vector< InputType >& val)
{
    if(val.size() > 8)
    {
        ControlExceptions::IllegalParameterErrorExImpl ex(__FILE__, __LINE__,
            "ACS::pattern AMB::Converter< std::vector< InputType >, "
            "ACS::pattern >::value(std::vector< InputType >&)");
        ex.log();
        throw ex;
    }

    ACS::pattern x(0U);
    std::size_t i(0U);
    for(typename std::vector< InputType >::const_iterator iter(val.begin());
        iter != val.end(); ++iter, ++i)
    {
        x |= static_cast<ACS::pattern>(*iter) << (i * 8 * sizeof(InputType));
    }

    return x;
}

template< typename Type >
void AMB::Utils::signFix(Type& raw, unsigned int numBytes)
{
    if(raw & (static_cast< Type >(0x1) << (numBytes * 8U - 1U)))
    {
        raw |= ((static_cast< Type >(0x1) << (sizeof(Type) - numBytes)
            * 8U) - 1U) << numBytes * 8U;
    }
}

template< typename Type >
void AMB::Utils::swapBytes(Type& raw, unsigned int numBytes)
{
    if(numBytes == 0U)
    {
        return;
    }

    char tmp(0);
    char* p(reinterpret_cast< char* >(&raw));
    for(unsigned int i(0U); i < (numBytes / 2U); ++i)
    {
        tmp = p[i];
        p[i] = p[numBytes - i - 1U];
        p[numBytes - i - 1U] = tmp;
    }
}

template< typename Type >
void AMB::Utils::swapBytes(Type& raw)
{
    swapBytes(raw, sizeof(Type));
}

template< typename CppVec, typename Corba, typename CorbaSeq,
    typename CorbaSeq_var >
CorbaSeq_var AMB::Utils::worldDataArrayToCorbaSeq(CppVec& y)
{
    CorbaSeq_var x(new CorbaSeq);
    x->length(y.size());
    std::size_t i(0U);
    for(typename CppVec::iterator iter(y.begin()); iter != y.end();
        ++iter, ++i)
    {
        x[i] = static_cast< Corba >(*iter);
    }

    return x._retn();
};

template< typename Raw, typename World >
Raw AMB::Utils::worldToRaw(const World& world, double offset, double scale)
{
    typename World::const_iterator iterWorld(world.begin());
    Raw raw(world.size());
    for(typename Raw::iterator iterRaw(raw.begin());
        iterWorld != world.end(); ++iterWorld, ++iterRaw)
    {
        if((typeid(typename Raw::value_type) == typeid(double))
        || (typeid(typename Raw::value_type) == typeid(float)))
        {
            // Do NOT round this value if the raw type is double or float!
            (*iterRaw) = static_cast< typename Raw::value_type >(
                ((*iterWorld) - offset) / scale);
        }
        else
        {
            (*iterRaw) = static_cast< typename Raw::value_type >(
                ::round(((*iterWorld) - offset) / scale));
        }
    }

    return raw;
}

template< typename Raw, typename World >
World AMB::Utils::rawToWorld(const Raw& raw, double offset, double scale)
{
    typename Raw::const_iterator iterRaw(raw.begin());
    World world(raw.size());
    for(typename World::iterator iterWorld(world.begin());
        iterRaw != raw.end(); ++iterRaw, ++iterWorld)
    {
        // Do NOT round this value if the world type is double or float!
        if((typeid(typename World::value_type) == typeid(double))
        || (typeid(typename World::value_type) == typeid(float)))
        {
            (*iterWorld) = static_cast< typename World::value_type >(
                (*iterRaw) * scale + offset);
        }
        else
        {
            (*iterWorld) = static_cast< typename World::value_type >(
                ::round((*iterRaw) * scale + offset));
        }
    }

    return world;
}


/// RODevIO methods
template< typename AssemblyBase, typename CPPType, typename CORBAType >
AMB::RODevIO< AssemblyBase, CPPType, CORBAType >::RODevIO(
    AssemblyBase* base, funcType mp):
    Logging::Loggable(),
    device(base),
    func(mp)
{
    AUTO_TRACE(__PRETTY_FUNCTION__);
}

template< typename AssemblyBase, typename CPPType, typename CORBAType >
AMB::RODevIO< AssemblyBase, CPPType, CORBAType >::RODevIO(
    AssemblyBase* base, funcType mp, const std::string& loggerName):
    Logging::Loggable(loggerName),
    device(base),
    func(mp)
{
    AUTO_TRACE(__PRETTY_FUNCTION__);
}

template< typename AssemblyBase, typename CPPType, typename CORBAType >
AMB::RODevIO< AssemblyBase, CPPType, CORBAType >::RODevIO(AssemblyBase* base,
        funcType mp, Logging::Logger::LoggerSmartPtr logger):
    Logging::Loggable(logger),
    device(base),
    func(mp)
{
    AUTO_TRACE(__PRETTY_FUNCTION__);
}

/// Implement the destructor method in the ACS DevIO class for this BACI
/// property.
template< typename AssemblyBase, typename CPPType, typename CORBAType >
AMB::RODevIO< AssemblyBase, CPPType, CORBAType >::~RODevIO()
{
    AUTO_TRACE(__PRETTY_FUNCTION__);
}

template< typename AssemblyBase, typename CPPType, typename CORBAType >
bool AMB::RODevIO< AssemblyBase, CPPType, CORBAType >::initializeValue()
{
    AUTO_TRACE(__PRETTY_FUNCTION__);

    return false;
}

template< typename AssemblyBase, typename CPPType, typename CORBAType >
CORBAType AMB::RODevIO< AssemblyBase, CPPType, CORBAType >::read(
    ACS::Time& timestamp)
{
    try
    {
        return converter.value((device->*func)(timestamp));
    }
    catch(const ControlExceptions::CAMBErrorExImpl& ex)
    {
        ControlExceptions::CAMBErrorExImpl nex(ex, __FILE__, __LINE__,
            __PRETTY_FUNCTION__);
        throw ACSErr::ACSbaseExImpl(nex);
    }
    catch(const ControlExceptions::INACTErrorExImpl& ex)
    {
        ControlExceptions::INACTErrorExImpl nex(ex, __FILE__, __LINE__,
            __PRETTY_FUNCTION__);
        throw ACSErr::ACSbaseExImpl(nex);
    }
}


/// RWDevIO methods
template< typename AssemblyBase, typename CPPType, typename CORBAType >
AMB::RWDevIO< AssemblyBase, CPPType, CORBAType >::RWDevIO(
    AssemblyBase* base, funcType mp):
    Logging::Loggable(),
    device(base),
    func(mp)
{
    AUTO_TRACE(__PRETTY_FUNCTION__);
}

template< typename AssemblyBase, typename CPPType, typename CORBAType >
AMB::RWDevIO< AssemblyBase, CPPType, CORBAType >::RWDevIO(
    AssemblyBase* base, funcType mp, const std::string& loggerName):
    Logging::Loggable(loggerName),
    device(base),
    func(mp)
{
    AUTO_TRACE(__PRETTY_FUNCTION__);
}

template< typename AssemblyBase, typename CPPType, typename CORBAType >
AMB::RWDevIO< AssemblyBase, CPPType, CORBAType >::RWDevIO(
    AssemblyBase* base, funcType mp, Logging::Logger::LoggerSmartPtr logger):
    Logging::Loggable(logger),
    device(base),
    func(mp)
{
    AUTO_TRACE(__PRETTY_FUNCTION__);
}

/// Implement the destructor method in the ACS DevIO class for this BACI
/// property.
template< typename AssemblyBase, typename CPPType, typename CORBAType >
AMB::RWDevIO< AssemblyBase, CPPType, CORBAType >::~RWDevIO()
{
    AUTO_TRACE(__PRETTY_FUNCTION__);
}

template< typename AssemblyBase, typename CPPType, typename CORBAType >
bool AMB::RWDevIO< AssemblyBase, CPPType, CORBAType >::initializeValue()
{
    AUTO_TRACE(__PRETTY_FUNCTION__);

    return false;
}

template< typename AssemblyBase, typename CPPType, typename CORBAType >
CORBAType AMB::RWDevIO< AssemblyBase, CPPType, CORBAType >::read(
    ACS::Time& timestamp)
{
    return cacheValue;
}

template< typename AssemblyBase, typename CPPType, typename CORBAType >
void AMB::RWDevIO< AssemblyBase, CPPType, CORBAType >::write(
    const CORBAType& value, ACS::Time& timestamp)
{
    try
    {
        CPPType x(converter.value(value));
        (device->*func)(value, timestamp);

    }
    catch(const ControlExceptions::CAMBErrorExImpl& ex)
    {
        ControlExceptions::CAMBErrorExImpl nex(ex, __FILE__, __LINE__,
            __PRETTY_FUNCTION__);
        throw ACSErr::ACSbaseExImpl(nex);
    }
    catch(const ControlExceptions::INACTErrorExImpl& ex)
    {
        ControlExceptions::INACTErrorExImpl nex(ex, __FILE__, __LINE__,
            __PRETTY_FUNCTION__);
        throw ACSErr::ACSbaseExImpl(nex);
    }
}

template< typename AssemblyBase, typename ParentType >
AMB::Cache< AssemblyBase, ParentType >::Cache(
    AssemblyBase* base, funcType pm, double time, ACS::Time& updTime):
    device(base),
    method(pm),
    duration(static_cast< ACS::Time >(time * TETimeUtil::ACS_ONE_SECOND)),
    last_id(0ULL),
    current(&updTime)
{
}

template< typename AssemblyBase, typename ParentType >
void AMB::Cache< AssemblyBase, ParentType >::getVal(ACS::Time& id)
{
    // Safe guarding the access of the cached values.
    ACE_Guard< ACE_Mutex > guard(accessMutex);

    // Only read the current value from the hardware if one of two constraints
    // are fulfilled:
    // 1. The same ID, i.e. the same dependent monitor point shall be read
    // again.
    // 2. The  best-before-date has been reached and it is time to update the
    // parent monitor point.  This will update the wanted dependent monitor
    // point, too.
    if((&id == last_id)
    ||((*current - id) > duration))
    {
        try
        {
            (device->*method)(*current);
        }
        catch(const ControlExceptions::CAMBErrorExImpl& ex)
        {
            ControlExceptions::CAMBErrorExImpl nex(ex, __FILE__, __LINE__,
                __PRETTY_FUNCTION__);
            throw nex;
        }
        catch(const ControlExceptions::INACTErrorExImpl& ex)
        {
            ControlExceptions::INACTErrorExImpl nex(ex, __FILE__, __LINE__,
                __PRETTY_FUNCTION__);
            throw nex;
        }
    }

    last_id = &id;
}
