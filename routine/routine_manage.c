#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../include/data_type.h"
#include "../include/list.h"
#include "../include/string.h"
#include "../include/alloc.h"
#include "../include/json.h"
#include "../include/struct_deal.h"
#include "../include/basefunc.h"
#include "../include/memdb.h"
#include "../include/message.h"
#include "../include/routine.h"

#include "routine_internal.h"

static ROUTINE * manage_proc;

int _routine_manage_init()
{
	int ret;
	ret=Galloc0(&manage_proc,sizeof(ROUTINE));
	if(ret<0)
		return ret;	
	Strncpy(manage_proc->name,"manage_proc",DIGEST_SIZE);

	ret=comp_proc_uuid(myproc_context->uuid,manage_proc->name,manage_proc->uuid);
	if(ret<0)
		return -EINVAL;
	return 0;
}

int _routine_manage_start()
{
	int ret;
	int count=0;
	ROUTINE * subroutine;
	subroutine =_subroutine_getfirst();

	while(subroutine!=NULL)
	{
		if((subroutine->state==ROUTINE_INIT)
			&&(subroutine->type==ROUTINE_SOURCE))
		{
			count++;
			ret=subroutine->ops->init(subroutine,NULL);
			if(ret<0)
			{
				printf("subroutine %s init err %d!\n",subroutine->name,ret);
			}
			subroutine->state=ROUTINE_READY;
		}
		subroutine=_subroutine_getnext();
	}
	return count;
}
