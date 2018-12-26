#include "monitor/watchpoint.h"
#include "monitor/expr.h"


#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_, *head_f;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}


void new_wp(char *e, int result){
	assert(free_ != NULL);
	WP *new = free_;
	free_ = free_->next;
	strcpy(new->exp,e);
	new->result = result;
	head_f = new;
	if(new->NO == 0){
		head = new;
	}
	printf("create a new watchpoint: NO:%d, result: %d\n", new->NO, new->result);

}

/* TODO: Implement the functionality of watchpoint */

void free_wp(int i){
	if(head == NULL){
		printf("no used watchpoint\n");
		return;
	}
	WP *new_pre = head;
	WP *p1 = head;
	if(head->NO == i){
		new_pre->next = free_;
		free_ = new_pre;
		head = head->next;
		head_f->next =free_;
		printf("the NO of: %d has deleted!\n", i);
	}
	else{
		while(new_pre->NO != i){
			p1 = new_pre;
			new_pre = new_pre->next;
		}
		if(new_pre->next == free_){
			free_ = new_pre;
			head_f->next =free_;
			printf("the NO of: %d has deleted!\n", i);
		}
		else{
			p1->next = new_pre->next;
			new_pre->next = free_;
			free_ = new_pre;
			head_f->next =free_;
			printf("the NO of: %d has deleted!\n", i);
		}
		
	}
}

void print_wp(){
	WP *node = head;
	if(node == free_){
		printf("no watchpoint\n");
		return;
	}
	while(node != free_){
		printf("%-3d %10s %d|0x%x\n", node->NO, node->exp,node->result, node->result);
		node = node->next;
	}
}

bool check_wp(){
	WP *node = head;
	bool r = false;
	while(node != head_f){
		int reset = expr(node->exp);
		if(reset != node->result){
			node->result = reset;
			r = true;
		}
		node = node->next;
	}
	return r;
}