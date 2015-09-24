################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../My_Barrier.cpp 

CC_SRCS += \
../Complex.cc \
../InputImage.cc \
../threadDFT2d-skeleton.cc 

OBJS += \
./Complex.o \
./InputImage.o \
./My_Barrier.o \
./threadDFT2d-skeleton.o 

CC_DEPS += \
./Complex.d \
./InputImage.d \
./threadDFT2d-skeleton.d 

CPP_DEPS += \
./My_Barrier.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -pthread -fpermissive -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -pthread -fpermissive -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


