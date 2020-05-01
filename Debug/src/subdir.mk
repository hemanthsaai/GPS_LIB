################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Library.c \
../src/he_nmea_lib.c \
../src/he_std.c 

OBJS += \
./src/Library.o \
./src/he_nmea_lib.o \
./src/he_std.o 

C_DEPS += \
./src/Library.d \
./src/he_nmea_lib.d \
./src/he_std.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


