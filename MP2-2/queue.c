#include "./includes/queue.h"

void initQueue(struct Queue *q)
{
    q->first = -1;
    q->last = -1;
}

bool queueIsEmpty(struct Queue *q)
{
    if(q->first == -1)
        return true;
    return false;
}

bool queueIsFull(struct Queue *q,int nmax)
{
    if(q->last == q->first - 1 || (q->first == 0 && q->last == nmax-1))
    {
        return true;
    }
    return false;
}

bool pushBackQueue(struct Queue *q, struct Message cloud[], struct Message new_message, int nmax)
{
    if(queueIsFull(q,nmax))
        return false;

    if(queueIsEmpty(q))
    {
        cloud[0] = new_message;
        q->first = 0;
        q->last = 0;
        return true;
    }
    if(q->last < nmax-1)
    {
        cloud[q->last+1] = new_message;
        q->last += 1;
    }
    else
    {
        cloud[0]=new_message;
        q->last = 0;
    }
    return true;
}

bool topQueue(struct Queue *q,struct Message cloud[],struct Message *top){
    if(queueIsEmpty(q))
        return false ;

    *top=cloud[q->first];
    return true;
}

bool popQueue(struct Queue *q,struct Message cloud[],int nmax){
     if(queueIsEmpty(q))
        return false ;

    if(q->first == q->last)
    {
        q->first = -1;
        q->last = -1;
        return true;
    }

    if(q->first < nmax-1)
        q->first += 1;
    
    else{q->first = 0;}
    return true;
}
