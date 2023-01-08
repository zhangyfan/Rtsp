set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(sysroot_target /home/z/rv1126/buildroot/output/rockchip_rv1126_rv1109_spi_nand/host/arm-buildroot-linux-gnueabihf/sysroot)
set(tools /home/z/rv1126/prebuilts/gcc/linux-x86/arm/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin)
set(CMAKE_C_COMPILER ${tools}/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${tools}/arm-linux-gnueabihf-g++)
set(CMAKE_SYSROOT ${sysroot_target})
SET(CMAKE_CXX_FLAGS " -march=armv7-a -marm -mfpu=neon -mfloat-abi=hard --sysroot=${sysroot_target}")
SET(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS})
SET(CMAKE_EXE_LINKER_FLAGS "--sysroot=${sysroot_target}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)