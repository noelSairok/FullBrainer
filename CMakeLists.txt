cmake_minimum_required(VERSION 3.10)
project(FullBrainer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(SQLite3 REQUIRED)

# Try to find SFML with different naming conventions
find_package(SFML CONFIG QUIET)
if(NOT SFML_FOUND)
    # Fallback: use pkg-config to find SFML
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SFML REQUIRED sfml-window sfml-graphics sfml-system)
endif()

# Add the executable
add_executable(fullBrainer 
    fullBrainer.cpp
    visualizer.cpp
)

# Link libraries
target_link_libraries(fullBrainer 
    PRIVATE 
    SQLite3::SQLite3
    ${SFML_LIBRARIES}
)

target_include_directories(fullBrainer
    PRIVATE
    ${SFML_INCLUDE_DIRS}
)
