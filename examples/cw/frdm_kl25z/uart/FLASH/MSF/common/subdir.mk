################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"$(COMMON)/conio.c" \
"$(COMMON)/coniob.c" \
"$(COMMON)/msf_print.c" \

C_SRCS += \
$(COMMON)/conio.c \
$(COMMON)/coniob.c \
$(COMMON)/msf_print.c \

OBJS += \
./MSF/common/conio.o \
./MSF/common/coniob.o \
./MSF/common/msf_print.o \

C_DEPS += \
./MSF/common/conio.d \
./MSF/common/coniob.d \
./MSF/common/msf_print.d \

OBJS_QUOTED += \
"./MSF/common/conio.o" \
"./MSF/common/coniob.o" \
"./MSF/common/msf_print.o" \

C_DEPS_QUOTED += \
"./MSF/common/conio.d" \
"./MSF/common/coniob.d" \
"./MSF/common/msf_print.d" \

OBJS_OS_FORMAT += \
./MSF/common/conio.o \
./MSF/common/coniob.o \
./MSF/common/msf_print.o \


# Each subdirectory must supply rules for building sources it contributes
MSF/common/conio.o: $(COMMON)/conio.c
	@echo 'Building file: $<'
	@echo 'Executing target #11 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"MSF/common/conio.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"MSF/common/conio.o"
	@echo 'Finished building: $<'
	@echo ' '

MSF/common/coniob.o: $(COMMON)/coniob.c
	@echo 'Building file: $<'
	@echo 'Executing target #12 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"MSF/common/coniob.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"MSF/common/coniob.o"
	@echo 'Finished building: $<'
	@echo ' '

MSF/common/msf_print.o: $(COMMON)/msf_print.c
	@echo 'Building file: $<'
	@echo 'Executing target #13 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"MSF/common/msf_print.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"MSF/common/msf_print.o"
	@echo 'Finished building: $<'
	@echo ' '


