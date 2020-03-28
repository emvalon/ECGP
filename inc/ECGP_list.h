/**
  ******************************************************************************
  * File Name          : ECGP_list.c
  * Description        : linked list function
  ******************************************************************************

  * Copyright(c) 2019 Valon Shen (valonshen@foxmail.com)
  * All rights reserved.
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
