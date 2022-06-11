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

	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	delete interface_mp;
	
	return exito ? 0 : 1;
}



