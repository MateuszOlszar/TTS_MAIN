################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/CAN_LIB/src/CAN_FIFO.c \
../Drivers/CAN_LIB/src/CAN_LIB.c \
../Drivers/CAN_LIB/src/CAN_LIB_HW.c 

OBJS += \
./Drivers/CAN_LIB/src/CAN_FIFO.o \
./Drivers/CAN_LIB/src/CAN_LIB.o \
./Drivers/CAN_LIB/src/CAN_LIB_HW.o 

C_DEPS += \
./Drivers/CAN_LIB/src/CAN_FIFO.d \
./Drivers/CAN_LIB/src/CAN_LIB.d \
./Drivers/CAN_LIB/src/CAN_LIB_HW.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CAN_LIB/src/%.o Drivers/CAN_LIB/src/%.su Drivers/CAN_LIB/src/%.cyclo: ../Drivers/CAN_LIB/src/%.c Drivers/CAN_LIB/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F302x8 -c -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I../App/Inc -I../Drivers/CAN_LIB/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-CAN_LIB-2f-src

clean-Drivers-2f-CAN_LIB-2f-src:
	-$(RM) ./Drivers/CAN_LIB/src/CAN_FIFO.cyclo ./Drivers/CAN_LIB/src/CAN_FIFO.d ./Drivers/CAN_LIB/src/CAN_FIFO.o ./Drivers/CAN_LIB/src/CAN_FIFO.su ./Drivers/CAN_LIB/src/CAN_LIB.cyclo ./Drivers/CAN_LIB/src/CAN_LIB.d ./Drivers/CAN_LIB/src/CAN_LIB.o ./Drivers/CAN_LIB/src/CAN_LIB.su ./Drivers/CAN_LIB/src/CAN_LIB_HW.cyclo ./Drivers/CAN_LIB/src/CAN_LIB_HW.d ./Drivers/CAN_LIB/src/CAN_LIB_HW.o ./Drivers/CAN_LIB/src/CAN_LIB_HW.su

.PHONY: clean-Drivers-2f-CAN_LIB-2f-src

