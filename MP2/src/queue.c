#include "./includes/queue.h"



void initqueue(struct Queue *q){
    q->primeiro=-1;
    q->ultimo=-1;
}


bool queueIsEmpty(struct Queue *q){
    if(q->primeiro==-1)
        return true;
    return false;
}

bool queueIsFull(struct Queue *q,int nmax){
    if(q->ultimo==q->primeiro-1 || (q->primeiro==0 && q->ultimo==nmax-1)){
        return true;
    }
    return false;
}

bool pushbackqueue(struct Queue *q,struct Message armazem[],struct Message new_message,int nmax){
    
    if(queueIsFull(q,nmax))
        return false;

    if(q->ultimo<nmax-1)
        armazem[q->ultimo+1]=new_message;
    
    else{
         armazem[0]=new_message;
    }

    return true;
}

bool topQueue(struct Queue *q,struct Message armazem[],struct Message *top){
    if(queueIsEmpty(q))
        return false ;

    *top=armazem[q->primeiro];
    return true;
}

bool popQueue(struct Queue *q,struct Message armazem[],int nmax){
     if(queueIsEmpty(q))
        return false ;

    if(q->primeiro==q->ultimo){
        q->primeiro=-1;
        q->ultimo=-1;
        return true;
    }

    if(q->primeiro<nmax-1)
        q->primeiro+=1;
    
    else{q->primeiro=0;}
    return true;
}
