/**
 * @file ECGP_list.h
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
#ifndef _ECGP_LIST_H_
#define _ECGP_LIST_H_ 

#include <stdbool.h>

typedef struct ECGP_Node{
	struct ECGP_Node* next;
}ECGP_Node_t;

typedef ECGP_Node_t ECGP_List_t;

bool ECGP_listIsEmpty(ECGP_List_t *list);
void ECGP_listClear(ECGP_List_t *list);
void ECGP_insertAfter(ECGP_Node_t* node, ECGP_Node_t* newNode);
ECGP_Node_t* ECGP_listPopFirstNode(ECGP_List_t* list);
ECGP_Node_t* ECGP_listGetFirstNode(ECGP_List_t *list);
ECGP_Node_t* ECGP_listGetLastNode(ECGP_List_t *list);
ECGP_Node_t* ECGP_listGetNextNode(ECGP_Node_t* node);
void ECGP_listAddNode(ECGP_List_t *list, ECGP_Node_t* newNode);
void ECGP_listAddNodeFront(ECGP_List_t* list, ECGP_Node_t* newNode);
void ECGP_listDeleteNode(ECGP_List_t* list, ECGP_Node_t* node);
#endif
