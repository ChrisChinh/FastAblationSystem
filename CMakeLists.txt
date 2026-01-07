cmake_minimum_required(VERSION 3.15)

# Project configuration
project(FastAblationNew LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect sources and headers from src/
file(GLOB SRC_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)
file(GLOB SRC_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.h"
    "${CMAKE_SOURCE_DIR}/src/*.hpp"
)

add_executable(${PROJECT_NAME}
    ${SRC_SOURCES}
    ${SRC_HEADERS} # add headers for IDE visibility
)

# Include paths: include/ and src/
target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/src"
)

find_library(DAQHAT_LIB daqhats HINTS /usr/local/lib)
target_link_libraries(FastAblationNew PRIVATE ${DAQHAT_LIB})

# On Linux some DAQ backends may require pthread and dl; add them if present
if(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads dl)
endif()

# Place binaries under build/bin for convenience
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
