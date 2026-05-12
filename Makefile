# Compilateur et options
CC = gcc
AR = ar
CFLAGS = -Wall -Wextra -fPIC -I./include -I./include/configurator
LFLAGS = -pthread

# Chemins
DIR_SRC 	= src
DIR_BUILD	= build
DIR_EXAMPLE = examples
DIR_TEST 	= tests

# Fichiers cibles
SRCS = $(DIR_SRC)/configurator.c $(DIR_SRC)/config_parser.c
OBJS = $(DIR_BUILD)/configurator.o $(DIR_BUILD)/config_parser.o

LIB_NAME = libconfigurator
STATIC_LIB = $(DIR_BUILD)/$(LIB_NAME).a
SHARED_LIB = $(DIR_BUILD)/$(LIB_NAME).so
EXAMPLES = $(DIR_BUILD)/single_thread_dyn.e $(DIR_BUILD)/single_thread_sta.e
TEST = $(DIR_BUILD)/test_configurator.e

# Règles
.PHONY: all clean prebuild

all : prebuild $(STATIC_LIB) $(SHARED_LIB) $(EXAMPLES)

test : prebuild $(TEST)

prebuild :
	@mkdir -p $(DIR_BUILD)
	@cp $(DIR_EXAMPLE)/config.cfg $(DIR_BUILD)/ 

$(DIR_BUILD)/%.o : $(DIR_SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(STATIC_LIB) : $(OBJS)
	ar rcs $@ $^

$(SHARED_LIB) : $(OBJS)
	$(CC) -shared -o $@ $^

$(DIR_BUILD)/single_thread_dyn.e : $(DIR_EXAMPLE)/single_thread.c $(SHARED_LIB)
	$(CC) $(CFLAGS) $< -L$(DIR_BUILD) -Wl,-rpath='$$ORIGIN' -lconfigurator $(LDFLAGS) -o $@

$(DIR_BUILD)/single_thread_sta.e : $(DIR_EXAMPLE)/single_thread.c $(STATIC_LIB)
	$(CC) $(CFLAGS) $^ -o $@

$(TEST) : $(DIR_TEST)/test_configurator.c $(STATIC_LIB)
	$(CC) $(CFLAGS) $^ -o $@ -lcriterion

clean :
	rm -rf $(DIR_BUILD)