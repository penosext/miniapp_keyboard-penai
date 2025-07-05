cmake_minimum_required(VERSION 3.10)
project(jsapi_langningchen C CXX)
set(LIB_NAME jsapi_langningchen)

if(TARGET_ARCH STREQUAL "x86_64")
    set(CROSS_TOOLCHAIN_PREFIX "${CMAKE_SOURCE_DIR}/toolchains/x86-64--glibc--stable-2024.05-1/bin/x86_64-buildroot-linux-gnu-")
    set(CMAKE_C_COMPILER "${CROSS_TOOLCHAIN_PREFIX}gcc")
    set(CMAKE_CXX_COMPILER "${CROSS_TOOLCHAIN_PREFIX}g++")
    set(INSTALL_DIR "libs/x86_64")
else()
    set(CROSS_TOOLCHAIN_PREFIX "${CMAKE_SOURCE_DIR}/toolchains/armv7-eabihf--glibc--stable-2018.11-1/bin/arm-buildroot-linux-gnueabihf-")
    set(CMAKE_C_COMPILER "${CROSS_TOOLCHAIN_PREFIX}gcc")
    set(CMAKE_CXX_COMPILER "${CROSS_TOOLCHAIN_PREFIX}g++")
    set(INSTALL_DIR "libs/arm")
endif()

set(IOT_UI_SDK_PATH "${CMAKE_SOURCE_DIR}/iot-miniapp-sdk")
set(CMAKE_FIND_ROOT_PATH "${CMAKE_SOURCE_DIR}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

include_directories(
    ${CMAKE_SOURCE_DIR}/
    ${CMAKE_SOURCE_DIR}/lib/curl/include
    ${CMAKE_SOURCE_DIR}/lib/openssl/include
    ${CMAKE_SOURCE_DIR}/lib/nlohmann/include
)
find_library(CURL_LIBRARY curl PATHS ${CMAKE_SOURCE_DIR}/lib/curl/lib REQUIRED)
find_library(SSL_LIBRARY ssl PATHS ${CMAKE_SOURCE_DIR}/lib/openssl/lib REQUIRED)
find_library(CRYPTO_LIBRARY crypto PATHS ${CMAKE_SOURCE_DIR}/lib/openssl/lib REQUIRED)
link_directories(${IOT_UI_SDK_PATH}/lib)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror=return-type -Wno-psabi)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-Os)
else()
    add_compile_options(-g -O0)
    add_compile_options(-Wformat -Wformat-security -fstack-protector --param ssp-buffer-size=4)
endif()

file(GLOB_RECURSE LIB_SOURCES iot-miniapp-sdk/src/*.cpp)
add_library(iot_sdk_lib STATIC ${LIB_SOURCES})
target_include_directories(iot_sdk_lib PUBLIC ${IOT_UI_SDK_PATH}/include)
set_target_properties(iot_sdk_lib PROPERTIES POSITION_INDEPENDENT_CODE ON)

set(RAWDICT_TXT ${CMAKE_SOURCE_DIR}/rawdict_utf16_65105_freq.txt)
set(RAWDICT_TXT_UTF8 ${CMAKE_BINARY_DIR}/rawdict_utf8.txt)
set(RAWDICT_HPP ${CMAKE_SOURCE_DIR}/src/IME/rawdict_data.hpp)
add_custom_command(
    OUTPUT ${RAWDICT_HPP}
    COMMAND iconv -f UTF-16 -t UTF-8 ${RAWDICT_TXT} -o ${RAWDICT_TXT_UTF8}
    COMMAND ${CMAKE_COMMAND} -E echo "// Auto-generated from rawdict_utf16_65105_freq.txt" > ${RAWDICT_HPP}
    COMMAND ${CMAKE_COMMAND} -E echo "#pragma once" >> ${RAWDICT_HPP}
    COMMAND ${CMAKE_COMMAND} -E echo "#include <string>" >> ${RAWDICT_HPP}
    COMMAND ${CMAKE_COMMAND} -E echo "static const std::string RAWDICT_DATA = R\"DICT(" >> ${RAWDICT_HPP}
    COMMAND ${CMAKE_COMMAND} -E cat ${RAWDICT_TXT_UTF8} >> ${RAWDICT_HPP}
    COMMAND ${CMAKE_COMMAND} -E echo ")DICT\";" >> ${RAWDICT_HPP}
    VERBATIM
)
add_custom_target(generate_rawdict_data_hpp DEPENDS ${RAWDICT_HPP})

file(GLOB_RECURSE SOURCES src/*.cpp src/AI/*.cpp)
add_library(${LIB_NAME} SHARED ${SOURCES})
add_dependencies(${LIB_NAME} generate_rawdict_data_hpp)
target_link_libraries(${LIB_NAME} PRIVATE
    iot_sdk_lib
    ${CURL_LIBRARY}
    ${SSL_LIBRARY}
    ${CRYPTO_LIBRARY}
    -Wl,-unresolved-symbols=ignore-all)
