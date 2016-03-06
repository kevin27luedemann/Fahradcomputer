################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../BMP180.cpp \
../diskio.c \
../Display.cpp \
../ff.c \
../I2C.cpp \
../Interface.cpp \
../Kompass.cpp \
../LSM303D.cpp \
../main.cpp \
../Output.cpp \
../RTC.cpp \
../SOUND.cpp


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
BMP180.o \
diskio.o \
Display.o \
ff.o \
I2C.o \
Interface.o \
Kompass.o \
LSM303D.o \
main.o \
Output.o \
RTC.o \
SOUND.o

OBJS_AS_ARGS +=  \
BMP180.o \
diskio.o \
Display.o \
ff.o \
I2C.o \
Interface.o \
Kompass.o \
LSM303D.o \
main.o \
Output.o \
RTC.o \
SOUND.o

C_DEPS +=  \
BMP180.d \
diskio.d \
Display.d \
ff.d \
I2C.d \
Interface.d \
Kompass.d \
LSM303D.d \
main.d \
Output.d \
RTC.d \
SOUND.d

C_DEPS_AS_ARGS +=  \
BMP180.d \
diskio.d \
Display.d \
ff.d \
I2C.d \
Interface.d \
Kompass.d \
LSM303D.d \
main.d \
Output.d \
RTC.d \
SOUND.d

OUTPUT_FILE_PATH +=FWCP_MK3.elf

OUTPUT_FILE_PATH_AS_ARGS +=FWCP_MK3.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler

























./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C Compiler : 4.9.2
	$(QUOTE)C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG  -I"C:\Program Files\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.0.98\include"  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax -g2 -Wall -mmcu=atmega1284p -B "C:\Program Files\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.0.98\gcc\dev\atmega1284p" -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

./%.o: .././%.cpp
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C Compiler : 4.9.2
	$(QUOTE)C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DDEBUG  -I"C:\Program Files\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.0.98\include"  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax -g2 -Wall -mmcu=atmega1284p -B "C:\Program Files\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.0.98\gcc\dev\atmega1284p" -c -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR8/GNU Linker : 4.9.2
	$(QUOTE)C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="FWCP_MK3.map" -Wl,-u,vfprintf -Wl,--start-group -Wl,-lm -Wl,-lprintf_flt  -Wl,--end-group -Wl,--gc-sections -mrelax -mmcu=atmega1284p -B "C:\Program Files\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.0.98\gcc\dev\atmega1284p"  
	@echo Finished building target: $@
	"C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "FWCP_MK3.elf" "FWCP_MK3.hex"
	"C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "FWCP_MK3.elf" "FWCP_MK3.eep" || exit 0
	"C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "FWCP_MK3.elf" > "FWCP_MK3.lss"
	"C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "FWCP_MK3.elf" "FWCP_MK3.srec"
	"C:\Program Files\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "FWCP_MK3.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "FWCP_MK3.elf" "FWCP_MK3.a" "FWCP_MK3.hex" "FWCP_MK3.lss" "FWCP_MK3.eep" "FWCP_MK3.map" "FWCP_MK3.srec" "FWCP_MK3.usersignatures"
	