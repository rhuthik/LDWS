CXX      := -g++
CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror  `pkg-config --cflags --libs opencv4`
LDFLAGS  := -L/usr/lib -lstdc++ -lm
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := ldws
INCLUDE  := -Iinclude/
SRC      :=                      \
   $(wildcard src/deblurring/*.cpp) \
   $(wildcard src/module2/*.cpp) \
   $(wildcard src/shedutil/*.cpp) \
   $(wildcard src/*.cpp)         \

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
DEPENDENCIES \
         := $(OBJECTS:.o=.d)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

-include $(DEPENDENCIES)

.PHONY: all build clean debug release info

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

clean:
	-@rm -rvf $(BUILD)/*
