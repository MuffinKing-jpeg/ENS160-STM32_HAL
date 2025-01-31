cmake_minimum_required(VERSION 3.22)

project(ens160)
add_library(ens160 INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)


message("ENS160 driver by:
Andrii Dobrus
Git: https://github.com/MuffinKing-jpeg")

file(GLOB ENS160_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}//Src/*.c)

target_include_directories(ens160 INTERFACE
${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

target_sources(ens160 PUBLIC
    ${ENS160_SRC_FILES}
    # Add user sources here
)

target_link_directories(ens160 INTERFACE
)

target_link_libraries(ens160 INTERFACE
)

# Validate that STM32CubeMX code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()