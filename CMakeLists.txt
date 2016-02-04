cmake_minimum_required(VERSION 3.3)
project(QBL-Pilot-STD)

enable_language(C)
enable_language(ASM)


include(CortexM.CMake)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build)

set(cortex_tool_root "D:\\Develop\\GCC5_ARM\\")

cortex_set_tool_chain(
        ${cortex_tool_root}
        "arm-none-eabi"
        "${CMAKE_SOURCE_DIR}/GCC/STM32F407VG_FLASH.ld"
)

cortex_enable_fpu()

add_definitions("-DUSE_STDPERIPH_DRIVER" "-DSTM32F40_41xxx")

include_directories(STD)
include_directories(STD/inc/)
include_directories(CMSIS/Include)
include_directories(QBL)
include_directories(Middleware)
include_directories(Driver)

aux_source_directory(User SourceLists)
aux_source_directory(STD SourceLists)
aux_source_directory(STD/src SourceLists)
aux_source_directory(QBL SourceLists)
aux_source_directory(Middleware SourceLists)
aux_source_directory(Driver SourceLists)

set(SourceLists
        ${SourceLists}
        GCC/syscalls.c
        GCC/startup_stm32f40_41xxx.s)

add_executable(${CMAKE_PROJECT_NAME} ${SourceLists})
cortex_build_info()
cortex_build_bin()

