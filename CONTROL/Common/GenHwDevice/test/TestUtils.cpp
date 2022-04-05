/**
 * ALMA - Atacama Large Millimeter Array
 * (c) European Southern Observatory, 2002
 * (c) Associated Universities Inc., 2002
 * Copyright by ESO (in the framework of the ALMA collaboration),
 * Copyright by AUI (in the framework of the ALMA collaboration),
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307  USA
 */


#include <iostream>
// For std::numeric< T > numbers.
#include <limits>
// For [u]intXX_t types.
#include <stdint.h>
// For ACE_Time_Value and ACE_OS::select
#include <OS_NS_sys_select.h>

#include <Utils.h>
#include <HardwareDeviceC.h>
#include <TestRODevIO.h>
#include <TestCache.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


/**
 * A test case for the Utils class
 *
 */
class UtilsTestCase: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(UtilsTestCase);
    CPPUNIT_TEST(test_signFix);
    CPPUNIT_TEST(test_swapBytes);
    CPPUNIT_TEST(test_worldDataArrayToCorbaSeq);
    CPPUNIT_TEST(test_worldToRaw);
    CPPUNIT_TEST(test_rawToWorld);
    CPPUNIT_TEST(test_RODevIO);
    CPPUNIT_TEST(test_Cache);
    CPPUNIT_TEST_SUITE_END();


    public:
    virtual void setUp();
    virtual void tearDown();


    protected:
    void test_signFix();
    void test_swapBytes();
    void test_worldDataArrayToCorbaSeq();
    void test_worldToRaw();
    void test_rawToWorld();
    void test_RODevIO();
    void test_Cache();
    template< class Type > void testSwapBytes(Type exp, Type act);
    template< class Type > void testSwapBytes(Type exp, Type act,
                                              std::size_t size);
};


CPPUNIT_TEST_SUITE_REGISTRATION(UtilsTestCase);


void UtilsTestCase::setUp()
{
}

void UtilsTestCase::tearDown()
{
}

void UtilsTestCase::test_signFix()
{
    //int24
    int p = 0x800000 - 1;
    AMB::Utils::signFix(p, 3U);
    CPPUNIT_ASSERT_EQUAL(static_cast< int >(0x800000 - 1), p);
    p = 0x800000;
    AMB::Utils::signFix(p, 3U);
    CPPUNIT_ASSERT_EQUAL(static_cast< int >(0xFF800000), p);

    // int48
    int64_t q = 0x800000000000LL - 1LL;
    AMB::Utils::signFix(q, 6U);
    CPPUNIT_ASSERT_EQUAL(static_cast< int64_t >(0x800000000000LL - 1LL), q);
    q = 0x800000000000LL;
    AMB::Utils::signFix(q, 6U);
    CPPUNIT_ASSERT_EQUAL(static_cast< int64_t >(0xFFFF800000000000LL), q);
}

template< class Type >
void UtilsTestCase::testSwapBytes(Type exp, Type act)
{
    AMB::Utils::swapBytes(act);
    CPPUNIT_ASSERT_EQUAL(exp, act);
}

template< class Type >
void UtilsTestCase::testSwapBytes(Type exp, Type act, std::size_t size)
{
    AMB::Utils::swapBytes(act, size);
    CPPUNIT_ASSERT_EQUAL(exp, act);
}

void UtilsTestCase::test_swapBytes()
{
    // int8
    testSwapBytes(static_cast< int8_t >(0x0), static_cast< int8_t >(0x0));
    testSwapBytes(static_cast< int8_t >(0xA), static_cast< int8_t >(0xA));
    testSwapBytes(static_cast< int8_t >(0xFF), static_cast< int8_t >(0xFF));
    testSwapBytes(static_cast< int8_t >(0xAF), static_cast< int8_t >(0xAF));
    testSwapBytes(static_cast< int8_t >(0xAC), static_cast< int8_t >(0xAC));

    // int16
    testSwapBytes(static_cast< int16_t >(0x0),
        static_cast< int16_t >(0x0));
    testSwapBytes(static_cast< int16_t >(0xA00),
        static_cast< int16_t >(0xA));
    testSwapBytes(static_cast< int16_t >(0xA),
        static_cast< int16_t >(0xA00));
    testSwapBytes(static_cast< int16_t >(0xFFFF),
        static_cast< int16_t >(0xFFFF));
    testSwapBytes(static_cast< int16_t >(0xAEC2),
        static_cast< int16_t >(0xC2AE));
    testSwapBytes(static_cast< int16_t >(0x17F1),
        static_cast< int16_t >(0xF117));

    // int24
    testSwapBytes(static_cast< int32_t >(0x0), static_cast< int32_t >(0x0), 3U);
    testSwapBytes(static_cast< int32_t >(0xA0000),
        static_cast< int32_t >(0xA), 3U);
    testSwapBytes(static_cast< int32_t >(0x200),
        static_cast< int32_t >(0x200), 3U);
    testSwapBytes(static_cast< int32_t >(0xA),
        static_cast< int32_t >(0xA0000), 3U);
    testSwapBytes(static_cast< int32_t >(0xFFFFFF),
        static_cast< int32_t >(0xFFFFFF), 3U);
    testSwapBytes(static_cast< int32_t >(0xC3F1A0),
        static_cast< int32_t >(0xA0F1C3), 3U);
    testSwapBytes(static_cast< int32_t >(0xFFA2B1),
        static_cast< int32_t >(0xB1A2FF), 3U);
    testSwapBytes(static_cast< int32_t >(0xC4D1BB),
        static_cast< int32_t >(0xBBD1C4), 3U);
    testSwapBytes(static_cast< int32_t >(0x070001),
        static_cast< int32_t >(0x010007), 3U);

    // int32
    testSwapBytes(static_cast< int32_t >(0x0),
        static_cast< int32_t >(0x0));
    testSwapBytes(static_cast< int32_t >(0xA000000),
        static_cast< int32_t >(0xA));
    testSwapBytes(static_cast< int32_t >(0x20000),
        static_cast< int32_t >(0x200));
    testSwapBytes(static_cast< int32_t >(0x200),
        static_cast< int32_t >(0x20000));
    testSwapBytes(static_cast< int32_t >(0xA),
        static_cast< int32_t >(0xA000000));
    testSwapBytes(static_cast< int32_t >(0xFFFFFFFF),
        static_cast< int32_t >(0xFFFFFFFF));
    testSwapBytes(static_cast< int32_t >(0xB9C2A117),
        static_cast< int32_t >(0x17A1C2B9));
    testSwapBytes(static_cast< int32_t >(0x011751A0),
        static_cast< int32_t >(0xA0511701));
    testSwapBytes(static_cast< int32_t >(0xE5A3B1B0),
        static_cast< int32_t >(0xB0B1A3E5));
    testSwapBytes(static_cast< int32_t >(0x5C3AA4AA),
        static_cast< int32_t >(0xAAA43A5C));

    // int48
    testSwapBytes(static_cast< int64_t >(0x0LL),
        static_cast< int64_t >(0x0LL), 6U);
    testSwapBytes(static_cast< int64_t >(0xA0000000000LL),
        static_cast< int64_t >(0xALL), 6U);
    testSwapBytes(static_cast< int64_t >(0x200000000LL),
        static_cast< int64_t >(0x200LL), 6U);
    testSwapBytes(static_cast< int64_t >(0x4000000LL),
        static_cast< int64_t >(0x40000LL), 6U);
    testSwapBytes(static_cast< int64_t >(0x40000LL),
        static_cast< int64_t >(0x4000000LL), 6U);
    testSwapBytes(static_cast< int64_t >(0x200LL),
        static_cast< int64_t >(0x200000000LL), 6U);
    testSwapBytes(static_cast< int64_t >(0xALL),
        static_cast< int64_t >(0xA0000000000LL), 6U);
    testSwapBytes(static_cast< int64_t >(0xFFFFFFFFFFFFLL),
        static_cast< int64_t >(0xFFFFFFFFFFFFLL),
        6U);
    testSwapBytes(static_cast< int64_t >(0x0C5BA7F99C01LL),
        static_cast< int64_t >(0x019CF9A75B0CLL),
        6U);
    testSwapBytes(static_cast< int64_t >(0xB4778A9BF1C0LL),
        static_cast< int64_t >(0xC0F19B8A77B4LL),
        6U);
    testSwapBytes(static_cast< int64_t >(0x57788ABCD1D5LL),
        static_cast< int64_t >(0xD5D1BC8A7857LL),
        6U);
    testSwapBytes(static_cast< int64_t >(0xB1F1565A9CD1LL),
        static_cast< int64_t >(0xD19C5A56F1B1LL),
        6U);

    // int64
    testSwapBytes(static_cast< int64_t >(0x0LL), static_cast< int64_t >(0x0LL));
    testSwapBytes(static_cast< int64_t >(0xA00000000000000LL),
        static_cast< int64_t >(0xALL));
    testSwapBytes(static_cast< int64_t >(0x2000000000000LL),
        static_cast< int64_t >(0x200LL));
    testSwapBytes(static_cast< int64_t >(0x40000000000LL),
        static_cast< int64_t >(0x40000LL));
    testSwapBytes(static_cast< int64_t >(0x600000000LL),
        static_cast< int64_t >(0x6000000LL));
    testSwapBytes(static_cast< int64_t >(0x6000000LL),
        static_cast< int64_t >(0x600000000LL));
    testSwapBytes(static_cast< int64_t >(0x40000LL),
        static_cast< int64_t >(0x40000000000LL));
    testSwapBytes(static_cast< int64_t >(0x200LL),
        static_cast< int64_t >(0x2000000000000LL));
    testSwapBytes(static_cast< int64_t >(0xALL),
        static_cast< int64_t >(0xA00000000000000LL));
    testSwapBytes(static_cast< int64_t >(0xFFFFFFFFFFFFFFFFLL),
        static_cast< int64_t >(0xFFFFFFFFFFFFFFFFLL));
    testSwapBytes(static_cast< int64_t >(0xCD6719D9741821C4LL),
        static_cast< int64_t >(0xC4211874D91967CDLL));
    testSwapBytes(static_cast< int64_t >(0x009871000AB57C01LL),
        static_cast< int64_t >(0x017CB50A00719800LL));
    testSwapBytes(static_cast< int64_t >(0xBA7C9AA7AABFF5A4LL),
        static_cast< int64_t >(0xA4F5BfAAA79A7CBALL));
    testSwapBytes(static_cast< int64_t >(0xF7C205D100BCB777LL),
        static_cast< int64_t >(0x77B7BC00D105C2F7LL));

    // uint8
    testSwapBytes(static_cast< uint8_t >(0x0), static_cast< uint8_t >(0x0));
    testSwapBytes(static_cast< uint8_t >(0xA), static_cast< uint8_t >(0xA));
    testSwapBytes(static_cast< uint8_t >(0xFF), static_cast< uint8_t >(0xFF));
    testSwapBytes(static_cast< uint8_t >(0xAF), static_cast< uint8_t >(0xAF));
    testSwapBytes(static_cast< uint8_t >(0xAC), static_cast< uint8_t >(0xAC));

    // uint16
    testSwapBytes(static_cast< uint16_t >(0x0), static_cast< uint16_t >(0x0));
    testSwapBytes(static_cast< uint16_t >(0xA00), static_cast< uint16_t >(0xA));
    testSwapBytes(static_cast< uint16_t >(0xA), static_cast< uint16_t >(0xA00));
    testSwapBytes(static_cast< uint16_t >(0xFFFF),
        static_cast< uint16_t >(0xFFFF));
    testSwapBytes(static_cast< uint16_t >(0xAEC2),
        static_cast< uint16_t >(0xC2AE));
    testSwapBytes(static_cast< uint16_t >(0x17F1),
        static_cast< uint16_t >(0xF117));

    // uint24
    testSwapBytes(static_cast< uint32_t >(0x0),
        static_cast< uint32_t >(0x0), 3U);
    testSwapBytes(static_cast< uint32_t >(0xA0000),
        static_cast< uint32_t >(0xA), 3U);
    testSwapBytes(static_cast< uint32_t >(0x200),
        static_cast< uint32_t >(0x200), 3U);
    testSwapBytes(static_cast< uint32_t >(0xA),
        static_cast< uint32_t >(0xA0000), 3U);
    testSwapBytes(static_cast< uint32_t >(0xFFFFFF),
        static_cast< uint32_t >(0xFFFFFF), 3U);
    testSwapBytes(static_cast< uint32_t >(0xC3F1A0),
        static_cast< uint32_t >(0xA0F1C3), 3U);
    testSwapBytes(static_cast< uint32_t >(0xFFA2B1),
        static_cast< uint32_t >(0xB1A2FF), 3U);
    testSwapBytes(static_cast< uint32_t >(0xC4D1BB),
        static_cast< uint32_t >(0xBBD1C4), 3U);
    testSwapBytes(static_cast< uint32_t >(0x070001),
        static_cast< uint32_t >(0x010007), 3U);

    // uint32
    testSwapBytes(static_cast< uint32_t >(0x0), static_cast< uint32_t >(0x0));
    testSwapBytes(static_cast< uint32_t >(0xA000000),
        static_cast< uint32_t >(0xA));
    testSwapBytes(static_cast< uint32_t >(0x20000),
        static_cast< uint32_t >(0x200));
    testSwapBytes(static_cast< uint32_t >(0x200),
        static_cast< uint32_t >(0x20000));
    testSwapBytes(static_cast< uint32_t >(0xA),
        static_cast< uint32_t >(0xA000000));
    testSwapBytes(static_cast< uint32_t >(0xFFFFFFFF),
        static_cast< uint32_t >(0xFFFFFFFF));
    testSwapBytes(static_cast< uint32_t >(0xB9C2A117),
        static_cast< uint32_t >(0x17A1C2B9));
    testSwapBytes(static_cast< uint32_t >(0x011751A0),
        static_cast< uint32_t >(0xA0511701));
    testSwapBytes(static_cast< uint32_t >(0xE5A3B1B0),
        static_cast< uint32_t >(0xB0B1A3E5));
    testSwapBytes(static_cast< uint32_t >(0x5C3AA4AA),
        static_cast< uint32_t >(0xAAA43A5C));

    // uint48
    testSwapBytes(static_cast< uint64_t >(0x0ULL),
        static_cast< uint64_t >(0x0ULL), 6U);
    testSwapBytes(static_cast< uint64_t >(0xA0000000000ULL),
        static_cast< uint64_t >(0xAULL), 6U);
    testSwapBytes(static_cast< uint64_t >(0x200000000ULL),
        static_cast< uint64_t >(0x200ULL), 6U);
    testSwapBytes(static_cast< uint64_t >(0x4000000ULL),
        static_cast< uint64_t >(0x40000ULL), 6U);
    testSwapBytes(static_cast< uint64_t >(0x40000ULL),
        static_cast< uint64_t >(0x4000000ULL), 6U);
    testSwapBytes(static_cast< uint64_t >(0x200ULL),
        static_cast< uint64_t >(0x200000000ULL), 6U);
    testSwapBytes(static_cast< uint64_t >(0xAULL),
        static_cast< uint64_t >(0xA0000000000ULL), 6U);
    testSwapBytes(static_cast< uint64_t >(0xFFFFFFFFFFFFULL),
        static_cast< uint64_t >(0xFFFFFFFFFFFFULL), 6U);
    testSwapBytes(static_cast< uint64_t >(0x0C5BA7F99C01ULL),
        static_cast< uint64_t >(0x019CF9A75B0CULL), 6U);
    testSwapBytes(static_cast< uint64_t >(0xB4778A9BF1C0ULL),
        static_cast< uint64_t >(0xC0F19B8A77B4ULL), 6U);
    testSwapBytes(static_cast< uint64_t >(0x57788ABCD1D5ULL),
        static_cast< uint64_t >(0xD5D1BC8A7857ULL), 6U);
    testSwapBytes(static_cast< uint64_t >(0xB1F1565A9CD1ULL),
        static_cast< uint64_t >(0xD19C5A56F1B1ULL), 6U);

    // uint64
    testSwapBytes(static_cast< uint64_t >(0x0ULL),
        static_cast< uint64_t >(0x0ULL));
    testSwapBytes(static_cast< uint64_t >(0xA00000000000000ULL),
        static_cast< uint64_t >(0xAULL));
    testSwapBytes(static_cast< uint64_t >(0x2000000000000ULL),
        static_cast< uint64_t >(0x200ULL));
    testSwapBytes(static_cast< uint64_t >(0x40000000000ULL),
        static_cast< uint64_t >(0x40000ULL));
    testSwapBytes(static_cast< uint64_t >(0x600000000ULL),
        static_cast< uint64_t >(0x6000000ULL));
    testSwapBytes(static_cast< uint64_t >(0x6000000ULL),
        static_cast< uint64_t >(0x600000000ULL));
    testSwapBytes(static_cast< uint64_t >(0x40000ULL),
        static_cast< uint64_t >(0x40000000000ULL));
    testSwapBytes(static_cast< uint64_t >(0x200ULL),
        static_cast< uint64_t >(0x2000000000000ULL));
    testSwapBytes(static_cast< uint64_t >(0xAULL),
        static_cast< uint64_t >(0xA00000000000000ULL));
    testSwapBytes(static_cast< uint64_t >(0xFFFFFFFFFFFFFFFFULL),
        static_cast< uint64_t >(0xFFFFFFFFFFFFFFFFULL));
    testSwapBytes(static_cast< uint64_t >(0xCD6719D9741821C4ULL),
        static_cast< uint64_t >(0xC4211874D91967CDULL));
    testSwapBytes(static_cast< uint64_t >(0x009871000AB57C01ULL),
        static_cast< uint64_t >(0x017CB50A00719800ULL));
    testSwapBytes(static_cast< uint64_t >(0xBA7C9AA7AABFF5A4ULL),
        static_cast< uint64_t >(0xA4F5BfAAA79A7CBAULL));
    testSwapBytes(static_cast< uint64_t >(0xF7C205D100BCB777ULL),
        static_cast< uint64_t >(0x77B7BC00D105C2F7ULL));
}

void UtilsTestCase::test_worldDataArrayToCorbaSeq()
{
    std::vector< unsigned char > ret;
    Control::LongSeq_var x;

    x = AMB::Utils::worldDataArrayToCorbaSeq< std::vector< unsigned char >,
        CORBA::Long, Control::LongSeq, Control::LongSeq_var >(ret);

    ret.push_back(3);
    x = AMB::Utils::worldDataArrayToCorbaSeq< std::vector< unsigned char >,
        CORBA::Long, Control::LongSeq, Control::LongSeq_var >(ret);
    CPPUNIT_ASSERT_EQUAL(3,x[0]);

    ret.push_back(2);
    ret.push_back(5);
    x = AMB::Utils::worldDataArrayToCorbaSeq< std::vector< unsigned char >,
        CORBA::Long, Control::LongSeq, Control::LongSeq_var >(ret);
    CPPUNIT_ASSERT_EQUAL(3, x[0]);
    CPPUNIT_ASSERT_EQUAL(2, x[1]);
    CPPUNIT_ASSERT_EQUAL(5, x[2]);

    ret.push_back(0);
    ret.push_back(12);
    ret.push_back(255);
    x = AMB::Utils::worldDataArrayToCorbaSeq< std::vector< unsigned char >,
        CORBA::Long, Control::LongSeq, Control::LongSeq_var >(ret);
    CPPUNIT_ASSERT_EQUAL(3, x[0]);
    CPPUNIT_ASSERT_EQUAL(2, x[1]);
    CPPUNIT_ASSERT_EQUAL(5, x[2]);
    CPPUNIT_ASSERT_EQUAL(0, x[3]);
    CPPUNIT_ASSERT_EQUAL(12, x[4]);
    CPPUNIT_ASSERT_EQUAL(255, x[5]);
}

void UtilsTestCase::test_worldToRaw()
{
    std::vector< float > raw;
    std::vector< float > world;

    // No element test.
    raw = AMB::Utils::worldToRaw< std::vector< float >, std::vector< float > >(
        world, 0, 1);

    // One element test.
    world.push_back(3.0f);
    CPPUNIT_ASSERT_EQUAL(3.0f, ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 0.0f, 1.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(1.5f, ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 0.0f, 2.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(1.0f, ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 0.0f, 3.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(2.0f, ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 1.0f, 1.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(1.0f, ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 2.0f, 1.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(((3.0f - 2.0f) / 0.4f), ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 2.0f, 0.4f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(((3.0f - 5.0f) / 3.0f), ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 5.0f, 3.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(((3.0f - 3.1f) / 0.1f), ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 3.1f, 0.1f)).at(0)));

    // Multiple element test.
    world.push_back(17.2f);
    CPPUNIT_ASSERT_EQUAL(3.0f, ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 0.0f, 1.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(1.5f, ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 0.0f, 2.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(1.0f, ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 0.0f, 3.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(2.0f, ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 1.0f, 1.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(1.0f, ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 2.0f, 1.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(((3.0f - 2.0f) / 0.4f), ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 2.0f, 0.4f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(((3.0f - 5.0f) / 3.0f), ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 5.0f, 3.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(((3.0f - 3.1f) / 0.1f), ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 3.1f, 0.1f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(17.2f, ((AMB::Utils::worldToRaw< std::vector< float>,
        std::vector< float > >(world, 0.0f, 1.0f)).at(1)));
    CPPUNIT_ASSERT_EQUAL((17.2f / 2.0f), ((AMB::Utils::worldToRaw< std::vector< float>,
        std::vector< float > >(world, 0.0f, 2.0f)).at(1)));
    CPPUNIT_ASSERT_EQUAL((17.2f / 3.0f), ((AMB::Utils::worldToRaw< std::vector< float>,
        std::vector< float > >(world, 0.0f, 3.0f)).at(1)));
    CPPUNIT_ASSERT_EQUAL(((17.2f - 1.0f) / 1.0f), ((AMB::Utils::worldToRaw< std::vector< float>,
        std::vector< float > >(world, 1.0f, 1.0f)).at(1)));
    CPPUNIT_ASSERT_EQUAL(((17.2f - 2.0f) / 1.0f) , ((AMB::Utils::worldToRaw< std::vector< float>,
        std::vector< float > >(world, 2.0f, 1.0f)).at(1)));
    CPPUNIT_ASSERT_EQUAL(((17.2f - 2.0f) / 4.0f), ((AMB::Utils::worldToRaw< std::vector< float>,
        std::vector< float > >(world, 2.0f, 4.0f)).at(1)));
    CPPUNIT_ASSERT_EQUAL(((17.2f - 5.0f) / 3.0f), ((AMB::Utils::worldToRaw< std::vector< float>,
        std::vector< float > >(world, 5.0f, 3.0f)).at(1)));
    CPPUNIT_ASSERT_EQUAL(((17.2f - 3.0f) / 0.1f), ((AMB::Utils::worldToRaw< std::vector< float>,
        std::vector< float > >(world, 3.0f, 0.1f)).at(1)));

    world.push_back(-21.5f);
    CPPUNIT_ASSERT_EQUAL(3.0f, ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 0.0f, 1.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(1.5f, ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 0.0f, 2.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(1.0f, ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 0.0f, 3.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(2.0f, ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 1.0f, 1.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(1.0f, ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 2.0f, 1.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(((3.0f - 2.0f) / 0.4f), ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 2.0f, 0.4f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(((3.0f - 5.0f) / 3.0f), ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 5.0f, 3.0f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(((3.0f - 3.1f) / 0.1f), ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 3.1f, 0.1f)).at(0)));
    CPPUNIT_ASSERT_EQUAL(17.2f, ((AMB::Utils::worldToRaw< std::vector< float>,
        std::vector< float > >(world, 0.0f, 1.0f)).at(1)));
    CPPUNIT_ASSERT_EQUAL((17.2f / 2.0f), ((AMB::Utils::worldToRaw< std::vector< float>,
        std::vector< float > >(world, 0.0f, 2.0f)).at(1)));
    CPPUNIT_ASSERT_EQUAL((17.2f / 3.0f), ((AMB::Utils::worldToRaw< std::vector< float>,
        std::vector< float > >(world, 0.0f, 3.0f)).at(1)));
    CPPUNIT_ASSERT_EQUAL(((17.2f - 1.0f) / 1.0f), ((AMB::Utils::worldToRaw< std::vector< float>,
        std::vector< float > >(world, 1.0f, 1.0f)).at(1)));
    CPPUNIT_ASSERT_EQUAL(((17.2f - 2.0f) / 1.0f) , ((AMB::Utils::worldToRaw< std::vector< float>,
        std::vector< float > >(world, 2.0f, 1.0f)).at(1)));
    CPPUNIT_ASSERT_EQUAL(((17.2f - 2.0f) / 4.0f), ((AMB::Utils::worldToRaw< std::vector< float>,
        std::vector< float > >(world, 2.0f, 4.0f)).at(1)));
    CPPUNIT_ASSERT_EQUAL(((17.2f - 5.0f) / 3.0f), ((AMB::Utils::worldToRaw< std::vector< float>,
        std::vector< float > >(world, 5.0f, 3.0f)).at(1)));
    CPPUNIT_ASSERT_EQUAL(((17.2f - 3.0f) / 0.1f), ((AMB::Utils::worldToRaw< std::vector< float>,
        std::vector< float > >(world, 3.0f, 0.1f)).at(1)));
    CPPUNIT_ASSERT_EQUAL(-21.5f, ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 0.0f, 1.0f)).at(2)));
    CPPUNIT_ASSERT_EQUAL((-21.5f / 2.0f), ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 0.0f, 2.0f)).at(2)));
    CPPUNIT_ASSERT_EQUAL((-21.5f / 3.0f), ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 0.0f, 3.0f)).at(2)));
    CPPUNIT_ASSERT_EQUAL(((-21.5f - 1.0f) / 1.0f), ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 1.0f, 1.0f)).at(2)));
    CPPUNIT_ASSERT_EQUAL(((-21.5f - 2.0f) / 1.0f), ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 2.0f, 1.0f)).at(2)));
    CPPUNIT_ASSERT_EQUAL(((-21.5f - 2.0f) / 4.0f), ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 2.0f, 4.0f)).at(2)));
    CPPUNIT_ASSERT_EQUAL(((-21.5f - 5.0f) / 3.0f), ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 5.0f, 3.0f)).at(2)));
    CPPUNIT_ASSERT_EQUAL(((-21.5f - 3.0f) / 0.1f), ((AMB::Utils::worldToRaw< std::vector< float >,
        std::vector< float > >(world, 3.0f, 0.1f)).at(2)));

    // Checking for scale 0
    raw = AMB::Utils::worldToRaw< std::vector< float >, std::vector< float > >(
        world, 0, 0);
    CPPUNIT_ASSERT_EQUAL(std::numeric_limits< float >::infinity(), raw.at(0));
    CPPUNIT_ASSERT_EQUAL(std::numeric_limits< float >::infinity(), raw.at(1));
    CPPUNIT_ASSERT_EQUAL(-std::numeric_limits< float >::infinity(), raw.at(2));
}


void UtilsTestCase::test_rawToWorld()
{
    std::vector< float > raw;
    std::vector< float > world;

    // No element test.
    world = AMB::Utils::rawToWorld< std::vector< float >,
        std::vector< float > >(raw, 0, 1);

    // One element test.
    raw.push_back(3.0f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 1)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 2)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 3)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 1, 1)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 2, 1)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(14.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 2, 4)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(14.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 5, 3)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.3f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 3, 0.1)).at(0)), 0.00001f);

    // Multiple element test.
    raw.push_back(17.2f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 1)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 2)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 3)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 1, 1)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 2, 1)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(14.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 2, 4)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(14.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 5, 3)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.3f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 3, 0.1)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(17.2f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 1)).at(1)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(34.4f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 2)).at(1)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(51.6f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 3)).at(1)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(18.2f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 1, 1)).at(1)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(19.2f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 2, 1)).at(1)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(70.8f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 2, 4)).at(1)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(56.6f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 5, 3)).at(1)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.72f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 3, 0.1)).at(1)), 0.00001f);

    raw.push_back(-21.5f);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 1)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 2)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 3)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 1, 1)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 2, 1)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(14.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 2, 4)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(14.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 5, 3)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.3f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 3, 0.1)).at(0)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(17.2f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 1)).at(1)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(34.4f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 2)).at(1)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(51.6f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 3)).at(1)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(18.2f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 1, 1)).at(1)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(19.2f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 2, 1)).at(1)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(70.8f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 2, 4)).at(1)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(56.6f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 5, 3)).at(1)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.72f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 3, 0.1)).at(1)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-21.5f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 1)).at(2)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-64.5f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 3)).at(2)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-20.5f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 1, 1)).at(2)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-19.5f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 2, 1)).at(2)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-84.0f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 2, 4)).at(2)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-59.5f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 5, 3)).at(2)), 0.00001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.85f, ((
        AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 3, 0.1)).at(2)), 0.00001f);

    // Checking for scale 0
    world = AMB::Utils::rawToWorld< std::vector< float >, std::vector< float > >(
            raw, 0, 0);
    CPPUNIT_ASSERT_EQUAL(0.0f, world.at(0));
    CPPUNIT_ASSERT_EQUAL(0.0f, world.at(1));
    CPPUNIT_ASSERT_EQUAL(0.0f, world.at(2));
}


void UtilsTestCase::test_RODevIO()
{
    {
        // Testing Simple case.
        TestRODevIOBase< float, CORBA::Float > x;
        float y(0.0);
        CORBA::Float z(0.0);

        x.setVal(y);
        z = x.read();

        y = 12.2;
        x.setVal(y);
        z = x.read();
        CPPUNIT_ASSERT_EQUAL(CORBA::Float(12.2), z);

        y = 200.02;
        x.setVal(y);
        z = x.read();
        CPPUNIT_ASSERT_EQUAL(CORBA::Float(200.02), z);

        y = -10.02;
        x.setVal(y);
        z = x.read();
        CPPUNIT_ASSERT_EQUAL(CORBA::Float(-10.02), z);
    }

    {
        // Testing Vector case.
        TestRODevIOBase< std::vector< unsigned char >, ACS::longSeq > x;
        std::vector< unsigned char > y;
        ACS::longSeq z;

        x.setVal(y);
        z = x.read();

        y.push_back(12);
        x.setVal(y);
        z = x.read();
        CPPUNIT_ASSERT_EQUAL(12, z[0]);

        y.push_back(255);
        x.setVal(y);
        z = x.read();
        CPPUNIT_ASSERT_EQUAL(12, z[0]);
        CPPUNIT_ASSERT_EQUAL(255, z[1]);

        y.push_back(0);
        x.setVal(y);
        z = x.read();
        CPPUNIT_ASSERT_EQUAL(12, z[0]);
        CPPUNIT_ASSERT_EQUAL(255, z[1]);
        CPPUNIT_ASSERT_EQUAL(0, z[2]);
    }

    {
        // Testing Simple Pattern case.
        TestRODevIOBase< int64_t, ACS::pattern > x;
        int64_t y(0);
        ACS::pattern z(0ULL);

        x.setVal(y);
        z = x.read();

        y = 12;
        x.setVal(y);
        z = x.read();
        //CPPUNIT_ASSERT_EQUAL(CORBA::Float(200.02), z);

        y = -2000;
        x.setVal(y);
        z = x.read();

        y = 0;
        x.setVal(y);
        z = x.read();
    }

    {
        // Testing Vector to Pattern case.
        TestRODevIOBase< std::vector< char >, ACS::pattern > x;
        std::vector< char > y;
        ACS::pattern z(0);

        x.setVal(y);
        z = x.read();

        y.push_back(12);
        x.setVal(y);
        z = x.read();
        //CPPUNIT_ASSERT_EQUAL(12, z[0]);

        y.push_back( -127);
        x.setVal(y);
        z = x.read();

        y.push_back(128);
        x.setVal(y);
        z = x.read();
    }
}


void UtilsTestCase::test_Cache()
{
    {
        // Important to notice no changes when going from A to B to A.
        // But there are when going to from A to A or B to B.
        CacheAssemblyBase x(1.0);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        CPPUNIT_ASSERT_EQUAL(1, x.getA());
        CPPUNIT_ASSERT_EQUAL(1.2, x.getB());
        CPPUNIT_ASSERT_EQUAL(1, x.getA());
        CPPUNIT_ASSERT_EQUAL(2, x.getA());
        CPPUNIT_ASSERT_EQUAL(3, x.getA());
    }

    {
        // If time (sleep 2) > duration (1) it should change when querying B.
        CacheAssemblyBase x(1.0);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        CPPUNIT_ASSERT_EQUAL(1, x.getA());
        const ACE_Time_Value t(2, 0);
        // This makes an additional change in the subsequent calls.
        ACE_OS::select(0, 0, 0, 0, t);
        CPPUNIT_ASSERT_EQUAL(2.4, x.getB());
        CPPUNIT_ASSERT_EQUAL(2, x.getA());
        CPPUNIT_ASSERT_EQUAL(3, x.getA());
        CPPUNIT_ASSERT_EQUAL(4, x.getA());
    }
}

/**
 * Main function running the tests
 */

int main(int a __attribute__((unused)), char* v[] __attribute__((unused)))
{
    // Create the event manager and test controller
    CPPUNIT_NS::TestResult controller;
    // Add a listener that colllects test result
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener(&result);
    // Add a listener that print dots as test run.
    CPPUNIT_NS::BriefTestProgressListener progress;
    controller.addListener(&progress);
    // Add the top suite to the test runner
    CPPUNIT_NS::TestRunner runner;
    runner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
    runner.run(controller);
    // Print test in a compiler compatible format.
    CPPUNIT_NS::CompilerOutputter outputter(&result, std::cerr);
    outputter.write();

    return (result.wasSuccessful() ? 0 : 1);
}
