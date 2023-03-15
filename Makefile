CXX      := -g++
CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror  `pkg-config --cflags --libs opencv4` -fopenmp
LDFLAGS  := -L/usr/lib -lstdc++ -lm -I/usr/include/opencv4/ -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := ldws
INCLUDE  := -Iinclude/ 
SRC      :=                      \
   $(wildcard src/blurring/*.cpp) \
   $(wildcard src/thresholding/*.cpp) \
   $(wildcard src/edgedetection/*.cpp) \
   $(wildcard src/hough/*.cpp) \
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
