################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"$(FRDM_KL25Z)/arduino.c" \

C_SRCS += \
$(FRDM_KL25Z)/arduino.c \

OBJS += \
./MSF/frdm_kl25z/arduino.o \

C_DEPS += \
./MSF/frdm_kl25z/arduino.d \

OBJS_QUOTED += \
"./MSF/frdm_kl25z/arduino.o" \

C_DEPS_QUOTED += \
"./MSF/frdm_kl25z/arduino.d" \

OBJS_OS_FORMAT += \
./MSF/frdm_kl25z/arduino.o \


# Each subdirectory must supply rules for building sources it contributes
MSF/frdm_kl25z/arduino.o: $(FRDM_KL25Z)/arduino.c
	@echo 'Building file: $<'
	@echo 'Executing target #10 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"MSF/frdm_kl25z/arduino.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"MSF/frdm_kl25z/arduino.o"
	@echo 'Finished building: $<'
	@echo ' '


