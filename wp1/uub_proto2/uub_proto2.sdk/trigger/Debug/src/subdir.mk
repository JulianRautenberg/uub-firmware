################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/FeShwrRead_test.c \
../src/read_evt.c \
../src/trigger.c 

OBJS += \
./src/FeShwrRead_test.o \
./src/read_evt.o \
./src/trigger.o 

C_DEPS += \
./src/FeShwrRead_test.d \
./src/read_evt.d \
./src/trigger.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Linux gcc compiler'
	arm-xilinx-linux-gnueabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

