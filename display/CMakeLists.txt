cmake_minimum_required(VERSION 3.25)

ADD_EXECUTABLE(displayIpAddress
  ${PROJECT_SOURCE_DIR}/display/DisplayIpAddress.cpp

)

target_link_libraries(displayIpAddress stdc++)

INSTALL(TARGETS displayIpAddress RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

ADD_EXECUTABLE(displayOutput
  ${PROJECT_SOURCE_DIR}/display/DisplayOutput.cpp

)

target_link_libraries(displayOutput stdc++)

INSTALL(TARGETS displayOutput RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

