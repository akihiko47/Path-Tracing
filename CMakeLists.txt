cmake_minimum_required ( VERSION 3.6 )

project ( PathTracing LANGUAGES CXX )

set ( CMAKE_CXX_STANDARD          17 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )
set ( CMAKE_CXX_EXTENSIONS        OFF )

set (SOURCES
	src/main.cpp
	src/camera.hpp
	src/hittable.hpp
	src/image.hpp
	src/material.hpp
	src/ray.hpp
	src/scene.hpp
	src/timer.hpp
	src/utils.hpp
)


add_executable(PathTracing ${SOURCES})

target_include_directories(PathTracing PRIVATE ${PROJECT_SOURCE_DIR}/dependencies ${PROJECT_SOURCE_DIR}/src)

target_compile_definitions(PathTracing PRIVATE OUTPUT_DIR="${PROJECT_SOURCE_DIR}/output")


if(MSVC)
 target_compile_options(PathTracing PRIVATE "/MP")
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT PathTracing)