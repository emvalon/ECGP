/**
 * @file ECGP_list.c
 * @author Weilong Shen (valonshen@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-12
 * 
 * Copyright © 2021 - 2022 Weilong Shen (valonshen@foxmail.com)
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */
#include <misc/ECGP_list.h>
#include <ECGP_common.h>
#include <stdlib.h>

ECGP_Node_t* ECGP_listGetFirstNode(ECGP_List_t *list)
{
	return list->next;
}

bool ECGP_listIsEmpty(ECGP_List_t *list)
{
	if (ECGP_listGetFirstNode(list) != NULL) {
		return false;
	}
	else{
		return true;
	}
}

void ECGP_listClear(ECGP_List_t* list)
{
	list->next = NULL;
}

void ECGP_listMoveTo(ECGP_List_t *srcList, ECGP_List_t *dList)
{

}

void ECGP_insertAfter(ECGP_Node_t *node, ECGP_Node_t *newNode)
{
	newNode->next = node->next;
	node->next = newNode;
}

ECGP_Node_t* ECGP_listPopFirstNode(ECGP_List_t *list)
{
	ECGP_Node_t *temp;
	if (ECGP_listIsEmpty(list)) {
		return NULL;
	}
	else {
		temp = ECGP_listGetFirstNode(list);
		list->next = temp->next;
		temp->next = NULL;
		return temp;
	}
}

ECGP_Node_t* ECGP_listGetLastNode(ECGP_List_t *list)
{
	ECGP_Node_t *temp;

	temp = ECGP_listGetFirstNode(list);
	if (temp != NULL) {
		while (temp->next !=  NULL) {
			temp = temp->next;
		} 
		return temp;
	}
	else {
		return NULL;
	}
}

ECGP_Node_t* ECGP_listGetNextNode(ECGP_Node_t *node)
{
	return node->next;
}

void ECGP_listAddNode(ECGP_List_t *list, ECGP_Node_t *newNode)
{
	ECGP_Node_t *last;

	last = ECGP_listGetLastNode(list);
	if (last == NULL) {
		list->next = newNode;
	}
	else {
		last->next = newNode;
	}
	newNode->next = NULL;
}

void ECGP_listAddNodeFront(ECGP_List_t* list, ECGP_Node_t* newNode)
{
	newNode->next = list->next;
	list->next = newNode;
}

void ECGP_listDeleteNode(ECGP_List_t *list, ECGP_Node_t *node)
{
	ECGP_Node_t *preNode;

	preNode = list;
	
	while (preNode->next != NULL) {
		if (preNode->next == node) {
			preNode->next = node->next;
			node->next = NULL;
			return;
		}
		preNode = preNode->next;
	}
}


