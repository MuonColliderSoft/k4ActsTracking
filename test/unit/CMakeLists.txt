#[[
Copyright (c) 2014-2024 Key4hep-Project.

This file is part of Key4hep.
See https://key4hep.github.io/key4hep-doc/ for further info.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
]]

# Find GTest package
find_package(GTest QUIET)

if(NOT GTest_FOUND)
  message(STATUS "Google Test not found, unit tests will be skipped")
  return()
endif()

# Set up the Acts library target variable
set(ACTS_LIBRARY_TARGETS "Acts::Core")
if (Acts_VERSION VERSION_LESS 43.0.0)
  set(ACTS_LIBRARY_TARGETS "ActsCore")
endif()

# Test for ActsGaudiLogger
add_executable(test_ActsGaudiLogger test_ActsGaudiLogger.cpp)
target_link_libraries(test_ActsGaudiLogger
  PRIVATE
  k4ActsTracking
  k4FWCore::k4FWCore
  ${ACTS_LIBRARY_TARGETS}
  GTest::gtest
  GTest::gtest_main
)
target_include_directories(test_ActsGaudiLogger PRIVATE
  ${CMAKE_SOURCE_DIR}/k4ActsTracking/include
)

add_test(NAME test_ActsGaudiLogger COMMAND test_ActsGaudiLogger)
set_test_env(test_ActsGaudiLogger)

# Test for coordinate transformations and helper functions
add_executable(test_Helpers test_Helpers.cpp)
target_link_libraries(test_Helpers
  PRIVATE
  k4ActsTracking
  k4FWCore::k4FWCore
  ${ACTS_LIBRARY_TARGETS}
  GTest::gtest
  GTest::gtest_main
)
target_include_directories(test_Helpers PRIVATE
  ${CMAKE_SOURCE_DIR}/k4ActsTracking/include
)

add_test(NAME test_Helpers COMMAND test_Helpers)
set_test_env(test_Helpers)
