################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
device/device.obj: ../device/device.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -Ooff --opt_for_speed=2 --include_path="C:/Users/Owner/Desktop/School/453/CCS/guitar_tuner_control" --include_path="C:/Users/Owner/Desktop/School/453/CCS/guitar_tuner_control/device" --include_path="C:/ti/tirex-content/C2000Ware_1_00_00_00_Software/driverlib/f2837xs/driverlib" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --define=DEBUG --define=CPU1 --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="device/device.d" --obj_directory="device" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


