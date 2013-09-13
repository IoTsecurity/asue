C_COMPILER := gcc
RM := rm -rf 

EXE := ./asue

.PHONY: all clean

# All Target
all: $(EXE)

# Add inputs and outputs from these tool invocations to the build variables 
LIB_DIR := -L/usr/local/openssl/lib
LIBS := -lssl -lcrypto
OBJS := ./asue.o
USER_OBJS := 

# Tool invocations
$(EXE): $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	$(C_COMPILER) -ldl $(LIB_DIR) $(LIBS) -o $@ $< $(USER_OBJS) 
	@echo 'Finished building target: $@'
	@echo ' '

# All of the sources participating in the build are defined here
INCLUDE_DIR := -I/usr/local/openssl/include

# Each subdirectory must supply rules for building sources it contributes
./%.o: ./%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	$(C_COMPILER) -O0 -g3 -Wall $(INCLUDE_DIR) -o $@ -c $<
	@echo 'Finished building: $<'
	@echo ' '

clean:
	-$(RM) $(OBJS) $(EXE)
	-@echo ' '

.SECONDARY:




