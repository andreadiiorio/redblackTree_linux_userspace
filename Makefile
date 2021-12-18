## Copyright Andrea Di Iorio 2021
## This file is part of RedBlackTree_linux_userspace
## RedBlackTree_linux_userspace is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
## 
## RedBlackTree_linux_userspace is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with RedBlackTree_linux_userspace.  If not, see <http://www.gnu.org/licenses/>.
## 

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
