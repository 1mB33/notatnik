INTERMEDIATE_DIR := intermediate
LIB_DATABASE_MANAGER_SRC_DIR := database_manager/src
LIB_DIR := lib
APP_SRC_DIR := app/src
BIN_OUT_DIR := bin/application

CXX := g++
CXXFLAGS := -Wall -Wextra 
APP_SOURCES := $(wildcard $(APP_SRC_DIR)/*.cpp)
APP_OBJECTS := $(patsubst $(APP_SRC_DIR)/%.cpp, $(INTERMEDIATE_DIR)/%.o, $(APP_SOURCES))

.DEFAULT_GOAL := all

run: all
	$(APP_DIR)/application

all: build_app

build_app: $(APP_OBJECTS)
	$(CXX) $(CXXFLAGS) \
		$^ -o $(BIN_OUT_DIR)/$@

$(INTERMEDIATE_DIR)/%.o: $(APP_SRC_DIR)/%.cpp Makefile out_dirs
	$(CXX) $(CXXFLAGS) \
		-I $(LIB_DATABASE_MANAGER_SRC)/ \
		-L$(LIB_DIR) \
		-ldatabase_manager -Wl,-rpath,$(pwd)$(LIB_DIR) \
		$< -o $@

out_dirs:
	mkdir -p $(BIN_OUT_DIR)
	mkdir -p $(LIB_DIR)
	mkdir -p $(INTERMEDIATE_DIR)

clean: 
	rm -fr $(BIN_DIR)
	rm -fr $(LIB_DIR)
	rm -fr $(INTERMEDIATE_DIR)
