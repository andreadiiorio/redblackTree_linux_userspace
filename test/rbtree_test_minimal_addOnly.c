// SPDX-License-Identifier: GPL-2.0-only
/* Copyright Andrea Di Iorio 2021
 * This file is part of RedBlackTree_linux_userspace
 * RedBlackTree_linux_userspace is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * RedBlackTree_linux_userspace is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with RedBlackTree_linux_userspace.  If not, see <http://www.gnu.org/licenses/>.
 */ 

//Userspace GNUC porting:	Andrea Di Iorio
#include <stdlib.h>
//#include <linux/module.h>         //TODO LESS_DEPENDENCIES
//#include <linux/moduleparam.h>    //TODO LESS_DEPENDENCIES
#include <linux/rbtree_augmented.h>
//#include <linux/random.h>         //TODO LESS_DEPENDENCIES 


//#include <linux/slab.h>           //TODO LESS_DEPENDENCIES 
//#include <asm/timex.h>			//TODO LESS_DEPENDENCIES 

/*#define __param(type, name, init, msg)		\
	static type name = init;		\
	module_param(name, type, 0444);		\
	MODULE_PARM_DESC(name, msg); **/ //TODO LESS_DEPENDENCIES

#include <errno.h>

int nnodes      = 1000;  //"Number of nodes in the rb-tree");
int perf_loops  = 1000; //"Number of iterations modifying the rb-tree");
int check_loops = 100;  // "Number of iterations modifying and verifying the rb-tree");

struct test_node {
	u32 key;
	struct rb_node rb;

	/* following fields used for testing augmented rbtree functionality */
	u32 val;
	u32 augmented;	///only for AUGMENTED_TEST
};
#define NODE_VAL(node) ((node)->val)

static struct rb_root_cached root = RB_ROOT_CACHED;
static struct test_node *nodes = NULL;


static void insert(struct test_node *node, struct rb_root_cached *root)
{
	struct rb_node **new = &root->rb_root.rb_node, *parent = NULL;
	u32 key = node->key;

	while (*new) {
		parent = *new;
		if (key < rb_entry(parent, struct test_node, rb)->key)
			new = &parent->rb_left;
		else
			new = &parent->rb_right;
	}

	rb_link_node(&node->rb, parent, new);
	rb_insert_color(&node->rb, &root->rb_root);
}

/**static void insert_cached(struct test_node *node, struct rb_root_cached *root)
{
	struct rb_node **new = &root->rb_root.rb_node, *parent = NULL;
	u32 key = node->key;
	bool leftmost = true;

	while (*new) {
		parent = *new;
		if (key < rb_entry(parent, struct test_node, rb)->key)
			new = &parent->rb_left;
		else {
			new = &parent->rb_right;
			leftmost = false;
		}
	}

	rb_link_node(&node->rb, parent, new);
	rb_insert_color_cached(&node->rb, root, leftmost);
}**/

///rbtree_test_init -> main globalized vars for easyness subtest divisio, n
static int i, j;
static struct rb_node *node;
static cycles_t time1, time2, time;

static void init(void)
{
	int i;
	for (i = 0; i < nnodes; i++) {
		nodes[i].key = random();	///TODO LESS_DEPENDENCIES prandom_u32_state(&rnd);
		nodes[i].val = random();	///TODO LESS_DEPENDENCIES prandom_u32_state(&rnd);
	}
}

/**TODO ONLY ADD
static bool is_red(struct rb_node *rb)
{
	return !(rb->__rb_parent_color & 1);
}

static int black_path_count(struct rb_node *rb)
{
	int count;
	for (count = 0; rb; rb = rb_parent(rb))
		count += !is_red(rb);
	return count;
}

static void check_postorder_foreach(int nr_nodes)
{
	struct test_node *cur, *n;
	int count = 0;
	rbtree_postorder_for_each_entry_safe(cur, n, &root.rb_root, rb)
		count++;

	WARN_ON_ONCE(count != nr_nodes);
}

static void check_postorder(int nr_nodes)
{
	struct rb_node *rb;
	int count = 0;
	for (rb = rb_first_postorder(&root.rb_root); rb; rb = rb_next_postorder(rb))
		count++;

	WARN_ON_ONCE(count != nr_nodes);
}

static void check(int nr_nodes)
{
	struct rb_node *rb;
	int count = 0, blacks = 0;
	u32 prev_key = 0;

	for (rb = rb_first(&root.rb_root); rb; rb = rb_next(rb)) {
		struct test_node *node = rb_entry(rb, struct test_node, rb);
		WARN_ON_ONCE(node->key < prev_key);
		WARN_ON_ONCE(is_red(rb) &&
			     (!rb_parent(rb) || is_red(rb_parent(rb))));
		if (!count)
			blacks = black_path_count(rb);
		else
			WARN_ON_ONCE((!rb->rb_left || !rb->rb_right) &&
				     blacks != black_path_count(rb));
		prev_key = node->key;
		count++;
	}

	WARN_ON_ONCE(count != nr_nodes);
	WARN_ON_ONCE(count < (1 << black_path_count(rb_last(&root.rb_root))) - 1);

	check_postorder(nr_nodes);
	check_postorder_foreach(nr_nodes);
} **/


///WRAP TESTS IN FUNCTIONS
static void rbtree_test(){
	printf("rbtree testing");
	init();

	time1 = get_cycles();

	for (j = 0; j < nnodes; j++)	insert(nodes + j, &root);

	time2 = get_cycles();
	time = time2 - time1;

	printf(" -> test 1 (latency of nnodes insert): %llu cycles\n",
	       (unsigned long long)time);
	
	time1 = get_cycles();

	for (i = 0; i < perf_loops; i++) {
		for (node = rb_first(&root.rb_root); node; node = rb_next(node))
			;
	}

	time2 = get_cycles();
	time = time2 - time1;

	time = div_u64(time, perf_loops);
	printf(" -> test 2 (latency of inorder traversal): %llu cycles\n",
	       (unsigned long long)time);

	time1 = get_cycles();

	for (i = 0; i < perf_loops; i++)
		node = rb_first(&root.rb_root);

	time2 = get_cycles();
	time = time2 - time1;

	time = div_u64(time, perf_loops);
	printf(" -> test 3 (latency to fetch first node)\n");
	printf("        non-cached: %llu cycles\n", (unsigned long long)time);

}

///static int __init rbtree_test_init(void) //TODO LESS_DEPENDENCIES
int main()
{

	nodes = calloc(nnodes, sizeof(*nodes)); ///, GFP_KERNEL); //TODO LESS_DEPENDENCIES
	if (!nodes)
		return -ENOMEM;

	srandom(199695); ///TODO LESS_DEPENDENCIES prandom_seed_state(&rnd, 3141592653589793238ULL);

	rbtree_test();

	kfree(nodes);
	//return -EAGAIN; /* Fail will directly unload the module */
	return 0;
}
/*//TODO LESS_DEPENDENCIES
static void rbtree_test_exit(void)	   ///__exit   
{
	printf("test exit\n");
}*/

/**module_init(rbtree_test_init)
module_exit(rbtree_test_exit)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michel Lespinasse");
MODULE_DESCRIPTION("Red Black Tree test"); */ ///TODO LESS_DEPENDENCIES
