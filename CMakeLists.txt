cmake_minimum_required(VERSION 3.16)
project(blocksctl VERSION 0.1.0 LANGUAGES CXX)

if(NOT APPLE AND NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "This initial build supports macOS and Linux only.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(SDK "${CMAKE_CURRENT_SOURCE_DIR}/vendor/sdk")
set(JUCE_MODULES juce_core juce_events juce_audio_basics juce_audio_devices)
set(SDK_SOURCES "${SDK}/roli_blocks_basics/roli_blocks_basics.cpp")
if(APPLE)
    enable_language(OBJCXX)
    set(CMAKE_OBJCXX_STANDARD 17)
    set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)
    foreach(module IN LISTS JUCE_MODULES)
        list(APPEND SDK_SOURCES "${SDK}/${module}/${module}.mm")
    endforeach()
else()
    foreach(module IN LISTS JUCE_MODULES)
        list(APPEND SDK_SOURCES "${SDK}/${module}/${module}.cpp")
    endforeach()
endif()

add_library(blocks_sdk STATIC ${SDK_SOURCES})
target_include_directories(blocks_sdk SYSTEM PUBLIC "${SDK}")
target_compile_definitions(blocks_sdk PUBLIC
    JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
    JUCE_DONT_DECLARE_PROJECTINFO=1
    JUCE_STANDALONE_APPLICATION=1
    JUCE_MODAL_LOOPS_PERMITTED=1
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0
    $<$<CONFIG:Debug>:DEBUG=1;_DEBUG=1>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG=1>)

if(APPLE)
    foreach(framework Cocoa Foundation IOKit CoreAudio CoreMIDI AudioToolbox Accelerate)
        find_library(FRAMEWORK_${framework} ${framework} REQUIRED)
        target_link_libraries(blocks_sdk PUBLIC "${FRAMEWORK_${framework}}")
    endforeach()
    target_compile_options(blocks_sdk PRIVATE $<$<COMPILE_LANGUAGE:OBJCXX>:-fno-objc-arc>)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ALSA REQUIRED IMPORTED_TARGET alsa)
    find_package(Threads REQUIRED)
    target_compile_definitions(blocks_sdk PUBLIC LINUX=1)
    target_link_libraries(blocks_sdk PUBLIC PkgConfig::ALSA Threads::Threads ${CMAKE_DL_LIBS} rt)
endif()

add_executable(blocksctl src/main.cpp)
target_link_libraries(blocksctl PRIVATE blocks_sdk)
target_compile_definitions(blocksctl PRIVATE BLOCKSCTL_VERSION="${PROJECT_VERSION}")
target_compile_options(blocksctl PRIVATE -Wall -Wextra -Wpedantic)
if(APPLE)
    target_sources(blocksctl PRIVATE src/event_loop_mac.mm)
else()
    target_sources(blocksctl PRIVATE src/event_loop_linux.cpp)
endif()

include(CTest)
if(BUILD_TESTING)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    add_test(NAME cli COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/cli_test.py" "$<TARGET_FILE:blocksctl>")
endif()

install(TARGETS blocksctl RUNTIME DESTINATION bin)
