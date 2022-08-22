# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.5)

# Include the components directory of the main application:
#
list(APPEND EXTRA_COMPONENT_DIRS "../../components/common")
list(APPEND EXTRA_COMPONENT_DIRS "../../components/display")
list(APPEND EXTRA_COMPONENT_DIRS "../../components/gps")
list(APPEND EXTRA_COMPONENT_DIRS "../../components/radio")

# Set the components to include the tests for.
# This can be overriden from CMake cache:
# - when invoking CMake directly: cmake -D TEST_COMPONENTS="xxxxx" ..
# - when using idf.py: idf.py -T xxxxx build
#
set(TEST_COMPONENTS "gps_cbor_coders" CACHE STRING "List of components to test")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(kids_lora_gps_tx_test)
