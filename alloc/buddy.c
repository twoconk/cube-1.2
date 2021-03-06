/**
 * Copyright [2015] Tianfu Ma (matianfu@gmail.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * File: buddy.c
 *
 * Created on: Jun 5, 2015
 * Author: Tianfu Ma (matianfu@gmail.com)
 */

#include "../include/errno.h"
#include "../include/data_type.h"
#include "../include/alloc.h"
#include "../include/string.h"
#include "buddy.h"

void buddy_clear(buddy_t * buddy) {
  	Memset(buddy->freelist,0,sizeof(void *)*(buddy->order+2)+buddy->poolsize);
  	buddy->freelist[buddy->order] = buddy->pool;
	return;
}

void buddy_reset(buddy_t * buddy) {
  	Memset(buddy->freelist,0,sizeof(void *)*(buddy->order+2));
  	buddy->freelist[buddy->order] = buddy->pool;
	return;
}

void * bmalloc(int size,buddy_t * buddy) {

	int i, order;
	void * block;
	void * buddymem;

  // calculate minimal order for this size
	i = MIN_ORDER;
	while (BLOCKSIZE(i) < size + 1) // one more byte for storing order
		i++;

	order = i;

  // level up until non-null list found
	for (;; i++) {
  		if (i > buddy->order)
			return NULL;
    		if (buddy->freelist[i])
      			break;
  	}

  // remove the block out of list
 	block = buddy->freelist[i];
 	buddy->freelist[i] = *(void * *) buddy->freelist[i];

  // split until i == order
 	while (i-- > order) {
		buddymem = buddyof(block, i,buddy);
    		buddy->freelist[i] = buddymem;
	}

  // store order in previous byte
 	*((BYTE*) (block - 1)) = order;
	return block;
}
void * bmalloc0(int size,buddy_t * buddy) 
{
	void * pointer=bmalloc(size,buddy);
	if(pointer!=NULL)
	{
		Memset(pointer,0,size);
	}
	return pointer;
	
}

void bfree(void * block,buddy_t * buddy) {

	int i;
	void * buddymem;
	void * * p;

  // fetch order in previous byte
	i = *((BYTE*) (block - 1));

	for (;; i++) {
    // calculate buddy
		buddymem = buddyof(block, i,buddy);
		p = &(buddy->freelist[i]);

    // find buddy in list
		while ((*p != NULL) && (*p != buddymem))
			p = (void * *) *p;

    // not found, insert into list
  		if (*p != buddymem) {
  			*(void **) block = buddy->freelist[i];
      			buddy->freelist[i] = block;
      			return;
		}
    // found, merged block starts from the lower one
    		block = (block < buddymem) ? block : buddymem;
    // remove buddy out of list
    		*p = *(void * *) *p;
  	}
	return;
}

void bfree0(void * pointer,buddy_t * buddy) 
{
	int i;
	i = *((BYTE*) (pointer - 1));
	if(pointer!=NULL)
	{
		Memset(pointer,0,BLOCKSIZE(i));
	}
	bfree(pointer,buddy);
}

/*
 * The following functions are for simple tests.
 */

static int count_blocks(int i,buddy_t * buddy) {

  int count = 0;
  void * * p = &(buddy->freelist[i]);

  while (*p != NULL) {
    count++;
    p = (void **) *p;
  }
  return count;
}

int total_free(buddy_t * buddy) {

  int i, bytecount = 0;

  for (i = MIN_ORDER; i <= buddy->order; i++) {
    bytecount += count_blocks(i,buddy) * BLOCKSIZE(i);
  }
  return bytecount;
}

int ispointerinbuddy(void * pointer,buddy_t * buddy)
{
	int offset=pointer-(void *)buddy->pool;
	if((offset>0) && (offset<buddy->poolsize))
		return 1;
	return 0;
}
/*
static void print_list(int i,buddy_t * buddy) {

  printf("freelist[%d]: \n", i);

  void **p = &buddy->freelist[i];
  while (*p != NULL) {
    printf("    0x%08lx, 0x%08lx\n", (uintptr_t) *p, (uintptr_t) *p - (uintptr_t) buddy->pool);
    p = (void **) *p;
  }
}
*/
