################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/Src/App.c \
../App/Src/FLASH_Handler.c \
../App/Src/MLX90621_API.c \
../App/Src/MLX90621_I2C_Driver.c 

OBJS += \
./App/Src/App.o \
./App/Src/FLASH_Handler.o \
./App/Src/MLX90621_API.o \
./App/Src/MLX90621_I2C_Driver.o 

C_DEPS += \
./App/Src/App.d \
./App/Src/FLASH_Handler.d \
./App/Src/MLX90621_API.d \
./App/Src/MLX90621_I2C_Driver.d 


# Each subdirectory must supply rules for building sources it contributes
App/Src/%.o App/Src/%.su App/Src/%.cyclo: ../App/Src/%.c App/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F302x8 -c -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I../App/Inc -I../Drivers/CAN_LIB/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-App-2f-Src

clean-App-2f-Src:
	-$(RM) ./App/Src/App.cyclo ./App/Src/App.d ./App/Src/App.o ./App/Src/App.su ./App/Src/FLASH_Handler.cyclo ./App/Src/FLASH_Handler.d ./App/Src/FLASH_Handler.o ./App/Src/FLASH_Handler.su ./App/Src/MLX90621_API.cyclo ./App/Src/MLX90621_API.d ./App/Src/MLX90621_API.o ./App/Src/MLX90621_API.su ./App/Src/MLX90621_I2C_Driver.cyclo ./App/Src/MLX90621_I2C_Driver.d ./App/Src/MLX90621_I2C_Driver.o ./App/Src/MLX90621_I2C_Driver.su

.PHONY: clean-App-2f-Src

