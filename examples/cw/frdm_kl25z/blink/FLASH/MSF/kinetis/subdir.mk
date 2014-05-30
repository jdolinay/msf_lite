################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

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
./MSF/kinetis/adc_kl25.o \
./MSF/kinetis/msf.o \
./MSF/kinetis/msf_analog.o \
./MSF/kinetis/uart_kl25.o \

C_DEPS += \
./MSF/kinetis/adc_kl25.d \
./MSF/kinetis/msf.d \
./MSF/kinetis/msf_analog.d \
./MSF/kinetis/uart_kl25.d \

OBJS_QUOTED += \
"./MSF/kinetis/adc_kl25.o" \
"./MSF/kinetis/msf.o" \
"./MSF/kinetis/msf_analog.o" \
"./MSF/kinetis/uart_kl25.o" \

C_DEPS_QUOTED += \
"./MSF/kinetis/adc_kl25.d" \
"./MSF/kinetis/msf.d" \
"./MSF/kinetis/msf_analog.d" \
"./MSF/kinetis/uart_kl25.d" \

OBJS_OS_FORMAT += \
./MSF/kinetis/adc_kl25.o \
./MSF/kinetis/msf.o \
./MSF/kinetis/msf_analog.o \
./MSF/kinetis/uart_kl25.o \


# Each subdirectory must supply rules for building sources it contributes
MSF/kinetis/adc_kl25.o: $(KINETIS)/adc_kl25.c
	@echo 'Building file: $<'
	@echo 'Executing target #6 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"MSF/kinetis/adc_kl25.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"MSF/kinetis/adc_kl25.o"
	@echo 'Finished building: $<'
	@echo ' '

MSF/kinetis/msf.o: $(KINETIS)/msf.c
	@echo 'Building file: $<'
	@echo 'Executing target #7 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"MSF/kinetis/msf.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"MSF/kinetis/msf.o"
	@echo 'Finished building: $<'
	@echo ' '

MSF/kinetis/msf_analog.o: $(KINETIS)/msf_analog.c
	@echo 'Building file: $<'
	@echo 'Executing target #8 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"MSF/kinetis/msf_analog.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"MSF/kinetis/msf_analog.o"
	@echo 'Finished building: $<'
	@echo ' '

MSF/kinetis/uart_kl25.o: $(KINETIS)/uart_kl25.c
	@echo 'Building file: $<'
	@echo 'Executing target #9 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"MSF/kinetis/uart_kl25.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"MSF/kinetis/uart_kl25.o"
	@echo 'Finished building: $<'
	@echo ' '


