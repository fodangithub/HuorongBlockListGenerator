cmake_minimum_required(VERSION 3.15)

# Set vcpkg toolchain file if not already set
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
    endif()
endif()

message(STATUS "use vcpkg toolchain at: ${CMAKE_TOOLCHAIN_FILE}")

set(ENV{HTTP_PROXY} "http://127.0.0.1:10808")
set(ENV{HTTPS_PROXY} "http://127.0.0.1:10808")

project(HuorongBlockingListGenerator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json CONFIG REQUIRED)

add_executable(HRBlockListGenerator HuorongBlockingListGenerator.cpp)

target_link_libraries(HRBlockListGenerator PRIVATE nlohmann_json::nlohmann_json)