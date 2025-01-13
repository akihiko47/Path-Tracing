cmake_minimum_required ( VERSION 3.31 )

project ( PathTracing LANGUAGES CXX )

set ( CMAKE_CXX_STANDARD          17 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )
set ( CMAKE_CXX_EXTENSIONS        OFF )


add_executable(PathTracing src/main.cpp)

target_include_directories(PathTracing PRIVATE ${PROJECT_SOURCE_DIR}/dependencies)