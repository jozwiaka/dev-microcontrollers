################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Main/ProceduryASM.s 

C_SRCS += \
../Main/main.c \
../Main/main_base.c \
../Main/stm32f4xx_hal_msp.c \
../Main/stm32f4xx_it.c 

OBJS += \
./Main/ProceduryASM.o \
./Main/main.o \
./Main/main_base.o \
./Main/stm32f4xx_hal_msp.o \
./Main/stm32f4xx_it.o 

C_DEPS += \
./Main/main.d \
./Main/main_base.d \
./Main/stm32f4xx_hal_msp.d \
./Main/stm32f4xx_it.d 


# Each subdirectory must supply rules for building sources it contributes
Main/%.o: ../Main/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Main/%.o: ../Main/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc  -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -DSTM32F429ZITx -DVECT_TAB_SRAM -DUSE_STM32F429I_DISCO -DSTM32F429xx -DUSE_HAL_DRIVER -DSTM32F429I_DISC1 -DSTM32F4 -DSTM32 -DDEBUG -I"C:/Studia/TM/STM32/Projekty/Zad1_Linia/Drivers/BSP" -I"C:/Studia/TM/STM32/Projekty/Zad1_Linia/Drivers/CMSIS" -I"C:/Studia/TM/STM32/Projekty/Zad1_Linia/Drivers/STM32F4xx_HAL_Driver" -I"C:/Studia/TM/STM32/Projekty/Zad1_Linia/Drivers/Utils" -I"C:/Studia/TM/STM32/Projekty/Zad1_Linia/Main" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


