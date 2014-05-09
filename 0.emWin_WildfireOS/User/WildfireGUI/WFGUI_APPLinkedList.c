
#include "WFGUI_Common.h"
#include "WFGUI_LinkedList.h"

/* ��Ҫ����Linux��������Ļ����������malloc�������� ��free���������ͷ� */

static  HANDLE_LIST hAppHead;				//����ͷ��

/**
  * @brief  hAPPLinkedlist_NewNodeΪ����һ���µ�hAPP��㣬����ռ�
  * @param  none
  * @retval ָ���½���ָ��
  */
 HANDLE_LIST* hAPPLinkedList_NewNode(void)
{
	HANDLE_LIST *app = (HANDLE_LIST *)malloc(sizeof(HANDLE_LIST));
	
	if (app == NULL)
	 {
		 DEBUG("\r\n hAPP linkedList malloc error\r\n ");
		return NULL;
	 }
	 
	 return app;
	 
}

/**
  * @brief  hAPPLinkedlist_Init��ʼ��hAPP����
  * @param  none
  * @retval none
  */
 void hAPPLinkedList_Init(void)
{
	//HANDLE_LIST * hAppHead = WFGUI_hAPPLinkedList_NewNode();
	
	INIT_LIST_HEAD(&hAppHead.listNode );

}

/**
  * @brief  hAPPLinkedList_Delɾ�������еĽ�㣬���ͷ�malloc�ռ�
  * @param  none
  * @retval none
  */
void hAPPLinkedList_Del(HANDLE_LIST *node)
{
	if (node == NULL)
	 {
		 DEBUG("\r\n hAPP linkedList free error\r\n ");
		return ;
	 }
	 
	 /* ɾ����������ļ�¼ */
	 list_del(&node->listNode);
	 
	 /* �ͷŽ���malloc�ռ� */
	 free(node);
}


/**
  * @brief hAPPLinkedList_AddTail���ӽ�㵽����
  * @param  none
  * @retval none
  */
void hAPPLinkedList_AddTail(HANDLE_LIST *node)
{
		list_add_tail(&node->listNode,&hAppHead.listNode);
}


/**
  * @brief  hAPPLinkedList_GetAppNode����APP�����ȡ������
  * @param  none
  * @retval app�������û�еĻ�����NULL
  */
HANDLE_LIST* hAPPLinkedList_GetAppNode(WM_HWIN hAPP)
{
	struct list_head *pos;	
	HANDLE_LIST *node;
	
	list_for_each(pos,&hAppHead.listNode)
	{
		/* TBD �����и�warning����֪��Ӱ��󲻴�*/		
		node = list_entry(pos,HANDLE_LIST,listNode);
		
		/* ����������app�����ͬ�Ľ�� */
		if(node->hAPP == hAPP)
		{
				return node;
		}
		
	}
	
	/* �Ҳ���������app�����ͬ�Ľ�� */
	if(pos == &hAppHead.listNode)
		return NULL;
	
	
	
}









