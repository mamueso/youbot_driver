#include <iostream>
#include <cstdlib>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include "YouBotArmTest.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(YouBotArmTest);

int main(int argc, char* argv[]) {
    int no_of_cycles = 1;
    if (argc > 1) {
        no_of_cycles = std::atoi(argv[1]);
        if (no_of_cycles < 1) no_of_cycles = 1;
    }
    YouBotArmTest::setNoOfCycles(no_of_cycles);

    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest(registry.makeTest());
    bool wasSuccessful = runner.run("", false);
    return wasSuccessful ? 0 : 1;
}
