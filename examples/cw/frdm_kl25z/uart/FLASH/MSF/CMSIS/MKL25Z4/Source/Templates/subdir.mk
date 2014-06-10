################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"$(TEMPLATES)/system_MKL25Z4.c" \

C_SRCS += \
$(TEMPLATES)/system_MKL25Z4.c \

OBJS += \
./MSF/CMSIS/MKL25Z4/Source/Templates/system_MKL25Z4.o \

C_DEPS += \
./MSF/CMSIS/MKL25Z4/Source/Templates/system_MKL25Z4.d \

OBJS_QUOTED += \
"./MSF/CMSIS/MKL25Z4/Source/Templates/system_MKL25Z4.o" \

C_DEPS_QUOTED += \
"./MSF/CMSIS/MKL25Z4/Source/Templates/system_MKL25Z4.d" \

OBJS_OS_FORMAT += \
./MSF/CMSIS/MKL25Z4/Source/Templates/system_MKL25Z4.o \


# Each subdirectory must supply rules for building sources it contributes
MSF/CMSIS/MKL25Z4/Source/Templates/system_MKL25Z4.o: $(TEMPLATES)/system_MKL25Z4.c
	@echo 'Building file: $<'
	@echo 'Executing target #13 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"MSF/CMSIS/MKL25Z4/Source/Templates/system_MKL25Z4.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"MSF/CMSIS/MKL25Z4/Source/Templates/system_MKL25Z4.o"
	@echo 'Finished building: $<'
	@echo ' '


