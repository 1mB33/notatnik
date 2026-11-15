DEBUG ?= 0
PRE_PROCESSOR_MACROS :=
LIB_DATABASE_MANAGER_INCLUDE :=
LIB_DATABASE_MANAGER_LIBS_PATHS :=
LIB_DATABASE_MANAGER_LIBS :=
LIB_DATABASE_MANAGER_INCLUDE_DIR := database_manager/include/
LIB_DATABASE_MANAGER_SRC_DIR := database_manager/src/
LIB_HTTP_INCLUDE_DIR := -I cpp-httplib/ -I json/
APP_SRC_DIR := app/src
ifeq ($(DEBUG), 1)
	BIN_OUT_DIR = bin/debug/
	LIB_OUT_DIR = lib/debug/
	INTERMEDIATE_DIR = intermediate/debug/
	PRE_PROCESSOR_MACROS = -D _DEBUG
else
	BIN_OUT_DIR = bin/
	LIB_OUT_DIR = lib/
	INTERMEDIATE_DIR = intermediate/
endif

ENV_DB_NAME := "notepad_dev_db"
ENV_DB_USER := "dev"
ENV_DB_PASS := "dev"

ifeq ($(OS),Windows_NT)
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
		LIB_DATABASE_MANAGER_LIBS += -lpq
		LIB_DATABASE_MANAGER_LIBS_PATHS += -L/opt/homebrew/opt/libpq/lib
		LIB_DATABASE_MANAGER_INCLUDE += -I /usr/include/postgresql
    endif
    ifeq ($(UNAME_S),Darwin)
		LIB_DATABASE_MANAGER_LIBS_PATHS += -L/opt/homebrew/opt/libpq/lib
		LIB_DATABASE_MANAGER_LIBS += -lpq
		LIB_DATABASE_MANAGER_INCLUDE += -I /opt/homebrew/opt/libpq/include
    endif
endif


CXX := g++
ifeq ($(DEBUG), 1)
	CXXFLAGS = -g -Wall -Wextra -std=c++17
else
	CXXFLAGS := -Wall -Wextra -O2 -std=c++17
endif
DATABASE_MANAGER_SOURCES := $(wildcard $(LIB_DATABASE_MANAGER_SRC_DIR)/*.cpp)
DATABASE_MANAGER_OBJECTS := $(patsubst $(LIB_DATABASE_MANAGER_SRC_DIR)/%.cpp, $(INTERMEDIATE_DIR)/%.o, $(DATABASE_MANAGER_SOURCES))
APP_SOURCES := $(wildcard $(APP_SRC_DIR)/*.cpp)
APP_OBJECTS := $(patsubst $(APP_SRC_DIR)/%.cpp, $(INTERMEDIATE_DIR)/%.o, $(APP_SOURCES))


.DEFAULT_GOAL := all

.PHONY: run
run: all
	$(BIN_OUT_DIR)/application

.PHONY: all
all: out_dirs database_manager application

.PHONY: run_debug
run_debug: 
	DEBUG=1 make run

.PHONY: all_debug
all_debug: 
	DEBUG=1 make all

application: $(APP_OBJECTS)
	$(CXX) \
		$^ -o $(BIN_OUT_DIR)$@ \
		-L$(LIB_OUT_DIR) \
		-ldatabase_manager -Wl,-rpath,$(LIB_OUT_DIR)

$(INTERMEDIATE_DIR)%.o: $(APP_SRC_DIR)%.cpp Makefile 
	$(CXX) \
		$< -o $@ \
		$(CXXFLAGS) \
		-c \
		$(PRE_PROCESSOR_MACROS) \
		-I $(LIB_DATABASE_MANAGER_INCLUDE_DIR) \
		$(LIB_HTTP_INCLUDE_DIR)

database_manager: $(DATABASE_MANAGER_OBJECTS)
	$(CXX) \
		$^ -o $(LIB_OUT_DIR)lib$@.so \
		-shared \
		$(LIB_DATABASE_MANAGER_LIBS) \
		$(LIB_DATABASE_MANAGER_LIBS_PATHS) \

$(INTERMEDIATE_DIR)%.o: $(LIB_DATABASE_MANAGER_SRC_DIR)%.cpp Makefile 
	$(CXX) $(CXXFLAGS) \
		$< -o $@ \
		-c \
		-fPIC \
		$(PRE_PROCESSOR_MACROS) \
		$(LIB_DATABASE_MANAGER_INCLUDE) \
		-I $(LIB_DATABASE_MANAGER_INCLUDE_DIR) \
		-D_LIB_WITH_STATE_EXPORTS

.PHONY: out_dirs
out_dirs:
	mkdir -p $(BIN_OUT_DIR)
	mkdir -p $(LIB_OUT_DIR)
	mkdir -p $(INTERMEDIATE_DIR)

.PHONY: clean
clean: 
	rm -fr $(BIN_OUT_DIR)
	rm -fr $(LIB_OUT_DIR)
	rm -fr $(INTERMEDIATE_DIR)
