cmake_minimum_required(VERSION 3.12)
project(link_rail_core)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Python
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Find or fetch PyBind11
find_package(pybind11 QUIET)
if(NOT pybind11_FOUND)
    message(STATUS "PyBind11 not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG        v2.11.1
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# Add core source files
set(CORE_SOURCES
    ../../core/schedule_module.cpp
    ../../core/position_engine.cpp
)

# Create Python module
pybind11_add_module(link_rail_core
    pybind11_wrapper.cpp
    ${CORE_SOURCES}
)

# Include directories
target_include_directories(link_rail_core PRIVATE
    ../../core
)

# Set output directory
set_target_properties(link_rail_core PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../python"
)

# Install target (optional)
install(TARGETS link_rail_core
    LIBRARY DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../python"
)
