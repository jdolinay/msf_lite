################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"$(COMMON)/coniob.c" \
"$(COMMON)/msf_print.c" \

C_SRCS += \
$(COMMON)/coniob.c \
$(COMMON)/msf_print.c \

OBJS += \
./common/coniob.o \
./common/msf_print.o \

C_DEPS += \
./common/coniob.d \
./common/msf_print.d \

OBJS_QUOTED += \
"./common/coniob.o" \
"./common/msf_print.o" \

C_DEPS_QUOTED += \
"./common/coniob.d" \
"./common/msf_print.d" \

OBJS_OS_FORMAT += \
./common/coniob.o \
./common/msf_print.o \


# Each subdirectory must supply rules for building sources it contributes
common/coniob.o: $(COMMON)/coniob.c
	@echo 'Building file: $<'
	@echo 'Executing target #6 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"common/coniob.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"common/coniob.o"
	@echo 'Finished building: $<'
	@echo ' '

common/msf_print.o: $(COMMON)/msf_print.c
	@echo 'Building file: $<'
	@echo 'Executing target #7 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"common/msf_print.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"common/msf_print.o"
	@echo 'Finished building: $<'
	@echo ' '


