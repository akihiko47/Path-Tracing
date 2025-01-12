cmake_minimum_required ( VERSION 3.31 )

project ( PathTracing LANGUAGES CXX )

set ( CMAKE_CXX_STANDARD          17 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )
set ( CMAKE_CXX_EXTENSIONS        OFF )


find_package(glm COMPONENTS REQUIRED)


add_executable(PathTracing src/main.cpp)

target_link_libraries(PathTracing PRIVATE glm::glm)