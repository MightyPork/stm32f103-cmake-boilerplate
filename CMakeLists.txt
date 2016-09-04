project(f103-bluepill C ASM)
cmake_minimum_required(VERSION 3.5.0)

file(GLOB_RECURSE USER_SOURCES "Src/*.c" "User/*.c")
file(GLOB_RECURSE HAL_SOURCES "Drivers/STM32F1xx_HAL_Driver/Src/*.c")

add_library(HAL ${HAL_SOURCES})
add_library(CMSIS
        Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c
        Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xb.s)

include_directories(Inc)
include_directories(User)
include_directories(Drivers/STM32F1xx_HAL_Driver/Inc)
include_directories(Drivers/CMSIS/Include)
include_directories(Drivers/CMSIS/Device/ST/STM32F1xx/Include)

add_definitions(-DSTM32F103x6)
add_executable(${PROJECT_NAME}.elf ${USER_SOURCES} ${LINKER_SCRIPT})

target_link_libraries(${PROJECT_NAME}.elf HAL CMSIS)

set(HEX_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.bin)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")
