# vim: set filetype=make nowrap noexpandtab tabstop=4:
# Makefile
#
# PROJECT: 'MORSE' (`morse`)
# AUTHOR: J. A. Corbal (<jacorbal@gmail.com>)

## Directories
PWD   = $(CURDIR)
I_DIR = ${PWD}/include
S_DIR = ${PWD}/src
L_DIR = ${PWD}/lib
O_DIR = ${PWD}/obj
B_DIR = ${PWD}/bin

SHELL=/bin/bash

## Compiler & linker options
CC          = gcc #gcc, clang
CCSTD       = c99 #c11, c17, gnu11, gnu17
CCOPT       = 2 #0:debug; 1:optimize; 2:optimize more; 3:optimize yet more
CCOPTS      = -pedantic -pedantic-errors
CCEXTRA     = -fdiagnostics-color=always -fdiagnostics-show-location=once
CCWARN_TINY = -Wpedantic -Wall -Wextra -Wshadow -Wundef #-Werror
CCWARN_MORE = -Wwrite-strings -Wconversion -Wdouble-promotion
CCWARN_MOST = -Wno-missing-braces -Wno-missing-field-initializers \
		-Wmissing-format-attribute -Wformat-nonliteral -Wformat=2 \
		-Wcast-align -Wpointer-arith -Wstrict-overflow=5 \
		-Wstrict-prototypes -Wnested-externs -Wunreachable-code \
		-Wuninitialized -Wno-unused-parameter -Winline -Wfloat-equal \
		-Wredundant-decls -Wold-style-definition -Winit-self \
		-Wdeclaration-after-statement -Winvalid-pch -Wnull-dereference \
		-Wformat-security -Wredundant-decls -Wbad-function-cast \
		-Wincompatible-pointer-types
CCWARN_GCC = -Wlogical-op -Wstrict-aliasing=3 -Wduplicated-branches \
		-Wformat-overflow -Wformat-signedness
CCWARN      = ${CCWARN_TINY} ${CCWARN_MORE} ${CCWARN_MOST}
CCFLAGS     = ${CCOPTS} ${CCWARN} -std=${CCSTD} ${CCEXTRA} -I ${I_DIR}
LDFLAGS     = -L ${L_DIR} -lm

# Compiler: `make clean && make CC=clang` or `make clean && make CC=gcc`
CC = clang
ifeq ($(CC), clang)
    CCWARN += ${CCWARN_CLANG}
else ifeq ($(CC), gcc)
    CCWARN += ${CCWARN_GCC}
else
    $(error Unsupported compiler '$(CC)': CC only admits 'gcc' or 'clang')
endif

# Use `make DEBUG=1` to add debugging information, symbol table, etc.
# Use `make DEBUG=2` to link with the address sanitizer 
DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CCFLAGS += -DDEBUG -g3 -ggdb3 -O0
else ifeq ($(DEBUG), 2)
	CCFLAGS += -DDEBUG -g3 -ggdb3 -O0
	LDFLAGS += -fsanitize=address -fno-omit-frame-pointer -fPIC
else
	CCFLAGS += -DNDEBUG -O${CCOPT}
endif


## Makefile options
SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .h .c .o


## Files options
TARGET = ${B_DIR}/main
SRCS = $(wildcard ${S_DIR}/*.c) $(wildcard ${S_DIR}/*/*.c)
OBJS = $(patsubst ${S_DIR}/%.c, ${O_DIR}/%.o, $(SRCS))
RUN_ARGS =


## Linkage
${TARGET}: ${OBJS}
	${CC} -o $@ $^ ${LDFLAGS} 


## Compilation
${O_DIR}/%.o: ${S_DIR}/%.c
	${CC} -o $@ -c $< ${CCFLAGS}


## Make options
.PHONY: all ctags clean-obj clean-bin clean run hard hard-run doxygen help

all:
	make ${TARGET}
	@make ctags

ctags:
ifeq (,$(wildcard "/usr/bin/ctags"))
	@echo "Generating tags..."
	@ctags -R --exclude='doc' --exclude='obj' .
endif

clean-obj:
	rm --force ${OBJS}

clean-bin:
	rm --force ${TARGET}

clean:
	@make clean-obj
	@make clean-bin

run:
	${TARGET} ${RUN_ARGS}

hard:
	@make clean
	@make all

hard-run:
	@make hard
	@make run

doxygen:
	@[ -f 'Doxyfile' ] && doxygen || echo "'Doxyfile' not found" >&2

help:
	@echo "Type:"
	@echo "  'make all'......................... Build project"
	@echo "  'make run'................ Run binary (if exists)"
	@echo "  'make clean-obj'.............. Clean object files"
	@echo "  'make clean'....... Clean binary and object files"
	@echo "  'make hard'...................... Clean and build"
	@echo "  'make doxygen'...... Create Doxygen documentation"
	@echo ""
	@echo "  Binary will be placed in '${TARGET}'"
