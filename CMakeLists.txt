cmake_minimum_required(VERSION 2.6.2)
project(AITD)
add_definitions(-DUNIX -DFORBIDDEN_SYMBOL_ALLOW_ALL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -pthread -march=native")
find_package(SDL REQUIRED)

option(BUILD_TESTS "build engine unit tests" ON)

set(CMAKE_MODULE_PATH
  "${CMAKE_SOURCE_DIR}/CMake")

#################### UNIT TESTING ####################

if (BUILD_TESTS)
include(CMake/DownloadProject.cmake)
download_project(PROJ                googletest
                 GIT_REPOSITORY      https://github.com/google/googletest.git
                 GIT_TAG             master
		 )

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

function(cxx_test name sources)
    add_executable(${name} ${sources})
    target_link_libraries(${name} ${ARGN} gtest gmock_main ${CMAKE_THREAD_LIBS_INIT})
    set_property(TARGET ${name} APPEND PROPERTY INCLUDE_DIRECTORIES
        ${GTEST_INCLUDE_DIRS}
        )

    # add gtest as a dependency if we are building it ourselves
    if(TARGET gtest)
        add_dependencies(${name} gtest)
    endif()
    # Working directory: where the dlls are installed.
    add_test(NAME ${name}
        COMMAND ${name})
    # For debugging, one can manually run the tests with the option
    # --gtest_break_on_failure which will raise a segfault on the first error
endfunction()
endif()

add_subdirectory(aitd)
add_subdirectory(src)

