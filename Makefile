# Project configs
debug 		?= 0
sampling	?= 0
NAME		:= raytracer
SRC_DIR		:= src
BUILD_DIR	:= build
INCLUDE_DIR	:= include
BIN_DIR 	:= bin


# Object file paths
OBJS 		:= $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp)) # it's patsubst not patsubset smh

# Compiler settings
CC 		:= g++

# Flags settings (Compiler and Linker)
# -O3 is used for aggressive optimization
CFLAGS 		:= -Wall -Wextra -pedantic -std=c++11 -I$(INCLUDE_DIR) -fopenmp
CFLAGS		:= $(CFLAGS) -O3 -ffast-math -fipa-pure-const -freciprocal-math -mtune=native -fivopts
LFLAGS 		:= -lSDL2 -lm

ifeq ($(debug), 1)
	CFLAGS 	:= $(CFLAGS) -g -D DEBUG_MODE
endif

ifeq ($(sampling), 1)
	CFLAGS := $(CFLAGS) -D SAMPLING_MODE
endif


all: $(NAME)

# Build exec
$(NAME): dir $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o $(BIN_DIR)/$@

# Object build rule
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | dir
	$(CC) $(CFLAGS) -c $< -o $@

dir:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean dir
