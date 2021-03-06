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
 * File: main.c
 *
 * Created on: Jun 5, 2015
 * Author: Tianfu Ma (matianfu@gmail.com)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../include/errno.h"
#include "../include/data_type.h"
#include "../include/alloc.h"
#include "../include/basefunc.h"
#include "../include/struct_deal.h"
#include "../include/memdb.h"
#include "../include/message.h"


int read_json_file(char * file_name)
{
	int ret;

	int fd;
	int readlen;
	int json_offset;

	int struct_no=0;
	void * root_node;
	void * findlist;
	void * memdb_template ;
	BYTE uuid[DIGEST_SIZE];
	char json_buffer[4096];

	fd=open(file_name,O_RDONLY);
	if(fd<0)
		return fd;

	readlen=read(fd,json_buffer,4096);
	if(readlen<0)
		return -EIO;
	json_buffer[readlen]=0;
	printf("%s\n",json_buffer);
	close(fd);

	json_offset=0;
	while(json_offset<readlen)
	{
		ret=json_solve_str(&root_node,json_buffer+json_offset);
		if(ret<0)
		{
			printf("solve json str error!\n");
			break;
		}
		json_offset+=ret;
		if(ret<32)
			continue;

		ret=memdb_read_desc(root_node,uuid);
		if(ret<0)
			break;
		struct_no++;
	}

	return struct_no;
}


int main() {

  	static unsigned char alloc_buffer[4096*(1+1+4+1+16+1+256)];	
	char json_buffer[4096];
	char print_buffer[4096];
	int ret;
	int readlen;
	int json_offset;
	void * root_node;
	void * findlist;
	void * memdb_template ;
	BYTE uuid[DIGEST_SIZE];
	int i;
	MSG_HEAD * msg_head;
	
	char * baseconfig[] =
	{
		"typelist.json",
		"subtypelist.json",
		"msghead.json",
		"headrecord.json",
		NULL
	};

	alloc_init(alloc_buffer);
	struct_deal_init();
	memdb_init();

// test namelist reading start

	for(i=0;baseconfig[i]!=NULL;i++)
	{
		ret=read_json_file(baseconfig[i]);
		if(ret<0)
			return ret;
		printf("read %d elem from file %s!\n",ret,baseconfig[i]);
	}

	void * record;

// test struct desc reading start
	
	int msg_type = memdb_get_typeno("MESSAGE");
	if(msg_type<=0)
		return -EINVAL;

	int subtype=memdb_get_subtypeno(msg_type,"HEAD");
	if(subtype<=0)
		return -EINVAL;

	record=memdb_get_first(msg_type,subtype);
	while(record!=NULL)
	{
		ret=memdb_print(record,print_buffer);
		if(ret<0)
			return -EINVAL;
		printf("%s\n",print_buffer);
		record=memdb_get_next(msg_type,subtype);
	}

	msgfunc_init();
	
	void * message;

	message=message_create(512,1,NULL);	

	ret=Galloc0(&msg_head,sizeof(MSG_HEAD));
	if(msg_head==NULL)
		return -EINVAL;
	Strcpy(msg_head->sender_uuid,"Test sender");	
	Strcpy(msg_head->receiver_uuid,"Test receiver");
	ret=message_add_record(message,msg_head);
	if(ret<0)
	{
		printf("add message head record failed!\n");
		return ret;
	}	

	ret=message_record_struct2blob(message);
	if(ret<0)
	{
		printf("message struct2blob failed!\n");
		return ret;
	}	

	BYTE * blob;
	
	message_output_blob(message,&blob);
	ret=message_output_json(message,json_buffer);
	if(ret<0)
	{
		printf("message output json failed!\n");
		return ret;
	}

	printf("%s\n",json_buffer);
	
	
	void * new_msg;

	ret=json_2_message(json_buffer,&new_msg);

	if(ret<0)
		return ret;

	printf ("read % from json_buffer\n",ret);

	ret=message_output_json(message,json_buffer);
	if(ret<0)
	{
		printf("message output json failed!\n");
		return ret;
	}

	printf("%s\n",json_buffer);
	

	
	return 0;

}
