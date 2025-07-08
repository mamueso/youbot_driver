#include "YouBotBaseKinematicsTestEndurance.hpp"
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

CPPUNIT_TEST_SUITE_REGISTRATION( YouBotBaseKinematicsTestEndurance );

int main(int argc, char* argv[]) {
  std::cout << "Attention! All wheels of the youBot will move during the test. \nThe youBot should NOT stand on the ground and the wheels should be in the air! \nAlso the arm will move please be carefull!" << std::endl;
  char input = 0;

  while (input != 'y' && input != 'n') {
    std::cout << "Are all wheels off the ground? [n/y]" << std::endl;
    input = getchar();
    if (input == 'n') {
      return 0;
    }
  }
  
  int ret = scanf("%d", &g_no_of_cycles);
  if(ret != 1 || g_no_of_cycles < 1) {
    std::cout << "Invalid number of cycles, using default value of 1." << std::endl;
    g_no_of_cycles = 1;
  }

  Logger::logginLevel = trace;
  
  CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();
  CppUnit::TextUi::TestRunner runner;
  runner.addTest( suite );
  runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(), std::cerr ) );
  /** let the test run */
  bool wasSucessful = runner.run();
  
  /** check whether it was sucessfull or not */
  return wasSucessful ? 0 : 1;
}


