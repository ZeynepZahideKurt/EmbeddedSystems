################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
TimerOutputCompre.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
TimerOutputCompre.list \

OBJCOPY_BIN += \
TimerOutputCompre.bin \


# All Target
all: TimerOutputCompre.elf secondary-outputs

# Tool invocations
TimerOutputCompre.elf: $(OBJS) $(USER_OBJS) E:\CubeMX5\TimerOutputCompre\STM32F411VETX_FLASH.ld
	arm-none-eabi-gcc -o "TimerOutputCompre.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"E:\CubeMX5\TimerOutputCompre\STM32F411VETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="TimerOutputCompre.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

TimerOutputCompre.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "TimerOutputCompre.list"
	@echo 'Finished building: $@'
	@echo ' '

TimerOutputCompre.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "TimerOutputCompre.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
