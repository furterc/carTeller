######################################
# target
######################################
TARGET = carTeller
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m0plus

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32L053R8Tx_FLASH.ld

# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
$(wildcard Drivers/STM32L0xx_HAL_Driver/Src/*.c) \
$(wildcard Drivers/BSP/Utils/Src/*.c) \
$(wildcard Src/*.c)

# C++ sources
CXX_SOURCES =  \
$(wildcard Src/*.cpp) \
$(wildcard Drivers/BSP/Utils/Src/*.cpp)

# ASM sources
ASM_SOURCES =  \
startup_stm32l053xx.s

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32L053xx

# C includes
C_INCLUDES =  \
-IInc \
-IDrivers/STM32L0xx_HAL_Driver/Inc \
-IDrivers/BSP/Utils/Inc \
-IDrivers/STM32L0xx_HAL_Driver/Inc/Legacy \
-IDrivers/CMSIS/Device/ST/STM32L0xx/Include \
-IDrivers/CMSIS/Include
