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
 * Author: Hu jun (algorist@bjut.edu.cn)
 */

#include "../include/errno.h"
#include "../include/data_type.h"
#include "../include/alloc.h"
#include "buddy.h"

void *  Calloc(int size)
{
	return bmalloc(size,C_mem_struct);
}
void *  Calloc0(int size)
{
	return bmalloc0(size,C_mem_struct);
}
void Cmemdestroy()
{
	buddy_destroy(C_mem_struct);
}
int Cgetfreecount()
{
	return total_free(C_mem_struct);
}
