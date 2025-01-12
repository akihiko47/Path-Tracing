cmake_minimum_required ( VERSION 3.5 )

project ( PathTracing LANGUAGES CXX )

set ( CMAKE_CXX_STANDARD          17 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )
set ( CMAKE_CXX_EXTENSIONS        OFF )

include_directories(src)

add_executable(PathTracing src/main.cpp)