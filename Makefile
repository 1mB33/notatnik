DEBUG ?= 0
LIB_DATABASE_MANAGER_INCLUDE :=
LIB_DATABASE_MANAGER_LIBS_PATHS :=
LIB_DATABASE_MANAGER_LIBS :=
LIB_DATABASE_MANAGER_INCLUDE_DIR := database_manager/include
LIB_DATABASE_MANAGER_SRC_DIR := database_manager/src
APP_SRC_DIR := app/src
ifeq ($(DEBUG), 1)
	BIN_OUT_DIR = bin/debug
	LIB_OUT_DIR = lib/debug
	INTERMEDIATE_DIR = intermediate/debug
else
	BIN_OUT_DIR = bin
	LIB_OUT_DIR = lib
	INTERMEDIATE_DIR = intermediate
endif

ENV_DB_NAME := "notepad_dev_db"
ENV_DB_USER := "dev"
ENV_DB_PASS := "dev"

ifeq ($(OS),Windows_NT)
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
		LIB_DATABASE_MANAGER_LIBS += -lpq
    endif
    ifeq ($(UNAME_S),Darwin)
		LIB_DATABASE_MANAGER_LIBS_PATHS += -L/opt/homebrew/opt/libpq/lib
		LIB_DATABASE_MANAGER_LIBS += -lpq
		LIB_DATABASE_MANAGER_INCLUDE += /opt/homebrew/opt/libpq/include
    endif
endif


CXX := g++
ifeq ($(DEBUG), 1)
	CXXFLAGS = -g -Wall -Wextra 
else
	CXXFLAGS := -Wall -Wextra -O2
endif
DATABASE_MANAGER_SOURCES := $(wildcard $(LIB_DATABASE_MANAGER_SRC_DIR)/*.cpp)
DATABASE_MANAGER_OBJECTS := $(patsubst $(LIB_DATABASE_MANAGER_SRC_DIR)/%.cpp, $(INTERMEDIATE_DIR)/%.o, $(DATABASE_MANAGER_SOURCES))
APP_SOURCES := $(wildcard $(APP_SRC_DIR)/*.cpp)
APP_OBJECTS := $(patsubst $(APP_SRC_DIR)/%.cpp, $(INTERMEDIATE_DIR)/%.o, $(APP_SOURCES))


.DEFAULT_GOAL := all

run: all
	$(BIN_OUT_DIR)/application

all: application

run_debug: 
	DEBUG=1 make run

all_debug: 
	DEBUG=1 make all

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
		$(LIB_DATABASE_MANAGER_LIBS) \
		$(LIB_DATABASE_MANAGER_LIBS_PATHS) \
		$^ -o $(LIB_OUT_DIR)/lib$@.so

$(INTERMEDIATE_DIR)/%.o: $(LIB_DATABASE_MANAGER_SRC_DIR)/%.cpp Makefile out_dirs
	$(CXX) $(CXXFLAGS) \
		-c \
		-fPIC \
		-I $(LIB_DATABASE_MANAGER_INCLUDE)/ \
		-I $(LIB_DATABASE_MANAGER_INCLUDE_DIR)/ \
		-D_LIB_WITH_STATE_EXPORTS \
		$< -o $@

out_dirs:
	mkdir -p $(BIN_OUT_DIR)
	mkdir -p $(LIB_OUT_DIR)
	mkdir -p $(INTERMEDIATE_DIR)

clean: 
	rm -fr $(BIN_OUT_DIR)
	rm -fr $(LIB_OUT_DIR)
	rm -fr $(INTERMEDIATE_DIR)
