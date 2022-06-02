################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F3xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
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
stm32f303-UART\ HABERLESME.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
stm32f303-UART\ HABERLESME.list \

OBJCOPY_BIN += \
stm32f303-UART\ HABERLESME.bin \


# All Target
all: stm32f303-UART\ HABERLESME.elf secondary-outputs

# Tool invocations
stm32f303-UART\ HABERLESME.elf: $(OBJS) $(USER_OBJS) C:\Users\Enelsis\Desktop\enelsis\deneme\stm32f303-UART\ HABERLESME\STM32F303RETX_FLASH.ld
	arm-none-eabi-gcc -o "stm32f303-UART HABERLESME.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\Enelsis\Desktop\enelsis\deneme\stm32f303-UART HABERLESME\STM32F303RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="stm32f303-UART HABERLESME.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

stm32f303-UART\ HABERLESME.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "stm32f303-UART HABERLESME.list"
	@echo 'Finished building: $@'
	@echo ' '

stm32f303-UART\ HABERLESME.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "stm32f303-UART HABERLESME.bin"
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
