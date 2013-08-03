C_COMPILER := gcc
RM := rm -rf 

# All of the sources participating in the build are defined here
INCLUDE_DIR := /usr/local/openssl/include
OBJS := ./asue.o
USER_OBJS := 

# Each subdirectory must supply rules for building sources it contributes
./%.o: ./%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	$(C_COMPILER) -I$(INCLUDE_DIR) -O0 -g3 -Wall -o $@ -c $<
	@echo 'Finished building: $<'
	@echo ' '

# Add inputs and outputs from these tool invocations to the build variables 
EXE := ./asue
LIB_DIR := /usr/local/openssl/lib
LIBS := -lssl -lcrypto

# All Target
all: $(EXE)

# Tool invocations
$(EXE): $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	$(C_COMPILER) -L$(LIB_DIR) -ldl -o $@ $< $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

clean:
	-$(RM) $(OBJS) $(EXE)
	-@echo ' '

.PHONY: all clean
.SECONDARY:




