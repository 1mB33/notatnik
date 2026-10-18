INTERMEDIATE_DIR := intermediate
LIB_DATABASE_MANAGER_INCLUDE_DIR := database_manager/include
LIB_DATABASE_MANAGER_SRC_DIR := database_manager/src
LIB_OUT_DIR := lib
APP_SRC_DIR := app/src
BIN_OUT_DIR := bin/application

ENV_DB_NAME := "notepad_dev_db"
ENV_DB_USER := "dev"
ENV_DB_PASS := "dev"

CXX := g++
CXXFLAGS := -Wall -Wextra 
DATABASE_MANAGER_SOURCES := $(wildcard $(LIB_DATABASE_MANAGER_SRC_DIR)/*.cpp)
DATABASE_MANAGER_OBJECTS := $(patsubst $(LIB_DATABASE_MANAGER_SRC_DIR)/%.cpp, $(INTERMEDIATE_DIR)/%.o, $(DATABASE_MANAGER_SOURCES))
APP_SOURCES := $(wildcard $(APP_SRC_DIR)/*.cpp)
APP_OBJECTS := $(patsubst $(APP_SRC_DIR)/%.cpp, $(INTERMEDIATE_DIR)/%.o, $(APP_SOURCES))

.DEFAULT_GOAL := all

run: all
	$(BIN_OUT_DIR)/application

all: application

application: $(APP_OBJECTS)
	$(CXX) \
		-L$(LIB_OUT_DIR) \
		-ldatabase_manager -Wl,-rpath,$(LIB_OUT_DIR) \
		$^ -o $(BIN_OUT_DIR)/$@

$(INTERMEDIATE_DIR)/%.o: $(APP_SRC_DIR)/%.cpp Makefile out_dirs database_manager
	$(CXX) $(CXXFLAGS) \
		-c \
		-I $(LIB_DATABASE_MANAGER_INCLUDE_DIR)/ \
		$< -o $@

database_manager: $(DATABASE_MANAGER_OBJECTS)
	$(CXX) \
		-shared \
		$^ -o $(LIB_OUT_DIR)/lib$@.so

$(INTERMEDIATE_DIR)/%.o: $(LIB_DATABASE_MANAGER_SRC_DIR)/%.cpp Makefile out_dirs
	$(CXX) $(CXXFLAGS) \
		-c \
		-fPIC \
		-I $(LIB_DATABASE_MANAGER_INCLUDE_DIR)/ \
		-D_LIB_WITH_STATE_EXPORTS \
		$< -o $@

out_dirs:
	mkdir -p $(BIN_OUT_DIR)
	mkdir -p $(LIB_OUT_DIR)
	mkdir -p $(INTERMEDIATE_DIR)

clean: 
	rm -fr $(BIN_DIR)
	rm -fr $(LIB_OUT_DIR)
	rm -fr $(INTERMEDIATE_DIR)
