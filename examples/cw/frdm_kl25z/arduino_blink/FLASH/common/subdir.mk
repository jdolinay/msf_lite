################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"$(COMMON)/conio.c" \
"$(COMMON)/msf_print.c" \

C_SRCS += \
$(COMMON)/conio.c \
$(COMMON)/msf_print.c \

OBJS += \
./common/conio.o \
./common/msf_print.o \

C_DEPS += \
./common/conio.d \
./common/msf_print.d \

OBJS_QUOTED += \
"./common/conio.o" \
"./common/msf_print.o" \

C_DEPS_QUOTED += \
"./common/conio.d" \
"./common/msf_print.d" \

OBJS_OS_FORMAT += \
./common/conio.o \
./common/msf_print.o \


# Each subdirectory must supply rules for building sources it contributes
common/conio.o: $(COMMON)/conio.c
	@echo 'Building file: $<'
	@echo 'Executing target #6 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"common/conio.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"common/conio.o"
	@echo 'Finished building: $<'
	@echo ' '

common/msf_print.o: $(COMMON)/msf_print.c
	@echo 'Building file: $<'
	@echo 'Executing target #7 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"common/msf_print.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"common/msf_print.o"
	@echo 'Finished building: $<'
	@echo ' '


