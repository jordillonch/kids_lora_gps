# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

list(APPEND EXTRA_COMPONENT_DIRS "../components")
list(APPEND EXTRA_COMPONENT_DIRS "../components/display")
list(APPEND EXTRA_COMPONENT_DIRS "../components/gps")
list(APPEND EXTRA_COMPONENT_DIRS "../components/radio")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(kids_lora_gps_tx)
