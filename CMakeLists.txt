cmake_minimum_required(VERSION 3.16)

# Set the project name
project(cppwebs)

# Set C++ standard and make it a requirement
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Check for GCC version 9 and add -fconcepts flag
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 9 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 9)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts")
    endif()
endif()

# Specify the header files. Include both .h and .hpp files
file(GLOB_RECURSE HEADER_FILES 
     ${CMAKE_SOURCE_DIR}/include/*.h
     ${CMAKE_SOURCE_DIR}/include/*.hpp)

# Specify the source files. Assuming they are in a 'src' directory
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*.c)

# Create the library
add_library(cppwebs::cppwebs STATIC ${SOURCE_FILES} ${HEADER_FILES})

# Specify include directories for the library and anyone linking against it
target_include_directories(cppwebs PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Add an executable for the sample
add_executable(simple_example samples/simple.cpp)

# Link the executable to your library
target_link_libraries(simple_example cppwebs)
