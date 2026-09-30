# Compilateur et options
CC = gcc
CC_WIN = x86_64-w64-mingw32-gcc
AR = ar
AR_WIN = x86_64-w64-mingw32-ar
CFLAGS = -Wall -Wextra -fPIC -I./include -I./include/configurator
LFLAGS = -pthread

# Chemins
DIR_SRC 	= src
DIR_BUILD	= build
DIR_EXAMPLE = examples
DIR_INCLUDE = include/configurator
DIR_TEST 	= tests

# Fichiers cibles
SRCS = $(DIR_SRC)/configurator.c $(DIR_SRC)/config_parser.c
OBJS = $(DIR_BUILD)/configurator.o $(DIR_BUILD)/config_parser.o

LIB_NAME = libconfigurator
STATIC_LIB = $(DIR_BUILD)/$(LIB_NAME).a
STATIC_LIB_WIN = $(DIR_BUILD)/$(LIB_NAME)_win.a
SHARED_LIB = $(DIR_BUILD)/$(LIB_NAME).so
EXAMPLES = $(DIR_BUILD)/single_thread_dyn.e $(DIR_BUILD)/single_thread_sta.e
TEST = $(DIR_BUILD)/test_configurator.e

# Règles
.PHONY: all clean prebuild

all : prebuild $(STATIC_LIB) $(SHARED_LIB) $(EXAMPLES)

windows : clean prebuild
	$(MAKE) CC=$(CC_WIN) AR=$(AR_WIN) STATIC_LIB=$(STATIC_LIB_WIN) $(STATIC_LIB_WIN)
	@mkdir -p $(DIR_BUILD)/Configurator
	@mkdir -p $(DIR_BUILD)/Configurator/lib
	@mkdir -p $(DIR_BUILD)/Configurator/include
	@mv $(STATIC_LIB_WIN) $(DIR_BUILD)/Configurator/lib
	@cp $(DIR_INCLUDE)/configurator.h $(DIR_BUILD)/Configurator/include

test : prebuild $(TEST)

prebuild :
	@mkdir -p $(DIR_BUILD)
	@cp $(DIR_EXAMPLE)/config.cfg $(DIR_BUILD)/ 

$(DIR_BUILD)/%.o : $(DIR_SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(STATIC_LIB) : $(OBJS)
	$(AR) rcs $@ $^

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