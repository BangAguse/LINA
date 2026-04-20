cmake_minimum_required(VERSION 3.10)
project(LINA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectories
add_subdirectory(core)
add_subdirectory(desktop)

# Print build information
message(STATUS "LINA Build Configuration:")
message(STATUS "  - Core Engine: ✓")
message(STATUS "  - Desktop Application (Qt): ✓")
