#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "xil_printf.h"
#include "Queue.h"

void Init_queue(PQUEUE Q)
{
    Q->array = (QueueElementDataType *)malloc(sizeof(QueueElementDataType)*8);
    if(Q->array == NULL)
    {
        printf("Dynamic memory allocation failed!\n");
        exit(0);
    }
    Q->front = 0;
    Q->tail = 0;
    return;
}

void In_queue(PQUEUE Q,QueueElementDataType val)
{
   if(Isfull(Q))
   {
       xil_printf("%s-%d- Failure to join the team! The queue is full.\n", val.payload, val.len);
   }else
   {
       Q->array[Q->tail] = val;
       Q->tail = (Q->tail+1)%8;
//       xil_printf("Inqueue: %d:%d:%d:%s\n\r", Q->tail, val.id, val.len, val.payload);
   }
}

void Out_queue(PQUEUE Q,QueueElementDataType* val)
{
    if(Isempty(Q))
    {
         xil_printf("Defeating failed! There are no elements in the queue.\n");
    }else
    {
        *val = Q->array[Q->front];
        Q->front = (Q->front+1)%8;
//        xil_printf("Outqueue: %d:%d:%d:%c\n\r", Q->front, val->id, val->len, *((char *)val->payload));
    }
}

void Traverse_queue(PQUEUE Q)
{
    int val = Q->front;
    while(val != Q->tail)
    {
        xil_printf("%s, %d",Q->array[val].payload, Q->array[val].len);
        val = (val+1)%8;
    }
    xil_printf("\n");

}

int Isempty(PQUEUE Q)
{
    if(Q->front ==  Q->tail)
    {
        return 1;
    }else
    {
        return 0;
    }
}

int Isfull(PQUEUE Q)
{
    if((Q->tail+1)%8 == Q->front)
    {
        return 1;
    }else
    {
        return 0;
    }
}
