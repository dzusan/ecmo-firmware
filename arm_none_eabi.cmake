# CMAKE toolchain for the gcc arm-none-eabi

set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_VERSION   1)
set(CMAKE_SYSTEM_PROCESSOR arm-none-eabi)
set(TOOL_CHAIN_PREFIX      arm-none-eabi)

# which compilers to use for C and C++
SET(CMAKE_AR               ${TOOL_CHAIN_PREFIX}-gcc-ar${EXE})
SET(CMAKE_RANLIB           ${TOOL_CHAIN_PREFIX}-gcc-ranlib${EXE})
SET(CMAKE_LD               ${TOOL_CHAIN_PREFIX}-ld${EXE})
set(CMAKE_C_COMPILER       ${TOOL_CHAIN_PREFIX}-gcc${EXE})
set(CMAKE_CXX_COMPILER     ${TOOL_CHAIN_PREFIX}-g++${EXE})
set(CMAKE_ASM_COMPILER     ${TOOL_CHAIN_PREFIX}-gcc${EXE})
set(CMAKE_OBJCOPY     	   ${TOOL_CHAIN_PREFIX}-objcopy${EXE} CACHE INTERNAL "objcopy command")
set(CMAKE_OBJDUMP     	   ${TOOL_CHAIN_PREFIX}-objdump${EXE} CACHE INTERNAL "objdump command")
set(CMAKE_GDB              ${TOOL_CHAIN_PREFIX}-gdb${EXE})
set(CMAKE_SIZE             ${TOOL_CHAIN_PREFIX}-size${EXE})

set(CMAKE_C_ARCHIVE_CREATE "${CMAKE_AR} qc <TARGET> <OBJECTS>")
set(CMAKE_C_ARCHIVE_FINISH "<CMAKE_RANLIB> <TARGET>")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Common build options
set(SPEC_FLAGS "--specs=nosys.specs")
set(LD_FLAGS "") # maybe "-nostartfiles"
set(BEHAVIOR_FLAGS "-Werror=implicit-function-declaration") # -Werror=return-type -Wstrict-prototypes

# -ggdb3 -g -gdwarf-2 to reveal source files in Ozone, or -Os for release
# NOT use -Og with FreeRTOS+TCP due to compiler not eliminate unused vNetworkInterfaceAllocateRAMToBuffers()
# If you are using an individual optimization option somewhere in CMake files it will be added after the following options.
# The last optimization option is the one that is effective.
# See experiment https://stackoverflow.com/a/66744547
set(DBG_FLAGS "-O1 -ggdb3 -g -gdwarf-2")

set(CMAKE_C_FLAGS          "${MCPU_FLAGS} ${VFP_FLAGS} ${SPEC_FLAGS} ${DBG_FLAGS} ${BEHAVIOR_FLAGS} -fdata-sections -ffunction-sections -mlong-calls" CACHE INTERNAL "c compiler flags")
set(CMAKE_ASM_FLAGS        "${MCPU_FLAGS} ${VFP_FLAGS} -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")
set(CMAKE_EXE_LINKER_FLAGS "${MCPU_FLAGS} ${LD_FLAGS} -T ${LINKER_FILE} -Wl,--gc-sections -Wl,-Map=output.map -lm -lc" CACHE INTERNAL "exe link flags")
