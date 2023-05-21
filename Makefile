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


CC	= gcc
INC	= -Iinclude
CFLAGS  = $(INC) -O2 
CWARN	= -Wall -Wextra 
CWARN  += -Wno-unused-value -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-switch #-Wno-unused
CFLAGS += $(CWARN)
CFLAGS += $(RUNTIME)


DBG	= -ggdb -Og #-O0	
CFLAGS += $(DBG)

#SYSTEM CONFIGURATION
UNAME	= $(shell uname -a | tr -c -d \[:alnum:\] | tr \[:lower:\] \[:upper:\] ) #upper uname-a
TMPDIR	= /run/user/$(shell id -u)/

objs := $(shell  grep -Eo '.*\..*:\s' Makefile | grep -v -e '@' -e PHONY -e '%' | awk -F: '{print $1}' | tr '\n:' ' ' )
all: $(objs)

srcCore 	= lib/rbtree.c
srcCoreMinimal 	= lib/rbtree_minimal.c
srcs		= $(shell find -name '*.c')

objCore 	= $(srcCore:.c=.o)
objCoreMinimal	= $(srcCoreMinimal:.c=.o)
deps		= $(srcs:.c=.d)
###

#get dependencies of sources as makefiles targets' pre-requisite
%.d:	%.c
	$(CC) -o $@ -MM $(INC) $<
	sed -i '0,/:/{s~.*.o~$(@:.d=.o)~}' $@	
	#made obj realpath to extend next implicit rule

# pull generated makefile like dependencies in this Makefile
# => they'll add pre-requisite to (implicit) matched rules of this makefile
include $(deps)
	
%.o:	%.c
	$(CC) -c -o $@  $(CFLAGS) $<



rbtree_lib.a:		$(objCore)
	ar rcs $@ $^

rbtree_minimal_lib.a:	$(srcCoreMinimal)
	ar rcs $@ $^



# TESTS
rbtree_test_full.o: $(srcCore) lib/rbtree_test.c
	$(CC) -o $@  $(CFLAGS)  $(filter-out %.h ,$^) -DAUGMENTED_TEST

rbtree_test_minimal.o: $(objCoreMinimal) lib/rbtree_test.o
	$(CC) -o $@  $(CFLAGS)  $^

clean:
	find -name '*.[od]' | xargs rm
.PHONY: all clean testAll
