#ifndef ADSDEVIOTESTING_H_
#define ADSDEVIOTESTING_H_
 
#include <cppunit/extensions/HelperMacros.h>
#include "ambDefs.h"
#include "adsDefs.h"
#include <test.h>
#include <ControlExceptions.h>
// for ControlDeviceExceptions::HwLifecycleEx
#include <ControlDeviceExceptions.h>




class OPCUADevioTesting : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( OPCUADevioTesting );
    CPPUNIT_TEST( testInstance );
    //CPPUNIT_TEST( testClean );
    CPPUNIT_TEST( testInsertRCA );
    CPPUNIT_TEST( testSendMessage );
    CPPUNIT_TEST( testSendMessage );
    CPPUNIT_TEST( testSendMessage );
    CPPUNIT_TEST( testWrongRCAMessage );
    CPPUNIT_TEST( testSendMessageThread );
    CPPUNIT_TEST_EXCEPTION( testInsertWrongRCA, ControlExceptions::CAMBErrorExImpl );
    CPPUNIT_TEST_EXCEPTION( testInstanceremoteIpError, ControlExceptions::CAMBErrorExImpl );
    CPPUNIT_TEST_EXCEPTION( testInstancePlcPortError, ControlExceptions::CAMBErrorExImpl );
    CPPUNIT_TEST_EXCEPTION( testInstanceAMSNetIdError, ControlExceptions::CAMBErrorExImpl );
    CPPUNIT_TEST_EXCEPTION( testInstanceDeviceNameError, ControlExceptions::CAMBErrorExImpl );
    
    CPPUNIT_TEST_SUITE_END();
 
    public:

        void setUp();
        void tearDown();

        void testInstance();
	void testInsertRCA();
	void testWrongRCAMessage();
	void testInsertWrongRCA();
	void testSendMessage();
	void testSendMessageThread();
        void testClean();
	void testInstanceremoteIpError();
	void testInstanceDeviceNameError();
	void testInstancePlcPortError();
	void testInstanceAMSNetIdError();
   private:
	
    	char hostname[1024];
	
};


 
#endif // ADSDEVIOTESTING_H_
