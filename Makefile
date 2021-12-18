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

CC	    = gcc
CFLAGS  = -O2 -Iinclude -Wall -Wextra 
CFLAGS += -Wno-unused-value -Wno-unused-parameter -Wno-unused-variable -Wno-switch #-Wno-unused-function -Wno-unused
CFLAGS += $(RUNTIME)
DBG		= -ggdb -Og #-O0	

#SYSTEM CONFIGURATION
UNAME	= $(shell uname -a | tr -c -d \[:alnum:\] | tr \[:lower:\] \[:upper:\] ) #upper uname-a
TMPDIR	= /run/user/$(shell id -u)/

objs := $(shell  grep -Eo '.*\..*:\s' Makefile | grep -v -e '@' -e PHONY | awk -F: '{print $1}' | tr '\n:' ' ' )
all: $(objs)

srcCore 		= lib/rbtree.c include/linux/rbtree.h include/linux/rbtree_augmented.h include/macros.h
srcCoreMinimal 	= lib/rbtree_minimal.c include/linux/rbtree_minimal.h include/macros.h

rbtree_test_full.o: $(srcCore) test/rbtree_test.c
	$(CC) -o $@ $(filter-out %.h ,$^)  $(CFLAGS) $(DBG) -DAUGMENTED_TEST

rbtree_test_minimal.o: $(srcCoreMinimal) test/rbtree_test.c
	$(CC) -o $@ $(filter-out %.h ,$^)  $(CFLAGS) $(DBG) 
#TODO EXPERIMENTAL
srcAddOnly = lib/rbtree_minimal_generic_addOnly.c include/linux/rbtree_minimal_generic_addOnly.h include/macros.h test/rbtree_test_minimal_addOnly.c
rbtree_test_minimal_addOnly.o: $(srcAddOnly)
	$(CC) -o $@  $(filter-out %.h ,$^) $(CFLAGS) $(DBG)
#https://stackoverflow.com/questions/54996229/is-ffunction-sections-fdata-sections-and-gc-sections-not-working
DISCARDED_FUNCS_LOG="discardedFuncs.list"
getUnusedFuncs_ld_mapfile: $(srcAddOnly)
	rm -f *.o	#clean
	$(CC) -c $(filter-out %.h ,$^) $(CFLAGS) \
	  -ffunction-sections -ffunction-sections -fdata-sections -fno-inline-small-functions -O0
	$(CC) -Wl,--gc-sections *.o -Wl,-Map=mapfile
	echo "KEEP ONLY DISCARDED SECTION OF MAPFILE" && sleep 2
	vi mapfile
	echo "Grepping unused functions from the whole mapfile in $(DISCARDED_FUNCS_LOG) (you may want to use this command just on the discarded section)"
	grep  "text\." mapfile | awk '{print $$1}' | awk -F'\.' '{print $$3}' | tee $(DISCARDED_FUNCS_LOG) 
	echo  "grepping discarded functions line numbers -> only first occurrence likelly to be the definition"
	cat $(DISCARDED_FUNCS_LOG) | xargs -n 1 -I% sh -c 'grep -m 1 -Rn % $(srcAddOnly) | head -1' | tee $(DISCARDED_FUNCS_LOG).grep
clean:
	rm -i *.o
.PHONY: all clean testAll
