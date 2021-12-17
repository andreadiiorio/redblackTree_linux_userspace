#DEV ANDREA DI IORIO
CC	    = gcc
CFLAGS  = -O2 -Iinclude -Wall -Wextra 
CFLAGS += -Wno-unused-value -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-switch #-Wno-unused
CFLAGS+ = $(RUNTIME)
DBG		= -ggdb -Og #-O0	

#SYSTEM CONFIGURATION
UNAME	= $(shell uname -a | tr -c -d \[:alnum:\] | tr \[:lower:\] \[:upper:\] ) #upper uname-a
TMPDIR	= /run/user/$(shell id -u)/

objs := $(shell  grep -Eo '.*\..*:\s' Makefile | grep -v -e '@' -e PHONY | awk -F: '{print $1}' | tr '\n:' ' ' )
all: $(objs)

srcCore 		= lib/rbtree.c include/linux/rbtree.h include/linux/rbtree_augmented.h include/macros.h
srcCoreMinimal 	= lib/rbtree_minimal.c include/linux/rbtree_minimal.h include/macros.h

rbtree_test_full.o: $(srcCore) lib/rbtree_test.c
	$(CC) -o $@  $(CFLAGS) $(DBG) $(filter-out %.h ,$^) -DAUGMENTED_TEST

rbtree_test_minimal.o: $(srcCoreMinimal) lib/rbtree_test.c
	$(CC) -o $@  $(CFLAGS) $(DBG) $(filter-out %.h ,$^) 

clean:
	rm -i "*.o"
.PHONY: all clean testAll
