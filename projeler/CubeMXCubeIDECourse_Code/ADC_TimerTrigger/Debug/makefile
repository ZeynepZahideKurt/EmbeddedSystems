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
ADC_TimerTrigger.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
ADC_TimerTrigger.list \

OBJCOPY_BIN += \
ADC_TimerTrigger.bin \


# All Target
all: ADC_TimerTrigger.elf secondary-outputs

# Tool invocations
ADC_TimerTrigger.elf: $(OBJS) $(USER_OBJS) E:\CubeMX5\ADC_TimerTrigger\STM32F411VETX_FLASH.ld
	arm-none-eabi-gcc -o "ADC_TimerTrigger.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"E:\CubeMX5\ADC_TimerTrigger\STM32F411VETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="ADC_TimerTrigger.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

ADC_TimerTrigger.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "ADC_TimerTrigger.list"
	@echo 'Finished building: $@'
	@echo ' '

ADC_TimerTrigger.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "ADC_TimerTrigger.bin"
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
