cmake_minimum_required(VERSION 3.14...3.18)
project(SPO2Algorithm) # Project name becomes name of the Python module

# See pybind11Config.cmake, instructions for pybind11_DIR
# According to CMake convention, find_package searches in path set in the <package_name>_DIR variable
set(pybind11_DIR "${CMAKE_CURRENT_SOURCE_DIR}\\..\\venv\\Lib\\site-packages\\pybind11\\share\\cmake\\pybind11")
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# gather all source files
file(GLOB SRC
  "C:/Users/jxie7/Documents/Arduino/libraries/DSPFilters/source/*.cpp"
)

# Add local source files
add_library(lib STATIC 
  "${CMAKE_CURRENT_SOURCE_DIR}/spo2_algorithm.cpp"
  ${SRC}
)

# Add header files to be included
target_include_directories(lib 
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
  PUBLIC "C:/Users/jxie7/Documents/Arduino/libraries/DSPFilters/include/"
)

# Add library source files (see if there is a way to do it this way)
# add_subdirectory(C:/Users/jxie7/Documents/Arduino/libraries/DSPFilters build)
# target_link_libraries(lib
#   PUBLIC DSPFilters
# )

# CMake function is a convenience helper provided by the Pybind11 project
pybind11_add_module(SPO2Algorithm bindings.cpp)

target_link_libraries(SPO2Algorithm PRIVATE lib)