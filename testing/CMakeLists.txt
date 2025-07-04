cmake_minimum_required(VERSION 3.25)

ADD_EXECUTABLE(base_arm_gripper_test
  ${PROJECT_SOURCE_DIR}/testing/TestSuite.cpp
  ${PROJECT_SOURCE_DIR}/testing/YouBotBaseTest.cpp
  ${PROJECT_SOURCE_DIR}/testing/YouBotBaseTestWithoutThread.cpp
  ${PROJECT_SOURCE_DIR}/testing/YouBotBaseKinematicsTest.cpp
  ${PROJECT_SOURCE_DIR}/testing/YouBotArmTest.cpp
  ${PROJECT_SOURCE_DIR}/testing/YouBotArmTestWithoutThread.cpp
  ${PROJECT_SOURCE_DIR}/testing/YouBotGripperTest.cpp
)

target_link_libraries(base_arm_gripper_test YouBotDriver cppunit m stdc++ )
INSTALL(TARGETS base_arm_gripper_test RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

ADD_EXECUTABLE(base_test
  ${PROJECT_SOURCE_DIR}/testing/TestSuiteBase.cpp
  ${PROJECT_SOURCE_DIR}/testing/YouBotBaseTest.cpp
  ${PROJECT_SOURCE_DIR}/testing/YouBotBaseTestWithoutThread.cpp
  ${PROJECT_SOURCE_DIR}/testing/YouBotBaseKinematicsTest.cpp
)
target_link_libraries(base_test YouBotDriver cppunit m stdc++ )
INSTALL(TARGETS base_test RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

ADD_EXECUTABLE(base_test_endurance
  ${PROJECT_SOURCE_DIR}/testing/TestSuiteBaseEndurance.cpp
  ${PROJECT_SOURCE_DIR}/testing/YouBotBaseKinematicsTestEndurance.cpp
)
target_link_libraries(base_test_endurance YouBotDriver cppunit m stdc++ )
INSTALL(TARGETS base_test_endurance RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

ADD_EXECUTABLE(arm_test
  ${PROJECT_SOURCE_DIR}/testing/TestSuiteArm.cpp
  ${PROJECT_SOURCE_DIR}/testing/YouBotArmTest.cpp
)
target_link_libraries(arm_test YouBotDriver cppunit m stdc++ )
INSTALL(TARGETS arm_test RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
