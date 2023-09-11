################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

S_SRCS += \
../src/load_elf.s 

C_SRCS += \
../src/devcfg.c \
../src/lab5_sd.c 

OBJS += \
./src/devcfg.o \
./src/lab5_sd.o \
./src/load_elf.o 

C_DEPS += \
./src/devcfg.d \
./src/lab5_sd.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v8 gcc compiler'
	aarch64-none-elf-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -I../../hello_world_bsp/psu_cortexa53_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v8 gcc assembler'
	aarch64-none-elf-gcc -c  -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


