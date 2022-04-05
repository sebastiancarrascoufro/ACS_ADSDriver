#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>


#include <chrono>
#include <thread>

#include <test.h>

int main(int argc, char* argv[])
{
	
	CPPUNIT_NS::Test *suite = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();

	CppUnit::TextUi::TestRunner runner;
	runner.addTest(suite);
	bool exito = runner.run();

	delete interface_mp;
	
	return exito ? 0 : 1;
}



