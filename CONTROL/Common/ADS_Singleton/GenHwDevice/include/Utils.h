#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C.
#endif
#ifndef Utils_h
#define Utils_h
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


#include <Mutex.h>
#include <baciDevIO.h>
#include <acscommonC.h>
#include <loggingLoggable.h>
#include <loggingLogger.h>


namespace AMB
{
    namespace Utils
    {
        template< typename Type >
        static void signFix(Type& raw, unsigned int numBytes);

        template< typename Type >
        static void swapBytes(Type& raw, unsigned int numBytes);

        template< typename Type >
        static void swapBytes(Type& raw);

        template< typename CppVec, typename Corba, typename CorbaSeq,
            typename CorbaSeq_var >
        static CorbaSeq_var worldDataArrayToCorbaSeq(CppVec& y);

        template< typename Raw, typename World >
        static Raw worldToRaw(const World& world, double offset, double scale);

        template< typename Raw, typename World >
        static World rawToWorld(const Raw& raw, double offset, double scale);
    };

    template< typename InputType, typename OutputType >
    class Converter
    {
        public:
        OutputType value(const InputType& val);
    };

    template< class InputType, class OutputType >
    class Converter< std::vector< InputType >, OutputType>
    {
       public:
       OutputType value(const std::vector< InputType >& val);
    };

    template< class InputType >
    class Converter<InputType, ACS::pattern>
    {
        public:
        ACS::pattern value(const InputType& val);
    };

    template< class InputType >
    class Converter< std::vector< InputType >, ACS::pattern >
    {
        public:
        ACS::pattern value(const std::vector< InputType >& val);
    };

    template< >
    class Converter< std::vector< bool >, ACS::pattern >
    {
        public:
        ACS::pattern value(const std::vector< bool >& val);
    };

    /// Cache Class: decides whether the cached value needs to be updated
    /// for the dependent monitor points.
    /// An update will be done only if at least one of two constraints is true:
    ///
    /// 1. The same ID, i.e. the same dependent monitor point shall be read
    /// twice in a row.
    /// Example (pseudo language):
    /// monitorPointValue = monitorTE(RCA = 0x100,
    ///     targetTimestamp = 0xabcdefgh000000)
    /// will return a cached value no matter how hard you try to get the "real"
    /// value.
    /// If you read now the same monitor point twice in a row:
    /// monitorPointValue = monitorTE(RCA = 0x100,
    ///     targetTimestamp = 0xabcdefgh000000)
    /// monitorPointValue = monitorTE(RCA = 0x100,
    ///     targetTimestamp = 0xabcdefgh000000)
    /// then monitorPointValue will contain the real current value.
    ///
    /// 2. The duration has expired (best-before-date has been reached) and it
    /// is time to update the parent monitor point anyway.  This will update the
    /// wanted dependent monitor point, too.
    ///
    /// \note Due to the current design, it is possible that already a single
    /// read of a dependent monitor point will cause a reread from the hardware.
    /// Imagine this case:
    /// Thread 1 reads a dependent monitor point A frequently and tries to keep
    /// track of the real hardware value.  It does so by reading the monitor
    /// point twice in a row.  Now Thread 2 is instructed to read the monitor
    /// point A, too.  The following situation can happen:
    ///
    /// - Thread 1 wants to read monitor point A and will be presented a cached
    /// value.  Okay, nothing wrong yet.
    /// - Now Thread 2 preempts thread 1 and reads monitor point A.  The second
    /// monitor request will trigger a read from the hardware.
    /// - Thread 1 becomes active again and sends the second read to trigger the
    /// - expected by Thread 1 - reread but it won't happen unless the duration
    /// has expired.
    template< typename AssemblyBase, typename ParentType >
    class Cache
    {
        /// Typedef for functor that reads a value from a hardware device.
        typedef ParentType (AssemblyBase::*funcType)(ACS::Time&);

        public:
        /// Constructor.
        Cache(AssemblyBase* base, funcType pm, double time,
            ACS::Time& updTime);

        /// Return a cached value or trigger a reread of a parent monitor point.
        void getVal(ACS::Time& id);


        private:
        /// Pointer for the AMB device.
        AssemblyBase* device;

        /// Functor that reads values from the hardware.
        funcType method;

        /// Minimum duration between two hardware reads.
        const ACS::Time duration;

        /// ID of last access, i.e. what RCA was last accessed?
        ACS::Time* last_id;

        /// Time stamp of current TE.  Updated regularly by the Time thread.
        ACS::Time* current;

        /// Mutex for protection of \ref current and \ref last_id.
        ACE_Mutex accessMutex;
    };


    /// RODevIO Template Base Class.
    template< typename AssemblyBase, typename CPPType, typename CORBAType >
    class RODevIO: public DevIO< CORBAType >, public Logging::Loggable
    {
        /// Typedef for functor that reads a value from a hardware device.
        typedef CPPType (AssemblyBase::*funcType)(ACS::Time&);

        public:
        /// Constructor.
        RODevIO(AssemblyBase* base, funcType mp);

        /// Constructor.
        RODevIO(AssemblyBase* base, funcType mp, const std::string& loggerName);

        /// Constructor.
        RODevIO(AssemblyBase* base, funcType mp,
            Logging::Logger::LoggerSmartPtr logger);

        /// Destructor.
        virtual ~RODevIO();

        /// Value initialisation enabled?
        /// \return true or false.
        virtual bool initializeValue();

        /// \return current value.
        /// \exception ACSErr::ACSbaseExImpl
        virtual CORBAType read(ACS::Time& timestamp);


        protected:
        /// No normal constructor.
        RODevIO();

        /// ALMA coding standards: copy constructor is disabled.
        RODevIO(const RODevIO& other);

        /// ALMA coding standards: assignment operator is disabled.
        RODevIO& operator=(const RODevIO& other);

        Converter< CPPType, CORBAType > converter;

        AssemblyBase* device;

        funcType func;
    };


    ///RWDevIO Template Base Class.
    template< typename AssemblyBase, typename CPPType, typename CORBAType >
    class RWDevIO: public DevIO< CORBAType >, public Logging::Loggable
    {
        /// Typedef for functor that writes a value to a hardware device.
        typedef void (AssemblyBase::*funcType)(CPPType&, ACS::Time&);

        public:
        /// Constructor.
        RWDevIO(AssemblyBase* base, funcType mp);

        /// Constructor.
        RWDevIO(AssemblyBase* base, funcType mp, const std::string& loggerName);

        /// Constructor.
        RWDevIO(AssemblyBase* base, funcType mp,
            Logging::Logger::LoggerSmartPtr logger);

        /// Destructor.
        virtual ~RWDevIO();

        /// Value initialisation enabled?
        /// \return true or false.
        virtual bool initializeValue();

        /// \return current value.
        /// \exception ACSErr::ACSbaseExImpl
        virtual CORBAType read(ACS::Time& timestamp);

        /// Write a new value.
        /// \param value: new value.
        /// \exception ACSErr::ACSbaseExImpl
        virtual void write(const CORBAType& value, ACS::Time& timestamp);


        protected:
        /// No normal constructor.
        RWDevIO();

        /// ALMA coding standards: copy constructor is disabled.
        RWDevIO(const RWDevIO& other);

        /// ALMA coding standards: assignment operator is disabled.
        RWDevIO& operator=(const RWDevIO& other);

        Converter< CPPType, CORBAType > converter;

        AssemblyBase* device;

        funcType func;

        /// Cached value.
        CORBAType cacheValue;
    };
};

// Include the implementations of the template classes.
#include <Utils.i>
#endif
