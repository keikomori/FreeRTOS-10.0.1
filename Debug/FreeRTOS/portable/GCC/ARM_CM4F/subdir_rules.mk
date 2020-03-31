################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/portable/GCC/ARM_CM4F/%.o: ../FreeRTOS/portable/GCC/ARM_CM4F/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"D:/Program Files/ti/ccs1000/ccs/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major-win32/bin/arm-none-eabi-gcc-7.2.1.exe" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C1294NCPDT -DTARGET_IS_TM4C129_RA1 -Dgcc -I"D:/Program Files/ti/TivaWare_C_Series-2.1.4.178" -I"D:/CodeComposerProjects/FreeRTOS_10.0.1/FreeRTOS-PLUS-CLI" -I"D:/CodeComposerProjects/FreeRTOS_10.0.1/FreeRTOS-Plus-IO/Device/TM4C1294/SupportedBoards" -I"D:/CodeComposerProjects/FreeRTOS_10.0.1/FreeRTOS-Plus-IO" -I"D:/CodeComposerProjects/FreeRTOS_10.0.1/FreeRTOS/include" -I"D:/CodeComposerProjects/FreeRTOS_10.0.1/FreeRTOS-Plus-IO/Include" -I"D:/CodeComposerProjects/FreeRTOS_10.0.1/FreeRTOS-Plus-IO/Device/TM4C1294" -I"D:/CodeComposerProjects/FreeRTOS_10.0.1/targetConfigs" -I"D:/CodeComposerProjects/FreeRTOS_10.0.1/FreeRTOS/portable/GCC/ARM_CM4F" -I"D:/CodeComposerProjects/FreeRTOS_10.0.1" -I"D:/CodeComposerProjects/FreeRTOS_10.0.1/FreeRTOS" -I"D:/Program Files/ti/ccs1000/ccs/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major-win32/arm-none-eabi/include" -Og -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


