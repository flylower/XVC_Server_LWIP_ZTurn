/*
 * Queue.h
 *
 *  Created on: 2019Äê4ÔÂ8ÈÕ
 *      Author: WangFule
 */

#ifndef SRC_QUEUE_H_
#define SRC_QUEUE_H_

typedef struct DataP
{
	unsigned int id;
	void *payload;
	unsigned short len;
} DP;

typedef  DP QueueElementDataType;

typedef struct Queue
{
	QueueElementDataType * array;
    int front;
    int tail;
}QUEUE,*PQUEUE;

void Init_queue(PQUEUE);
void In_queue(PQUEUE,QueueElementDataType val);
void Out_queue(PQUEUE,QueueElementDataType* val);
void Traverse_queue(PQUEUE);
int Isempty(PQUEUE);
int Isfull(PQUEUE);


#endif /* SRC_QUEUE_H_ */
