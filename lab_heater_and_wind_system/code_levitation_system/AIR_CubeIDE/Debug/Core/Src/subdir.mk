################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Controller_Real_time.c \
../Core/Src/Network_controller.c \
../Core/Src/SMC.c \
../Core/Src/TCLab.c \
../Core/Src/VL53L0X.c \
../Core/Src/app_ethernet.c \
../Core/Src/main.c \
../Core/Src/read_controller.c \
../Core/Src/stm32f7xx_hal_msp.c \
../Core/Src/stm32f7xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f7xx.c \
../Core/Src/udp_echoserver.c 

OBJS += \
./Core/Src/Controller_Real_time.o \
./Core/Src/Network_controller.o \
./Core/Src/SMC.o \
./Core/Src/TCLab.o \
./Core/Src/VL53L0X.o \
./Core/Src/app_ethernet.o \
./Core/Src/main.o \
./Core/Src/read_controller.o \
./Core/Src/stm32f7xx_hal_msp.o \
./Core/Src/stm32f7xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f7xx.o \
./Core/Src/udp_echoserver.o 

C_DEPS += \
./Core/Src/Controller_Real_time.d \
./Core/Src/Network_controller.d \
./Core/Src/SMC.d \
./Core/Src/TCLab.d \
./Core/Src/VL53L0X.d \
./Core/Src/app_ethernet.d \
./Core/Src/main.d \
./Core/Src/read_controller.d \
./Core/Src/stm32f7xx_hal_msp.d \
./Core/Src/stm32f7xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f7xx.d \
./Core/Src/udp_echoserver.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F767xx -c -I../LWIP/App -I../LWIP/Target -I../Core/Inc -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/system/arch -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/Controller_Real_time.d ./Core/Src/Controller_Real_time.o ./Core/Src/Network_controller.d ./Core/Src/Network_controller.o ./Core/Src/SMC.d ./Core/Src/SMC.o ./Core/Src/TCLab.d ./Core/Src/TCLab.o ./Core/Src/VL53L0X.d ./Core/Src/VL53L0X.o ./Core/Src/app_ethernet.d ./Core/Src/app_ethernet.o ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/read_controller.d ./Core/Src/read_controller.o ./Core/Src/stm32f7xx_hal_msp.d ./Core/Src/stm32f7xx_hal_msp.o ./Core/Src/stm32f7xx_it.d ./Core/Src/stm32f7xx_it.o ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/system_stm32f7xx.d ./Core/Src/system_stm32f7xx.o ./Core/Src/udp_echoserver.d ./Core/Src/udp_echoserver.o

.PHONY: clean-Core-2f-Src

