function(cortex_set_tool_chain path prefix linkscript)

    #设置编译器路径
    set(CMAKE_C_COMPILER "${path}/bin/${prefix}-gcc" PARENT_SCOPE)
    set(CMAKE_CXX_COMPILER "${path}/bin/${prefix}-g++" PARENT_SCOPE)
    set(CMAKE_ASM_COMPILER "${path}/bin/${prefix}-gcc" PARENT_SCOPE)
    set(CMAKE_C_LINK_EXECUTABLE "${path}/bin/${prefix}-ld" PARENT_SCOPE)
    set(CMAKE_OBJCOPY "${path}/bin/${prefix}-objcopy" PARENT_SCOPE)
    set(CMAKE_OBJDUMP "${path}/bin/${prefix}-objdump" PARENT_SCOPE)
    set(CMAKE_SIZE "${path}/bin/${prefix}-size" PARENT_SCOPE)
    #设置生成文件的后缀
    set(CMAKE_EXECUTABLE_SUFFIX ".elf" PARENT_SCOPE)

    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER PARENT_SCOPE)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY PARENT_SCOPE)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY PARENT_SCOPE)

    set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <LINK_FLAGS> -o <TARGET> <OBJECTS>" PARENT_SCOPE)
    set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <LINK_FLAGS> -o <TARGET> <OBJECTS>" PARENT_SCOPE)

    message("Target cpu arch    :${CMAKE_SYSTEM_PROCESSOR} ")
    message("Compiler's path    :${path} ")
    message("Compiler's prefix  :${prefix}")

    #通用的编译参数
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g -Os -flto  -std=c11 -mthumb -mcpu=${CMAKE_SYSTEM_PROCESSOR}" PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g0 -Os -flto -std=c++11 -mthumb -mcpu=${CMAKE_SYSTEM_PROCESSOR}" PARENT_SCOPE)
    #Nano lib
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -flto --specs=nano.specs --specs=nosys.specs -mthumb -mabi=aapcs -mcpu=${CMAKE_SYSTEM_PROCESSOR} -T${linkscript}" PARENT_SCOPE)

endfunction()

function(cortex_enable_fpu)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES cortex-m4)
        message("Target's FPU enabled !")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=fpv4-sp-d16 -mfloat-abi=hard" PARENT_SCOPE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=fpv4-sp-d16 -mfloat-abi=hard" PARENT_SCOPE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mfpu=fpv4-sp-d16 -mfloat-abi=hard" PARENT_SCOPE)
    endif()
endfunction()

function(cortex_build_info)
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf)
endfunction()

function(cortex_build_bin)
    ADD_CUSTOM_TARGET(${CMAKE_PROJECT_NAME}.bin ALL DEPENDS ${CMAKE_PROJECT_NAME} COMMAND ${CMAKE_OBJCOPY} -Obinary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin)
endfunction()

