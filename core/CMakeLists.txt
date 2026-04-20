cmake_minimum_required(VERSION 3.10)
project(LINA_Core)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(JSONCPP jsoncpp)
endif()

if(NOT JSONCPP_FOUND)
    find_package(jsoncpp CONFIG)
endif()

if(NOT JSONCPP_FOUND)
    message(FATAL_ERROR "jsoncpp not found. Install with: sudo apt-get install libjsoncpp-dev")
endif()

# Core library
add_library(lina_core STATIC
    src/network_collector.cpp
    src/network_collector.h
    src/data_normalizer.cpp
    src/data_normalizer.h
    src/ai_engine.cpp
    src/ai_engine.h
    src/language_manager.cpp
    src/language_manager.h
)

target_include_directories(lina_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${JSONCPP_INCLUDE_DIRS}
)

target_link_libraries(lina_core PUBLIC
    ${JSONCPP_LIBRARIES}
)

if(JSONCPP_COMPILE_FLAGS)
    target_compile_options(lina_core PUBLIC ${JSONCPP_COMPILE_FLAGS})
endif()

# Platform-specific linking
if(WIN32)
    target_link_libraries(lina_core PUBLIC
        ws2_32
        iphlpapi
    )
elseif(APPLE)
    target_link_libraries(lina_core PUBLIC
        "-framework Foundation"
        "-framework NetworkExtension"
    )
else()
    # Linux
    target_link_libraries(lina_core PUBLIC
        pthread
    )
endif()

# Export library
set(LINA_CORE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src PARENT_SCOPE)
set(LINA_CORE_LIBRARY lina_core PARENT_SCOPE)
