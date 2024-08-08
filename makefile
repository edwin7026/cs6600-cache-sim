# project name
PROJECT_NAME:=cache

# important paths
ROOT_PATH:=$(PWD)
SRC_PATH:=$(ROOT_PATH)/src/
INCLUDE_PATH:=$(SRC_PATH)/include

# compilation arguments
C_FLAGS:=
C_INCLUDES:=-I$(INCLUDE_PATH)/

# build parameters
BUILD_DIR:=build

run:
	@echo "Running simulation"
	@$(BUILD_DIR)/$(PROJECT_NAME)

# make the target
$(BUILD_DIR)/$(PROJECT_NAME): \
	$(SRC_PATH)/* $(INCLUDE_PATH)/*
	mkdir -p build; g++ -o $@ $(C_FLAGS) $(C_INCLUDES) ./src/main.cpp

# clean project
clean:
	@rm -rf build