include_directories(include)

add_library(DAQ DAQControl.cpp)

target_sources(DAQ INTERFACE ${CMAKE_SOURCE_DIR}/src/DAQ/DAQControl.cpp)