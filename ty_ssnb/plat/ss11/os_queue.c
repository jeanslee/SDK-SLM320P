#include "os_queue.h"




/**

	*  ˵������ʼ������

	*	������

	*		queueIncreasedEvent����Ҫע��Ķ��������¼�

	*	����ֵ���ɹ��򷵻ض��е�ָ�룬ʧ���򷵻�NULL

	*/

Queue* Queue_create(uint32_t msg_count, uint32_t msg_size, QueueIncreased queueIncreasedEvent)

{

	Queue* queue = NULL;

	queue = (Queue*)lpa_malloc(sizeof(Queue));

	if(queue == NULL)

	{

		return NULL;

	}

	queue->length = 0;

	queue->head = NULL;

	queue->tail = NULL;

       queue->ItemSize =  msg_size;
       
	queue->onQueueIncreased = queueIncreasedEvent;


	return queue;

}

 

/**

	*  ˵�����Ӷ��е�ͷ�����

	*	������

	*		queue������ָ��

	*		data����ӵ�������

	*	����ֵ����ӳɹ�����0��ʧ�ܷ���-1

	*/

int Queue_AddToHead(Queue* queue,void* data)

{

	QueueNode* node = NULL;

	if(queue == NULL)

		return -1;

	//�����ڵ�

	node = (QueueNode*)lpa_malloc(sizeof(QueueNode));

	if(node == NULL)

		return -1;

	node->data = data;

	//����ǵ�һ����ӣ�ͷ����β����ָ��ǰ�ڵ�

	if(queue->tail == NULL && queue->head == NULL)

	{

		queue->tail = node;

		queue->head = node;

		node->next = NULL;

		node->prior = NULL;

	}

	else

	{

		//��ʼ��ͷ�����

		queue->head->prior = node;

		node->next = queue->head;

		node->prior = NULL;

		queue->head = node;

	}

	queue->length++;

	//�ص�����¼�

	if(queue->onQueueIncreased != NULL)

		return queue->onQueueIncreased(queue,data);

	return 0;

}

 

/**

	*  ˵�����Ӷ��е�β�����

	*	������

	*		queue������ָ��

	*		data����ӵ�������

	*	����ֵ����ӳɹ�����0��ʧ�ܷ���-1

	*/

int Queue_AddToTail(Queue* queue,void* data)

{

	QueueNode* node = NULL;

	if(queue == NULL)

		return -1;

	//�����ڵ�

	node = (QueueNode*)lpa_malloc(sizeof(QueueNode));

	if(node == NULL)

		return -1;

	node->data = data;

	//����ǵ�һ����ӣ�ͷ����β����ָ��ǰ�ڵ�

	if(queue->tail == NULL && queue->head == NULL)

	{

		queue->tail = node;

		queue->head = node;

		node->next = NULL;

		node->prior = NULL;

	}

	else

	{

		//��ʼ��β�����

		queue->tail->next = node;

		node->prior = queue->tail;

		node->next = NULL;

		queue->tail = node;

	}

	queue->length++;

	//�ص�����¼�

	if(queue->onQueueIncreased != NULL)

		return queue->onQueueIncreased(queue,data);

	return 0;

}

 

/**

	*  ˵�����Ӷ��е�ͷ��ȡ������

	*	������

	*		queue������ָ��

	*	����ֵ���ɹ�ȡ�����ݣ�ʧ�ܷ���NULL

	*/

void* Queue_GetFromHead(Queue* queue)

{

	void* data = NULL;

	QueueNode* node = NULL; 

	if(queue == NULL || queue->head == NULL)

	{

		return NULL;

	}

	node = queue->head;

	queue->head = node->next;

	if(queue->head != NULL)//���ȡ����ͷ����Ϊ�գ���ͷ������һ���ڵ���ΪNULL

	{

		queue->head->prior = NULL;

	}

	else

	{

		//��ʾ�����Ѿ�ȡ����

		queue->tail = NULL;

		queue->head = NULL;

	}

	data = node->data;

	lpa_free(node);

	queue->length--;

	return data;

}

 

 

/**

	*  ˵�����Ӷ��е�β��ȡ������

	*	������

	*		queue������ָ��

	*	����ֵ���ɹ�ȡ�����ݣ�ʧ�ܷ���NULL

	*/

void* Queue_GetFromTail(Queue* queue)

{

	void* data = NULL;

	QueueNode* node = NULL; 

	if(queue == NULL || queue->tail == NULL)

	{

		return NULL;

	}

	node = queue->tail;

	queue->tail = node->prior;

	if(queue->tail != NULL)//���ȡ����β����Ϊ�գ���β������һ���ڵ���ΪNULL

	{

		queue->tail->next = NULL;

	}

	else

	{

		//��ʾ�����Ѿ�ȡ����

		queue->tail = NULL;

		queue->head = NULL;

	}

	data = node->data;

	lpa_free(node);

	queue->length--;

	return data;

}

 

/**

	*  ˵���������ͷ�

	*	������

	*		queue������ָ��

	*		isFreeData:�Ƿ��Զ��ͷ�data��

	*/

void Queue_Free(Queue* queue)

{

	void* data = NULL;

	data = Queue_GetFromHead(queue);

	while(data != NULL)

	{

		lpa_free(data);

	}

	lpa_free(queue);

 

}

 

//���������¼�

int OnQueueIncreasedEvent(void* queue,void* data)

{

//	Queue* q = (Queue*)queue;

//	int *p = (int*)data;

	return 0;

}

void Queue_Test(void)
{

        

        bool ret = 0;
        Queue* queue = NULL;
        uint32_t msg_count = 0;
        uint32_t msg_size = 10;
        char *ldata;
        
        char *i = "1abcd";
        char *j = "2defcda";
        char *k = "3ghxl";
        char *l = "4xyzyyyyy";
        
        ldata = (char*)lpa_malloc(msg_size* sizeof(char));
       queue = (Queue*)ty_create_msg_Que(msg_count, msg_size);
       
        if(queue == NULL)
            {
                opencpu_printf("error queue test!\r\n");
                return;
            }

        ty_put_msg_que(queue, j, NULL);
        ty_put_msg_que(queue, i, NULL);
        ty_put_msg_que(queue, l, NULL);
        ty_put_msg_que(queue, k, NULL);
  

        ret = ty_get_msg_que(queue, ldata, NULL);


        while(ret != false)

        {

            opencpu_printf("%s\t", ldata);

            ret = ty_get_msg_que(queue, ldata, NULL);
        }

         opencpu_printf("\n");
        lpa_free(ldata);
        opencpu_printf("lpa_free !\r\n");


        ty_delete_msg_que(queue);


}


//����
/*
void Queue_Test()

{

	void* data = NULL;

	Queue* queue = NULL;

	int i = 0,j = 1,k = 2,l = 3;

	int* p = NULL;

	queue = Queue_Init(OnQueueIncreasedEvent);

	Queue_AddToHead(queue,&i);

	Queue_AddToHead(queue,&j);

	Queue_AddToTail(queue,&k);

	Queue_AddToTail(queue,&l);

	data = Queue_GetFromHead(queue);

	while(data != NULL)

	{

		p = (int *)data;

		//printf("%d\t",*p);

		data = Queue_GetFromHead(queue);//��ͷȡ

	}

	//printf("\n");

	Queue_Free(queue,true);

	queue = Queue_Init(NULL);

	Queue_AddToHead(queue,&i);

	Queue_AddToHead(queue,&j);

	Queue_AddToTail(queue,&k);

	Queue_AddToTail(queue,&l);

	data = Queue_GetFromHead(queue);

	while(data != NULL)

	{

		p = (int *)data;

		//printf("%d\t",*p);

		data = Queue_GetFromTail(queue);//��ͷȡ

	}

	//printf("\n");

 

	Queue_AddToTail(queue,&i);

	Queue_AddToTail(queue,&j);

	Queue_AddToTail(queue,&k);

	Queue_AddToTail(queue,&l);

	data = Queue_GetFromHead(queue);

	while(data != NULL)

	{

		p = (int *)data;

		//printf("%d\t",*p);

		data = Queue_GetFromHead(queue);//��ͷȡ

	}

	//printf("\n");

	Queue_Free(queue,true);

	//getchar();

}*/
