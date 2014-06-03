################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"$(KINETIS)/adc_kl25.c" \
"$(KINETIS)/msf.c" \
"$(KINETIS)/msf_analog.c" \
"$(KINETIS)/uart_kl25.c" \

C_SRCS += \
$(KINETIS)/adc_kl25.c \
$(KINETIS)/msf.c \
$(KINETIS)/msf_analog.c \
$(KINETIS)/uart_kl25.c \

OBJS += \
./kinetis/adc_kl25.o \
./kinetis/msf.o \
./kinetis/msf_analog.o \
./kinetis/uart_kl25.o \

C_DEPS += \
./kinetis/adc_kl25.d \
./kinetis/msf.d \
./kinetis/msf_analog.d \
./kinetis/uart_kl25.d \

OBJS_QUOTED += \
"./kinetis/adc_kl25.o" \
"./kinetis/msf.o" \
"./kinetis/msf_analog.o" \
"./kinetis/uart_kl25.o" \

C_DEPS_QUOTED += \
"./kinetis/adc_kl25.d" \
"./kinetis/msf.d" \
"./kinetis/msf_analog.d" \
"./kinetis/uart_kl25.d" \

OBJS_OS_FORMAT += \
./kinetis/adc_kl25.o \
./kinetis/msf.o \
./kinetis/msf_analog.o \
./kinetis/uart_kl25.o \


# Each subdirectory must supply rules for building sources it contributes
kinetis/adc_kl25.o: $(KINETIS)/adc_kl25.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"kinetis/adc_kl25.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"kinetis/adc_kl25.o"
	@echo 'Finished building: $<'
	@echo ' '

kinetis/msf.o: $(KINETIS)/msf.c
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"kinetis/msf.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"kinetis/msf.o"
	@echo 'Finished building: $<'
	@echo ' '

kinetis/msf_analog.o: $(KINETIS)/msf_analog.c
	@echo 'Building file: $<'
	@echo 'Executing target #3 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"kinetis/msf_analog.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"kinetis/msf_analog.o"
	@echo 'Finished building: $<'
	@echo ' '

kinetis/uart_kl25.o: $(KINETIS)/uart_kl25.c
	@echo 'Building file: $<'
	@echo 'Executing target #4 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"kinetis/uart_kl25.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"kinetis/uart_kl25.o"
	@echo 'Finished building: $<'
	@echo ' '


