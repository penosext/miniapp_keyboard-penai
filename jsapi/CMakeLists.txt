cmake_minimum_required(VERSION 3.5)
project(jsapi_langningchen C CXX)
set(LIB_NAME jsapi_langningchen)

set(CROSS_TOOLCHAIN_PREFIX "${CMAKE_SOURCE_DIR}/toolchain/arm-buildroot-linux-gnueabihf/bin/arm-buildroot-linux-gnueabihf-")
set(CMAKE_C_COMPILER "${CROSS_TOOLCHAIN_PREFIX}gcc")
set(CMAKE_CXX_COMPILER "${CROSS_TOOLCHAIN_PREFIX}g++")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

set(IOT_UI_SDK_PATH "${CMAKE_SOURCE_DIR}/iot-miniapp-sdk")
include_directories(${IOT_UI_SDK_PATH}/include)
link_directories(${IOT_UI_SDK_PATH}/lib)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror=return-type)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0)
    add_compile_options(-Wformat -Wformat-security -fstack-protector --param ssp-buffer-size=4)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-Os)
else()
    message(FATAL_ERROR "CMAKE_BUILD_TYPE should be: Debug/Release")
endif()

file(GLOB_RECURSE LIB_SOURCES iot-miniapp-sdk/src/*.cpp)
add_library(iot_sdk_lib STATIC ${LIB_SOURCES})
target_include_directories(iot_sdk_lib PUBLIC ${IOT_UI_SDK_PATH}/include)
set_target_properties(iot_sdk_lib PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_options(iot_sdk_lib PRIVATE -w -Wno-psabi)

file(GLOB_RECURSE SOURCES src/*.c src/*.cpp)
add_library(${LIB_NAME} SHARED ${SOURCES})
target_include_directories(${LIB_NAME} PRIVATE src)
target_link_libraries(${LIB_NAME} PRIVATE iot_sdk_lib -Wl,-unresolved-symbols=ignore-all)

install(TARGETS ${LIB_NAME} LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/libs)
