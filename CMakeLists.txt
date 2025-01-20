cmake_minimum_required ( VERSION 3.20 )

project ( PathTracing LANGUAGES CXX )

set ( CMAKE_CXX_STANDARD          17 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )
set ( CMAKE_CXX_EXTENSIONS        OFF )


# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()


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
	src/texture.hpp
)


add_executable(PathTracing ${SOURCES})

target_include_directories(PathTracing PRIVATE ${PROJECT_SOURCE_DIR}/dependencies ${PROJECT_SOURCE_DIR}/src)

target_compile_definitions(PathTracing PRIVATE 
	OUTPUT_DIR="${PROJECT_SOURCE_DIR}/output" 
	TEXTURE_DIR="${PROJECT_SOURCE_DIR}/textures" 
	SCENE_DIR="${PROJECT_SOURCE_DIR}/scenes"
)


include(FetchContent)

FetchContent_Declare(
	yaml-cpp
	GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
	GIT_TAG yaml-cpp-0.7.0
)

set(YAML_CPP_BUILD_CONTRIB OFF CACHE INTERNAL "Enable yaml-cpp contrib in library" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE INTERNAL "Enable parse tools" FORCE)
set(BUILD_TESTING OFF CACHE INTERNAL "Build the testing tree." FORCE)

FetchContent_MakeAvailable(yaml-cpp)


if(NOT yaml-cpp_POPULATED)
  message(STATUS "Fetching yaml-cpp...")
  FetchContent_Populate(yaml-cpp)
  add_subdirectory(${yaml-cpp_SOURCE_DIR} ${yaml-cpp_BINARY_DIR})
endif()

target_link_libraries(PathTracing PUBLIC yaml-cpp::yaml-cpp)


if(MSVC)
 target_compile_options(PathTracing PRIVATE "/MP")
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT PathTracing)