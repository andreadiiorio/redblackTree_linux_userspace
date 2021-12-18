LINUX KERNEL RED BLACK TREE USERSPACE PORTING
=============================================
Andrea Di Iorio

Linux Kernel 5.10.85 LTS release red-black tree porting in userspace
fulfilling other modules dependencies of macros and functions with 
equivalent effect macros

Easy compile with make 
#Tests
Used kernel rbtree_test module to test correct Userspace porting

#Minimal version 
added a minimal version of rb_tree implementation without the dependencies to
augmented version of the linux kernel rb_tree 
( extra info, subtree depended, stored in each node )
for easier integration thanks to just 1 header: rbtree_minimal.h  dependent only to my macro.h 
and a source: rbtree_minimal.c  dependent only on these 2 headers

#Minimal Generic implementation - only Add&traverse functionality
##TODO generic multiImplement (template in other repo) work in progress, see other branch
Over the minimal version above, removed every function 
that is not for insert and traverse the rb_tree
-This has been done thanks to a minimalized test source plus an advanced Makefile target
to find unused function -> optimized out -
